// 需要安装的依赖:
// npm install pako @types/pako gbk.js

import * as pako from 'pako';
import * as GBK from 'gbk.js';

// 辅助工具类
class ListUtil {
    static insertIntoUint8Array(target: Uint8Array, offset: number, data: Uint8Array): Uint8Array {
        const newArray = new Uint8Array(target.length + data.length);
        newArray.set(target.subarray(0, offset), 0);
        newArray.set(data, offset);
        newArray.set(target.subarray(offset), offset + data.length);
        return newArray;
    }

    static removeRange(target: Uint8Array, start: number, length: number): Uint8Array {
        const newArray = new Uint8Array(target.length - length);
        newArray.set(target.subarray(0, start), 0);
        newArray.set(target.subarray(start + length), start);
        return newArray;
    }

    static deleteFromUint8Array(target: Uint8Array, start: number, length: number): Uint8Array {
        return this.removeRange(target, start, length);
    }
}

// MRP 文件项类
class FileItem {
    constructor(
        public filename: string,
        public filenameLen: number,
        public fileOffset: number,
        public fileLen: number,
        public fileData: Uint8Array | null
    ) {}

    toString(): string {
        return `文件: ${this.filename} (偏移: ${this.fileOffset}, 长度: ${this.fileLen})`;
    }
}

// MRP 文件信息类
export class MrpInfo {
    // 原始 MRP 文件数据
    data: Uint8Array;

    // 文件基本信息
    path: string = "";
    magic: string = "MRPG";

    // 文件结构信息
    fileStart: number = 0;
    mrpTotalLen: number = 0;
    mrpHeaderSize: number = 0;

    // 应用信息
    fileName: string = "";
    displayName: string = "";
    authStr: string = "";
    appid: number = 0;
    version: number = 0;
    flag: number = 3;
    builderVersion: number = 10002;
    crc32: number = 0;
    vendor: string = "";
    desc: string = "";

    // 其他信息
    appidBE: number = 0;
    versionBE: number = 0;
    reserve2: number = 0;
    screenWidth: number = 0;
    screenHeight: number = 0;
    plat: number = 1;
    reserve3: number[] = new Array(31).fill(0);

    // 文件列表
    files: FileItem[] = [];

    constructor(data: Uint8Array) {
        this.data = data;
        this._parseHeader();
        this._parseFileList();
    }

    // 工厂方法
    static fromBytes(data: Uint8Array): MrpInfo {
        return new MrpInfo(data);
    }

    // 插入新文件到 MRP 中
    insert(filename: string, fileData: Uint8Array): void {
        let fileItem:FileItem|undefined = this.files.find(file => file.filename === filename);
        // 检查文件名是否已存在
        if(fileItem){
            fileItem.fileData = fileData;
            fileItem.fileLen = fileData.length;
            console.log("insert 更新文件数据："+filename);
        }
        else{
            const filenameBytes = GBK.encode(filename);
            const filenameBytesWithNull = new Uint8Array(filenameBytes.length + 1);
        filenameBytesWithNull.set(filenameBytes);
        filenameBytesWithNull[filenameBytes.length] = 0;
        const filenameLen = filenameBytesWithNull.length;
            fileItem = new FileItem(filename, filenameLen, 0, fileData.length, fileData);
            this.files.push(fileItem);
        }
        
        this.pack();
        this._parseHeader();
        this._parseFileList();
    }

