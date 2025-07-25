// mrp-editor.ts
import * as pako from 'pako';
import * as GBK from 'gbk.js';


function padStart(str: string, targetLength: number, padString: string = ' '): string {
    targetLength = Math.floor(targetLength); // 确保是整数
    if (str.length >= targetLength) {
        return str;
    }
    
    padString = padString || ' ';
    const padLength = targetLength - str.length;
    let padding = '';
    
    while (padding.length < padLength) {
        padding += padString;
    }
    
    return padding.slice(0, padLength) + str;
}


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

// UI 控制器类
class MrpEditorUI {
    private mrpInfo: MrpInfo | null = null;
    private currentFile: File | null = null;

    constructor() {
        this.initEventListeners();
        this.setupFileInputListener();
    }

    private showToast(message: string, duration: number = 3000): void {
        const toastContainer = document.getElementById('toastContainer');
        if (!toastContainer) return;
        
        const toast = document.createElement('div');
        toast.className = 'toast';
        toast.textContent = message;
        toastContainer.appendChild(toast);
        
        // 触发重绘
        void toast.offsetWidth;
        
        // 显示 toast
        toast.classList.add('show');
        
        // 自动消失
        setTimeout(() => {
            toast.classList.remove('show');
            setTimeout(() => {
                toast.remove();
            }, 300);
        }, duration);
    }

    private setupFileInputListener(): void {
        const fileInput = document.getElementById('newFileContent') as HTMLInputElement;
        const fileNameInput = document.getElementById('newFileName') as HTMLInputElement;
        fileInput.addEventListener('change', () => {
            if (fileInput.files && fileInput.files.length > 0) {
                const file = fileInput.files[0];
                // 只有当文件名输入框为空时才自动填充
                if (!fileNameInput.value.trim()) {
                    fileNameInput.value = file.name;
                }
            }
        });
    }

    private initEventListeners(): void {
        // 移动端菜单按钮
        document.getElementById('mobileMenuBtn')?.addEventListener('click', () => {
            const menuItems = document.getElementById('menuItems');
            if (menuItems) menuItems.classList.toggle('show');
        });
// 为所有菜单项添加点击后关闭菜单的逻辑
    const menuItems = document.querySelectorAll('.menu-item');
    menuItems.forEach(item => {
        item.addEventListener('click', () => {
            const menu = document.getElementById('menuItems');
            if (menu && window.innerWidth <= 768) { // 只在移动端关闭
                menu.classList.remove('show');
            }
        });
    });
        // 切换基本信息面板折叠状态
        const fileInfoHeader = document.getElementById('fileInfoHeader');
        const toggleInfoBtn = document.getElementById('toggleInfoBtn');
        const fileInfoPanel = document.getElementById('fileInfoPanel');

        if (fileInfoHeader && toggleInfoBtn && fileInfoPanel) {
            const togglePanel = () => {
                fileInfoPanel.classList.toggle('collapsed');
                
                if (fileInfoPanel.classList.contains('collapsed')) {
                    toggleInfoBtn.innerHTML = '<i class="fas fa-chevron-down"></i> 展开';
                } else {
                    toggleInfoBtn.innerHTML = '<i class="fas fa-chevron-up"></i> 收起';
                }
            };

            fileInfoHeader.addEventListener('click', togglePanel);
            toggleInfoBtn.addEventListener('click', (e) => {
                e.stopPropagation();
                togglePanel();
            });
        }

        // 编辑文件信息按钮
        document.getElementById('editInfoBtn')?.addEventListener('click', (e) => {
            e.stopPropagation();
            if(!this.mrpInfo){
                this.showToast('请先打开一个MRP文件！');
                return;
            }
            this.showEditModal();
        });

        // 关闭编辑模态框
        document.getElementById('closeEditModal')?.addEventListener('click', () => this.hideEditModal());
        document.getElementById('cancelEditBtn')?.addEventListener('click', () => this.hideEditModal());

        // 保存编辑信息
        document.getElementById('editInfoForm')?.addEventListener('submit', (e) => {
            e.preventDefault();
            this.saveFileInfo();
        });

        // 添加文件按钮
        document.getElementById('addFileBtn')?.addEventListener('click', () => {
            if(!this.mrpInfo){
                this.showToast('请先打开一个MRP文件！');
                return;
            }
            this.showAddFileModal();
        });

        // 关闭添加文件模态框
        document.getElementById('closeAddModal')?.addEventListener('click', () => this.hideAddFileModal());
        document.getElementById('cancelAddBtn')?.addEventListener('click', () => this.hideAddFileModal());

        // 添加文件表单提交
        document.getElementById('addFileForm')?.addEventListener('submit', (e) => {
            e.preventDefault();
            this.addFile();
        });

        // 删除文件按钮事件委托
        document.getElementById('fileListBody')?.addEventListener('click', (e) => {
            const target = e.target as HTMLElement;
            if (target.classList.contains('delete-btn') || target.closest('.delete-btn')) {
                e.stopPropagation();
                this.deleteFile(target);
            }
        });

        // 关于按钮
        document.getElementById('aboutBtn')?.addEventListener('click', () => {
            this.showAboutModal();
        });

        // 关闭关于模态框
        document.getElementById('closeAboutModal')?.addEventListener('click', () => this.hideAboutModal());
        document.getElementById('closeAboutBtn')?.addEventListener('click', () => this.hideAboutModal());

        // 打开文件按钮
        document.getElementById('openFileBtn')?.addEventListener('click', () => {
            this.openFile();
        });

        // 保存文件按钮
        document.getElementById('saveFileBtn')?.addEventListener('click', () => {
            if(!this.mrpInfo){
                this.showToast('请先打开一个MRP文件！');
                return;
            }
            this.saveFile();
        });

        // 插入文件项按钮
        document.getElementById('insertFileBtn')?.addEventListener('click', () => {
            this.showAddFileModal();
        });

        // 帮助按钮
        document.getElementById('helpBtn')?.addEventListener('click', () => {
            alert('帮助文档将在实际应用中提供');
        });
    }

