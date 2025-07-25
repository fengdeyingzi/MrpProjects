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
        public fileLen: number
    ) {}

    toString(): string {
        return `文件: ${this.filename} (偏移: ${this.fileOffset}, 长度: ${this.fileLen})`;
    }
}

// MRP 文件信息类
class MrpInfo {
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
        // 检查文件名是否已存在
        if (this.files.some(file => file.filename === filename)) {
            throw new Error(`文件已存在: ${filename}`);
        }

        // 创建新的文件项
        const newFileLen = fileData.length;
        const filenameBytes = GBK.encode(filename);
        const filenameBytesWithNull = new Uint8Array(filenameBytes.length + 1);
        filenameBytesWithNull.set(filenameBytes);
        filenameBytesWithNull[filenameBytes.length] = 0;
        
        const filenameLen = filenameBytesWithNull.length;
        const newFileOffset = this.data.length + 4 + filenameLen + 4 + 4 + 4;
        
        console.log(`newfileoffset=${newFileOffset}, datalen=${this.data.length}`);
        const newFileItem = new FileItem(filename, filenameLen, 0, fileData.length);

        console.log(`.............. 插入文件名长度 ${filenameLen}`);

        // 插入文件列表数据
        const newEntry = new Uint8Array(4 + filenameLen + 4 + 4 + 4);
        const view = new DataView(newEntry.buffer);
        view.setInt32(0, filenameLen, true);
        newEntry.set(filenameBytesWithNull, 4);
        view.setInt32(4 + filenameLen, newFileOffset, true);
        view.setInt32(4 + filenameLen + 4, newFileLen, true);
        // 保留字段已经是0

        this.data = ListUtil.insertIntoUint8Array(
            this.data,
            this.fileStart + 8,
            newEntry
        );

        console.log("11");
        // 更新文件起始位置
        this.fileStart += 4 + filenameLen + 4 + 4 + 4;

        // 写入文件数据
        if (newFileOffset !== this.data.length) {
            console.log(`文件位置不相等：${newFileOffset} ${this.data.length}`);
        }
        console.log("22");
        this.data = ListUtil.insertIntoUint8Array(this.data, newFileOffset, fileData);

        // 更新总长度
        this.mrpTotalLen += 4 + filenameLen + 4 + 4 + 4;
        this.mrpTotalLen += newFileItem.filenameLen + 1 + 4 * 2 + newFileItem.fileLen;
        console.log("33");
        // 写入data
        this._setInt(4, this.fileStart);
        this._setInt(8, this.mrpTotalLen);

        // 更新文件列表
        newFileItem.fileOffset = newFileOffset;
        this.files.push(newFileItem);
    }

    // 从 MRP 中删除文件
    remove(filename: string): void {
        const fileIndex = this.files.findIndex(file => file.filename === filename);
        if (fileIndex === -1) {
            throw new Error(`文件不存在: ${filename}`);
        }

        const file = this.files[fileIndex];

        // 删除文件数据
        this.data = ListUtil.removeRange(this.data, file.fileOffset, file.fileLen);

        // 更新文件列表区域
        const listStart = this.mrpHeaderSize;
        const listEnd = this.fileStart + 8;
        let offset = listStart;

        while (offset < listEnd) {
            const tempOffset = offset;
            const fileNameLen = this._readInt(offset);
            offset += 4;
            const currentFilename = this._readGBKString(offset);
            offset += fileNameLen;
            const fileOffset = this._readInt(offset);
            offset += 4;
            const fileLen = this._readInt(offset);
            offset += 4;
            offset += 4; // 跳过保留字段
            
            if (currentFilename === filename) {
                // 找到要删除的文件，更新数据
                this.data = ListUtil.deleteFromUint8Array(
                    this.data,
                    tempOffset,
                    fileNameLen + 4 + 4 + 4 + 4
                );
                break;
            }
        }

        // 更新文件起始位置
        this.fileStart -= (file.filenameLen + 4 + 4 + 4);

        // 更新总长度
        this.mrpTotalLen -= (file.filenameLen + 4 + 4 + 4 + file.fileLen);

        // 写入更新后的数据
        this._setInt(4, this.fileStart);
        this._setInt(8, this.mrpTotalLen);

        // 更新文件列表
        this.files.splice(fileIndex, 1);
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
                this.files.push(new FileItem(filename, fileNameLen, fileOffset, fileLen));
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
    private _isGzip(data: Uint8Array): boolean {
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