    pack(): void {
      
        // this.crc32 = 0; // 初始为0，最后计算
        
        // this.reserve2 = 0;
        
        // this.reserve3 = new Array(31).fill(0);
        // 处理文件列表
        let listLen = 0;
        let dataLen = 0;
        const fileItems: Array<{
            filename: string;
            filenameLen: number;
            fileOffset: number;
            fileLen: number;
            data: Uint8Array;
            item: FileItem
        }> = [];
        // 计算文件列表和数据的长度
        for (const file of this.files) {
            let fileData = this.getFileData(file.filename);
            let filename = file.filename;
            
            // 处理文件名中的特殊格式 (name=xxx)
            if (file.filename.includes('(') && file.filename.includes(')')) {
                const tempStr = file.filename;
                const kString = tempStr.substring(tempStr.indexOf('(') + 1, tempStr.indexOf(')'));
                const items = kString.split(",");
                for (const item of items) {
                    const [key, value] = item.split("=").map(s => s.trim());
                    if (key === "name") {
                        filename = value;
                    }
                }
            }
            // 处理 BMP565 转换
            // if (config.useBMP565 && file.path.toLowerCase().endsWith(".bmp")) {
            //     const bmp565Data = this._convertToBMP565(fileData);
            //     if (bmp565Data) {
            //         fileData = bmp565Data;
            //     }
            // }
            // 处理 GZIP 压缩
            // if (config.useGZIP) {
            //     fileData = this._gzipData(fileData);
            // }
            const filenameBytes = GBK.encode(filename);
            const filenameLen = filenameBytes.length + 1; // 包含 null 终止符
            
            // 每个列表项由文件名长度(4) + 文件名 + null(1) + 文件偏移(4) + 文件长度(4) + 保留(4) 组成
            listLen += 4 + filenameLen + 4 + 4 + 4;
            // 每个文件数据由文件名长度(4) + 文件名 + null(1) + 文件长度(4) + 文件数据 组成
            dataLen += 4 + filenameLen + 4 + fileData!.length;
            fileItems.push({
                filename,
                filenameLen,
                fileOffset: 0, // 稍后计算
                fileLen: fileData!.length,
                data: fileData!,
                item: file // 保留原始 FileItem 引用
            });
        }
        // 计算文件位置
        this.mrpHeaderSize = 240; // 固定头大小
        const firstFilePos = this.mrpHeaderSize + listLen;
        this.fileStart = firstFilePos - 8; // 需要减8，与Java代码一致
        this.mrpTotalLen = this.mrpHeaderSize + listLen + dataLen;
        // 创建数据缓冲区
        const buffer = new ArrayBuffer(this.mrpTotalLen);
        const dataView = new DataView(buffer);
        const uint8Array = new Uint8Array(buffer);
        let pos = 0;
        // 写入文件头
        this._writeString(uint8Array, pos, this.magic, 4); pos += 4;
        dataView.setInt32(pos, this.fileStart, true); pos += 4;
        dataView.setInt32(pos, this.mrpTotalLen, true); pos += 4;
        dataView.setInt32(pos, this.mrpHeaderSize, true); pos += 4;
        this._writeGBKString(uint8Array, pos, this.fileName, 12); pos += 12;
        this._writeGBKString(uint8Array, pos, this.displayName, 24); pos += 24;
        this._writeGBKString(uint8Array, pos, this.authStr, 16); pos += 16;
        dataView.setInt32(pos, this.appid, true); pos += 4;
        dataView.setInt32(pos, this.version, true); pos += 4;
        dataView.setInt32(pos, this.flag, true); pos += 4;
        dataView.setInt32(pos, this.builderVersion, true); pos += 4;
        dataView.setInt32(pos, this.crc32, true); pos += 4;
        this._writeGBKString(uint8Array, pos, this.vendor, 40); pos += 40;
        this._writeGBKString(uint8Array, pos, this.desc, 64); pos += 64;
        dataView.setInt32(pos, this.appidBE, false); pos += 4; // 大端
        dataView.setInt32(pos, this.versionBE, false); pos += 4; // 大端
        dataView.setInt32(pos, this.reserve2, true); pos += 4;
        dataView.setInt16(pos, this.screenWidth, true); pos += 2;
        dataView.setInt16(pos, this.screenHeight, true); pos += 2;
        uint8Array[pos++] = this.plat;
        pos += 31; // reserve3
        // 写入文件列表
        let filePos = firstFilePos;
        /*
        for (FileItem item : config.list_file) {
			try {
				// 每个文件数据由：文件名长度、文件名、文件大小组成，数值都是uint32因此需要4*2
				filePos += item.namesize + 1 + 4 * 2;
				item.offset = filePos;
				// 下一个文件数据的开始位置
				filePos += item.len;

				output.write(getIntByte(item.namesize + 1));
				output.write(item.filename.getBytes("GBK"));
				output.writeByte(0);
				output.write(getIntByte(item.offset));
				output.write(getIntByte(item.len));
				output.write(getIntByte(0));
				System.out.println("filename:" + item.filename + "    pos=" + filePos + " len=" + item.len);

			} catch (UnsupportedEncodingException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
        */
        for (const item of fileItems) {
            filePos += item.filenameLen + 4 * 2;
            item.fileOffset = filePos;
            // 下一个文件数据的开始位置
            filePos += item.fileLen;
            // 写入文件名长度 (包含null)
            dataView.setInt32(pos, item.filenameLen, true); pos += 4;
            
            // 写入文件名 (GBK编码)
            const filenameBytes = GBK.encode(item.filename);
            uint8Array.set(filenameBytes, pos); pos += filenameBytes.length;
            uint8Array[pos++] = 0; // null终止符
            
            // 写入文件偏移
            dataView.setInt32(pos, item.fileOffset, true); pos += 4;
            
            // 写入文件长度
            dataView.setInt32(pos, item.fileLen, true); pos += 4;
            
            // 写入保留字段
            dataView.setInt32(pos, 0, true); pos += 4;

            item.item.fileOffset = filePos;
        }
        // 写入文件数据
        for (const item of fileItems) {
            // 写入文件名长度 (包含null)
            dataView.setInt32(pos, item.filenameLen, true); pos += 4;
            
            // 写入文件名 (GBK编码)
            const filenameBytes = GBK.encode(item.filename);
            uint8Array.set(filenameBytes, pos); pos += filenameBytes.length;
            uint8Array[pos++] = 0; // null终止符
            
            // 写入文件长度
            dataView.setInt32(pos, item.fileLen, true); pos += 4;
            
            // 写入文件数据
            uint8Array.set(item.data, pos); pos += item.data.length;
            
        }
        // 计算并更新CRC32
        this.crc32 = this._calculateCRC32(uint8Array);
        dataView.setInt32(84, this.crc32, true);
        // 更新data
        this.data = uint8Array;
    }