    private showEditModal(): void {
        if (!this.mrpInfo) return;

        const modal = document.getElementById('editInfoModal');
        if (modal) modal.style.display = 'flex';

        // 填充表单数据
        (document.getElementById('editDisplayName') as HTMLInputElement).value = this.mrpInfo.displayName;
        (document.getElementById('editFileName') as HTMLInputElement).value = this.mrpInfo.fileName;
        (document.getElementById('editAuthStr') as HTMLInputElement).value = this.mrpInfo.authStr;
        (document.getElementById('editAppid') as HTMLInputElement).value = this.mrpInfo.appid.toString();
        (document.getElementById('editVersion') as HTMLInputElement).value = this.mrpInfo.version.toString();
        (document.getElementById('editBuilderVersion') as HTMLInputElement).value = this.mrpInfo.builderVersion.toString();
        (document.getElementById('editVendor') as HTMLInputElement).value = this.mrpInfo.vendor;
        (document.getElementById('editDesc') as HTMLTextAreaElement).value = this.mrpInfo.desc;
        (document.getElementById('editScreenWidth') as HTMLInputElement).value = this.mrpInfo.screenWidth.toString();
        (document.getElementById('editScreenHeight') as HTMLInputElement).value = this.mrpInfo.screenHeight.toString();
        (document.getElementById('editPlat') as HTMLSelectElement).value = this.mrpInfo.plat.toString();
    }

    private hideEditModal(): void {
        const modal = document.getElementById('editInfoModal');
        if (modal) modal.style.display = 'none';
    }

    private saveFileInfo(): void {
        if (!this.mrpInfo) return;

        // 更新 MRP 信息
        this.mrpInfo.displayName = (document.getElementById('editDisplayName') as HTMLInputElement).value;
        this.mrpInfo.fileName = (document.getElementById('editFileName') as HTMLInputElement).value;
        this.mrpInfo.authStr = (document.getElementById('editAuthStr') as HTMLInputElement).value;
        this.mrpInfo.appid = parseInt((document.getElementById('editAppid') as HTMLInputElement).value);
        this.mrpInfo.version = parseInt((document.getElementById('editVersion') as HTMLInputElement).value);
        this.mrpInfo.builderVersion = parseInt((document.getElementById('editBuilderVersion') as HTMLInputElement).value);
        this.mrpInfo.vendor = (document.getElementById('editVendor') as HTMLInputElement).value;
        this.mrpInfo.desc = (document.getElementById('editDesc') as HTMLTextAreaElement).value;
        this.mrpInfo.screenWidth = parseInt((document.getElementById('editScreenWidth') as HTMLInputElement).value);
        this.mrpInfo.screenHeight = parseInt((document.getElementById('editScreenHeight') as HTMLInputElement).value);
        this.mrpInfo.plat = parseInt((document.getElementById('editPlat') as HTMLSelectElement).value);
        console.log("文件信息", this.mrpInfo);
        // 更新头部信息
        this.mrpInfo.writeHeader();

        // 更新 UI 显示
        this.updateFileInfoUI();

        this.hideEditModal();
        this.showToast('文件信息已更新！');
    }