    // 辅助方法：写入固定长度的字符串
    private _writeString(array: Uint8Array, offset: number, str: string, length: number): void {
        const encoder = new TextEncoder();
        const encoded = encoder.encode(str);
        const len = Math.min(encoded.length, length);
        array.set(encoded.subarray(0, len), offset);
        // 剩余部分填充0
        if (len < length) {
            array.fill(0, offset + len, offset + length);
        }
    }
    // 辅助方法：写入固定长度的GBK字符串
    private _writeGBKString(array: Uint8Array, offset: number, str: string, length: number): void {
    const encoded = new Uint8Array(GBK.encode(str)); // 强制转换为 Uint8Array
    const len = Math.min(encoded.length, length);
    array.set(encoded.subarray(0, len), offset);
    if (len < length) {
        array.fill(0, offset + len, offset + length);
    }
}
    // 辅助方法：32位整数字节序交换
    private _swapEndian32(value: number): number {
        return (
            ((value & 0xFF) << 24) |
            ((value & 0xFF00) << 8) |
            ((value >> 8) & 0xFF00) |
            ((value >> 24) & 0xFF)
        );
    }
    // 辅助方法：计算CRC32
    private _calculateCRC32(data: Uint8Array): number {
        let crc = 0xFFFFFFFF;
        for (let i = 0; i < data.length; i++) {
            if (i >= 84 && i < 88) continue; // 跳过CRC32字段本身
            
            crc ^= data[i];
            for (let j = 0; j < 8; j++) {
                crc = (crc >>> 1) ^ (crc & 1 ? 0xEDB88320 : 0);
            }
        }
        return crc ^ 0xFFFFFFFF;
    }
    // 辅助方法：GZIP压缩数据
    private _gzipData(data: Uint8Array): Uint8Array {
        if (this._isGzip(data)) {
            return data;
        }
        return pako.gzip(data);
    }
    // 辅助方法：转换BMP到BMP565格式
    private _convertToBMP565(bmpData: Uint8Array): Uint8Array | null {
        // 检查BMP文件头
        if (bmpData.length < 54 || bmpData[0] !== 0x42 || bmpData[1] !== 0x4D) {
            return null;
        }
        const view = new DataView(bmpData.buffer);
        const width = view.getInt32(18, true);
        const height = view.getInt32(22, true);
        const bitsPerPixel = view.getInt16(28, true);
        const dataOffset = view.getInt32(10, true);
        // 已经是16位色，直接返回
        if (bitsPerPixel === 16) {
            const bitmap = new Uint8Array(width * height * 2);
            for (let y = 0; y < height; y++) {
                const srcPos = dataOffset + (height - 1 - y) * width * 2;
                const destPos = y * width * 2;
                bitmap.set(bmpData.subarray(srcPos, srcPos + width * 2), destPos);
            }
            return bitmap;
        }
        // 24位色转16位色
        if (bitsPerPixel === 24) {
            const rowSize = Math.floor((width * 3 + 3) / 4) * 4; // 行对齐
            const bitmap = new Uint8Array(width * height * 2);
            
            for (let y = 0; y < height; y++) {
                for (let x = 0; x < width; x++) {
                    const srcPos = dataOffset + (height - 1 - y) * rowSize + x * 3;
                    const r = bmpData[srcPos + 2] >> 3;
                    const g = bmpData[srcPos + 1] >> 2;
                    const b = bmpData[srcPos] >> 3;
                    const color = (r << 11) | (g << 5) | b;
                    
                    const destPos = (y * width + x) * 2;
                    bitmap[destPos] = color & 0xFF;
                    bitmap[destPos + 1] = (color >> 8) & 0xFF;
                }
            }
            return bitmap;
        }
        return null;
    }

    // 从 MRP 中删除文件
    remove(filename: string): void {
        const fileIndex = this.files.findIndex(file => file.filename === filename);
        if (fileIndex === -1) {
            throw new Error(`文件不存在: ${filename}`);
        }
        // 更新文件列表
        this.files.splice(fileIndex, 1);
        this.pack();
        this._parseHeader();
        this._parseFileList();
    }

    // 替换指定文件数据
    replace(filename: string, newData: Uint8Array): void {
        this.remove(filename);
        this.insert(filename, newData);
    }

    // 解析 MRP 文件头
    private _parseHeader(): void {
        this.fileStart = this._readInt(4);
        this.mrpTotalLen = this._readInt(8);
        this.mrpHeaderSize = this._readInt(12);
        this.fileName = this._readGBKString(16)!;
        this.displayName = this._readGBKString(28)!;
        this.authStr = this._readGBKString(52)!;
        this.appid = this._readInt(68);
        this.version = this._readInt(72);
        this.flag = this._readInt(76);
        this.builderVersion = this._readInt(80);
        this.crc32 = this._readInt(84);
        this.vendor = this._readGBKString(88)!;
        this.desc = this._readGBKString(128)!;
        this.appidBE = this._readBigInt(192);
        this.versionBE = this._readBigInt(196);
        this.reserve2 = this._readInt(200);
        this.screenWidth = this._readInt(204);
        this.screenHeight = this._readInt(206);
        this.plat = this.data[208];
    }

    // 写入 MRP 文件头
    writeHeader(): void {
        this._setInt(4, this.fileStart);
        this._setInt(8, this.mrpTotalLen);
        this._setInt(12, this.mrpHeaderSize);
        this._setGBKString(16, this.fileName, 12);
        this._setGBKString(28, this.displayName, 24);
        this._setGBKString(52, this.authStr, 16);
        this._setInt(68, this.appid);
        this._setInt(72, this.version);
        this._setInt(76, this.flag);
        this._setInt(80, this.builderVersion);
        this._setInt(84, this.crc32);
        this._setGBKString(88, this.vendor, 40);
        this._setGBKString(128, this.desc, 64);
        this._setBigInt(192, this.appidBE);
        this._setBigInt(196, this.versionBE);
        this._setInt(200, this.reserve2);
        this._setInt(204, this.screenWidth);
        this._setInt(206, this.screenHeight);
        this.data[208] = this.plat;
    }

    // 解析文件列表
    private _parseFileList(): void {
        this.files = [];
        const listStart = this.mrpHeaderSize;
        const listEnd = this.fileStart + 8;
        let offset = listStart;

        while (offset < listEnd) {
            const fileNameLen = this._readInt(offset);
            offset += 4;

            const filename = this._readGBKString(offset);
            offset += fileNameLen;
            console.log(`read .... ${filename} ${offset}`);
            const fileOffset = this._readInt(offset);
            offset += 4;

            const fileLen = this._readInt(offset);
            offset += 4;

            offset += 4; // 跳过保留字段

            if (filename) {
                this.files.push(new FileItem(filename, fileNameLen, fileOffset, fileLen, null));
            }
            console.log(`offset=${offset} fileStart=${this.fileStart}`);
        }
    }

    // 解包 MRP 文件到内存中的文件映射
    unpackToMemory(): Map<string, Uint8Array> {
        const result = new Map<string, Uint8Array>();

        for (const file of this.files) {
            const fileData = this.getFileData(file.filename);
            if (!fileData) continue;

            if (this._isGzip(fileData)) {
                try {
                    const decompressed = pako.ungzip(fileData);
                    result.set(file.filename, decompressed);
                } catch (e) {
                    console.log(`解压 GZIP 失败: ${file.filename} - ${e}`);
                }
            } else {
                result.set(file.filename, fileData);
            }
        }

        return result;
    }

    // 获取指定文件的数据
    getFileData(filename: string): Uint8Array | null {
        const file = this.files.find(f => f.filename === filename);
        if (!file) {
            throw new Error(`文件不存在: ${filename}`);
        }
        if(file.fileData){
            console.log("getFileData 命中缓存 "+filename);
            return file.fileData;
        }

        try {
            return this.data.subarray(file.fileOffset, file.fileOffset + file.fileLen);
        } catch (e) {
            console.log(`读取文件数据失败: ${filename} - ${e}`);
            return null;
        }
    }