    private showAddFileModal(): void {
        const modal = document.getElementById('addFileModal');
        if (modal) modal.style.display = 'flex';
    }

    private hideAddFileModal(): void {
        const modal = document.getElementById('addFileModal');
        if (modal) modal.style.display = 'none';
    }

    private addFile(): void {
        if (!this.mrpInfo) {
        this.showToast('请先打开一个MRP文件！');
        this.hideAddFileModal();
        return;
    }

        const fileNameInput = document.getElementById('newFileName') as HTMLInputElement;
        const fileInput = document.getElementById('newFileContent') as HTMLInputElement;

        // 自动填充文件名逻辑
    if (fileInput.files && fileInput.files.length > 0 && !fileNameInput.value.trim()) {
        const file = fileInput.files[0];
        fileNameInput.value = file.name;
        
    }
        const fileName = fileNameInput.value.trim();
        
        if (!fileName) {
            alert('请输入文件名！');
            return;
        }

        if (!fileInput.files || fileInput.files.length === 0) {
            alert('请选择要添加的文件！');
            return;
        }

        const file = fileInput.files[0];
        const reader = new FileReader();

        reader.onload = (e) => {
            try {
                const fileData = new Uint8Array(e.target?.result as ArrayBuffer);
                this.mrpInfo?.insert(fileName, fileData);
                
                // 更新 UI
                this.updateFileListUI();
                this.updateBasicInfoUI();
                
                this.hideAddFileModal();
                fileNameInput.value = '';
                fileInput.value = '';
                
                this.showToast('文件已添加到MRP中！');
            } catch (error) {
                alert(`添加文件失败: ${error}`);
            }
        };

        reader.readAsArrayBuffer(file);
    }

    private deleteFile(target: HTMLElement): void {
        if (!this.mrpInfo) return;

        const row = target.closest('tr');
        if (!row) return;

        const fileName = row.querySelector('td:nth-child(2)')?.textContent?.trim();
        if (!fileName) return;

        if (confirm(`确定要删除文件 "${fileName}" 吗？`)) {
            try {
                this.mrpInfo.remove(fileName);
                
                // 更新 UI
                this.updateFileListUI();
                this.updateBasicInfoUI();
                
                this.showToast('文件已删除！');
            } catch (error) {
                alert(`删除文件失败: ${error}`);
            }
        }
    }

    private showAboutModal(): void {
        const modal = document.getElementById('aboutModal');
        if (modal) modal.style.display = 'flex';
    }

    private hideAboutModal(): void {
        const modal = document.getElementById('aboutModal');
        if (modal) modal.style.display = 'none';
    }

    private openFile(): void {
        const input = document.createElement('input');
        input.type = 'file';
        input.accept = '.mrp';

        input.onchange = (e) => {
            const file = (e.target as HTMLInputElement).files?.[0];
            if (!file) return;

            this.currentFile = file;
            const reader = new FileReader();

            reader.onload = (e) => {
                try {
                    const fileData = new Uint8Array(e.target?.result as ArrayBuffer);
                    this.mrpInfo = MrpInfo.fromBytes(fileData);
                    
                    // 更新 UI
                    this.updateFileInfoUI();
                    this.updateFileListUI();
                    this.updateBasicInfoUI();
                    
                    this.showToast('文件加载成功！');
                } catch (error) {
                    alert(`文件解析失败: ${error}`);
                }
            };

            reader.readAsArrayBuffer(file);
        };

        input.click();
    }

    private saveFile(): void {
        if (!this.mrpInfo || !this.currentFile) {
            alert('没有可保存的文件！');
            return;
        }

        const blob = new Blob([this.mrpInfo.data], { type: 'application/octet-stream' });
        const url = URL.createObjectURL(blob);
        
        const a = document.createElement('a');
        a.href = url;
        a.download = this.currentFile.name || 'modified.mrp';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
        
        alert('文件保存成功！');
    }

    private updateFileInfoUI(): void {
        if (!this.mrpInfo) return;

        // 基本信息
        document.getElementById('basicDisplayName')!.textContent = this.mrpInfo.displayName;
        document.getElementById('basicVersion')!.textContent = this.mrpInfo.version.toString();
        document.getElementById('basicAuthStr')!.textContent = this.mrpInfo.vendor;

        // 详细信息
        document.getElementById('fileName')!.textContent = this.mrpInfo.fileName;
        document.getElementById('appid')!.textContent = this.mrpInfo.appid.toString();
        document.getElementById('builderVersion')!.textContent = this.mrpInfo.builderVersion.toString();
        document.getElementById('vendor')!.textContent = this.mrpInfo.vendor;
        document.getElementById('desc')!.textContent = this.mrpInfo.desc;
        document.getElementById('screenSize')!.textContent = `${this.mrpInfo.screenWidth}x${this.mrpInfo.screenHeight}`;
        
        let platText = '未知';
        switch (this.mrpInfo.plat) {
            case 1: platText = 'MTK'; break;
            case 2: platText = '展讯'; break;
            case 3: platText = '其它'; break;
        }
        document.getElementById('plat')!.textContent = platText;
        
        const fileSizeKB = (this.mrpInfo.data.length / 1024).toFixed(1);
        document.getElementById('fileSize')!.textContent = `${fileSizeKB} KB`;
    }

    private updateBasicInfoUI(): void {
        if (!this.mrpInfo) return;

        document.getElementById('basicFileCount')!.textContent = this.mrpInfo.files.length.toString();
    }

    private updateFileListUI(): void {
        if (!this.mrpInfo) return;

        const fileListBody = document.getElementById('fileListBody');
        if (!fileListBody) return;

        // 清空现有内容
        fileListBody.innerHTML = '';

        // 添加新内容
        this.mrpInfo.files.forEach((file, index) => {
            const row = document.createElement('tr');
            
            // 获取文件图标类
            let fileIconClass = 'fa-file';
            const ext = file.filename.split('.').pop()?.toLowerCase();
            switch (ext) {
                case 'png':
                case 'jpg':
                case 'jpeg':
                case 'gif':
                case 'bmp':
                    fileIconClass = 'fa-file-image';
                    break;
                case 'js':
                case 'json':
                case 'xml':
                case 'html':
                case 'css':
                    fileIconClass = 'fa-file-code';
                    break;
                case 'mp3':
                case 'wav':
                case 'ogg':
                    fileIconClass = 'fa-file-audio';
                    break;
                case 'zip':
                case 'rar':
                case '7z':
                    fileIconClass = 'fa-file-archive';
                    break;
                case 'txt':
                case 'ini':
                case 'conf':
                    fileIconClass = 'fa-file-alt';
                    break;
            }

            const fileSizeKB = (file.fileLen / 1024).toFixed(1);
            // const offsetHex = `0x${file.fileOffset.toString(16).toUpperCase().(8, '0')}`;
            const offsetHex = `0x${padStart(file.fileOffset.toString(16).toUpperCase(), 8, '0')}`;

            row.innerHTML = `
                <td>${index + 1}</td>
                <td><i class="fas ${fileIconClass} file-icon"></i> ${file.filename}</td>
                <td>${offsetHex}</td>
                <td>${fileSizeKB} KB</td>
                <td>
                    <div class="file-actions">
                        <button class="action-btn" title="查看"><i class="fas fa-eye"></i></button>
                        <button class="action-btn" title="导出"><i class="fas fa-download"></i></button>
                        <button class="action-btn delete-btn" title="删除"><i class="fas fa-trash"></i></button>
                    </div>
                </td>
            `;

            fileListBody.appendChild(row);
        });
    }
}

// 初始化应用
document.addEventListener('DOMContentLoaded', () => {
    new MrpEditorUI();
});