    // 辅助方法：读取小端序整数
    private _readInt(offset: number): number {
        return this.data[offset] |
            (this.data[offset + 1] << 8) |
            (this.data[offset + 2] << 16) |
            (this.data[offset + 3] << 24);
    }

    private _setInt(offset: number, value: number): void {
        this.data[offset] = value & 0xFF;
        this.data[offset + 1] = (value >> 8) & 0xFF;
        this.data[offset + 2] = (value >> 16) & 0xFF;
        this.data[offset + 3] = (value >> 24) & 0xFF;
    }

    // 辅助方法：读取大端序整数
    private _readBigInt(offset: number): number {
        return this.data[offset + 3] |
            (this.data[offset + 2] << 8) |
            (this.data[offset + 1] << 16) |
            (this.data[offset] << 24);
    }

    private _setBigInt(offset: number, value: number): void {
        this.data[offset] = (value >> 24) & 0xFF;
        this.data[offset + 1] = (value >> 16) & 0xFF;
        this.data[offset + 2] = (value >> 8) & 0xFF;
        this.data[offset + 3] = value & 0xFF;
    }

    // 辅助方法：读取 GBK 字符串
    private _readGBKString(offset: number): string | null {
        let len = 0;
        for (let i = offset; i < this.data.length; i++) {
            if (this.data[i] !== 0) {
                len++;
            } else {
                break;
            }
        }

        try {
            return GBK.decode(this.data.subarray(offset, offset + len));
        } catch (e) {
            console.log(`解码字符串失败: ${e} ${this.data.subarray(offset, offset + len)}`);
            return null;
        }
    }

    // 辅助方法：设置 GBK 字符串
    private _setGBKString(offset: number, value: string, maxLen: number): void {
        const bytes = GBK.encode(value);
        if (bytes.length > maxLen) {
            throw new Error(`GBK 字符串长度超过限制: ${value}`);
        }

        this.data.set(bytes, offset);
        // 填充剩余部分为 0
        for (let i = offset + bytes.length; i < offset + maxLen; i++) {
            this.data[i] = 0;
        }
    }

    // 辅助方法：检查是否为 GZIP 文件
    public _isGzip(data: Uint8Array): boolean {
        return data.length >= 3 &&
            data[0] === 0x1f &&
            data[1] === 0x8b &&
            data[2] === 0x08;
    }

    toString(): string {
        return `
MRP 文件信息:
  文件名: ${this.fileName}
  显示名: ${this.displayName}
  供应商: ${this.vendor}
  描述: ${this.desc}
  AppID: ${this.appid} (BE: ${this.appidBE})
  版本: ${this.version} (BE: ${this.versionBE})
  构建版本: ${this.builderVersion}
  屏幕尺寸: ${this.screenWidth}x${this.screenHeight}
  平台: ${this.plat}
  文件数: ${this.files.length}
  数据大小: ${this.data.length} 字节
`;
    }
}

// 使用示例
async function mrpinfoMain() {
    try {
        // 从文件读取数据 (在浏览器中需要使用File API)
        // 随机生成数据
        const fileData = new Uint8Array(1024); // 假设这是从文件读取的 MRP 数据
        for (let i = 0; i < fileData.length; i++) {
            fileData[i] = Math.floor(Math.random() * 256);
        }
        
        // 从字节数据创建 MRP 信息
        const mrpInfo = MrpInfo.fromBytes(fileData);

        // 打印 MRP 信息
        console.log(mrpInfo.toString());

        // 打印文件列表
        for (const file of mrpInfo.files) {
            console.log(file.toString());
        }

        // 插入新文件
        const newFileData = GBK.encode('Hello, MRP!');
        mrpInfo.insert('new_file.txt', newFileData);
        mrpInfo.remove('new_file.txt');
        console.log(`插入文件后数据大小: ${mrpInfo.data.length} 字节`);

        // 解包到内存
        const memoryFiles = mrpInfo.unpackToMemory();
        console.log(`解包到内存中的文件数: ${memoryFiles.size}`);
        
        // 打印文件列表
        for (const entry of mrpInfo.files) {
            console.log(entry.toString());
        }
    } catch (e) {
        console.log(`处理 MRP 文件出错: ${e}`);
        console.error(e);
    }
}

// 执行示例
// mrpinfoMain();
