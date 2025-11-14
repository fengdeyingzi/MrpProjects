// mrp-editor.ts
import * as pako from 'pako';
import * as GBK from 'gbk.js';
import { encodeBase64, decodeBase64 } from "./base64";
import { MrpInfo } from "./mrpinfo";
import { Player } from './player';



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
    ) { }

    toString(): string {
        return `文件: ${this.filename} (偏移: ${this.fileOffset}, 长度: ${this.fileLen})`;
    }
}



// UI 控制器类
class MrpEditorUI {
    private mrpInfo: MrpInfo | null = null;
    private currentFile: File | null = null;
    private player: Player | null = null;

    constructor() {
        this.initEventListeners();
        this.setupFileInputListener();
    }

    private showToast(message: string, duration: number = 3000): void {
        console.log("toast:" + message);
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
            if (!this.mrpInfo) {
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
            if (!this.mrpInfo) {
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
            const row = target.closest('tr');
            if (!row || !this.mrpInfo) return;

            // 获取文件名
            const filename = row.querySelector('td:nth-child(2)')?.textContent?.trim();
            if (!filename) return;

            // 处理删除按钮
            if (target.classList.contains('delete-btn') || target.closest('.delete-btn')) {
                e.stopPropagation();
                this.deleteFile(target);
                return;
            }

            // 处理导出按钮
            if (target.classList.contains('export-btn') || target.closest('.export-btn')) {
                e.stopPropagation();
                try {
                    const fileData = this.mrpInfo.getFileData(filename);
                    if (fileData) {
                        this.exportFile(filename, fileData);
                    } else {
                        this.showToast('获取文件数据失败');
                    }
                } catch (error) {
                    this.showToast(`导出文件失败: ${error}`);
                }
                return;
            }

            // 处理查看按钮
            if (target.classList.contains('view-btn') || target.closest('.view-btn')) {
                e.stopPropagation();
                this.viewFile(filename);
                return;
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
            if (!this.mrpInfo) {
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
        let that = this;


        window.addEventListener('pageshow', function (event) {
            console.log("用户点击返回。。。。");
            // 检查是否是从缓存中恢复的页面（即返回操作）

            console.log('用户通过返回按钮回到此页');
            let mrpFileName = localStorage.getItem("mrpfilename");
            let mrpFile = localStorage.getItem("mrpfile");
            if (mrpFileName && mrpFile) {
                let mrpFileData = decodeBase64(mrpFile!);
                that.currentFile = new File([mrpFileData], mrpFileName, { type: 'application/octet-stream' });
                try {
                    this.localStorage.removeItem('mrpfile');
                    this.localStorage.removeItem('mrpfilename');
                    that.mrpInfo = MrpInfo.fromBytes(mrpFileData);

                    // 更新 UI
                    that.updateFileInfoUI();
                    that.updateFileListUI();
                    that.updateBasicInfoUI();

                    that.showToast('文件加载成功！');
                } catch (error) {
                    alert(`文件解析失败: ${error}`);
                }

            }
            let rcfileData = localStorage.getItem('rcfile');
            let rcfilename = localStorage.getItem('rcfilename');
            if (rcfilename && rcfileData) {



                try {



                    this.localStorage.removeItem('rcfile');
                    this.localStorage.removeItem('rcfilename');

                    const fileData = decodeBase64(rcfileData!);
                    if (that.mrpInfo) {
                        that.mrpInfo.insert(rcfilename, fileData);

                        // 更新 UI
                        that.updateFileInfoUI();
                        that.updateFileListUI();
                        that.updateBasicInfoUI();

                        //that.showToast('文件加载成功！');
                        that.showToast("" + rcfilename + "已更新");
                    }

                } catch (error) {
                    alert(`文件解析失败: ${error}`);
                }





            }




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

    private viewFile(filename: string): void {
        if (!this.mrpInfo) return;

        try {
            let fileData = this.mrpInfo.getFileData(filename);
            if (!fileData) {
                this.showToast('无法获取文件内容');
                return;
            }
            if (this.mrpInfo._isGzip(fileData)) {
                fileData = pako.ungzip(fileData);
            }

            // 检查是否是文本文件
            const textExtensions = ['txt', 'ini', 'conf', 'js', 'json', 'xml', 'html', 'css'];
            const musicExtensions = ['mp3', 'wav', 'ogg', 'flac', 'mid'];
            const ext = filename.split('.').pop()?.toLowerCase();

            if (ext && textExtensions.includes(ext)) {
                // 尝试解码为文本
                try {
                    let content: string;
                    if (this.mrpInfo._isGzip(fileData)) {
                        const decompressed = pako.ungzip(fileData);
                        content = GBK.decode(decompressed);
                    } else {
                        content = GBK.decode(fileData);
                    }

                    // 显示文件内容（可以扩展为模态框显示）
                    console.log(`文件 ${filename} 内容:`, content);
                    alert(`文件 ${filename} 内容:\n\n${content}`);
                    this.showToast(`已查看文件 ${filename} 内容（查看控制台）`);
                } catch (e) {
                    this.showToast('无法解码文件内容');
                }
            }
            else if (ext && musicExtensions.includes(ext)) {
                // 尝试播放音频文件
                // const audio = new Audio(URL.createObjectURL(new Blob([fileData])));
                // audio.controls = true;
                // audio.play();
                // document.body.appendChild(audio);
                if(!this.player){
                    this.player = new Player();
                }
                this.player.setData(fileData, filename);
                this.player.play();
                this.showToast(`已播放音频文件 ${filename}`);
            }
            else if (ext == 'rc') {
                const base64Data = encodeBase64(fileData!);
                localStorage.setItem('rcfile', base64Data);
                localStorage.setItem('rcfilename', filename);
                localStorage.setItem("mrpfile", encodeBase64(this.mrpInfo.data));
                localStorage.setItem("mrpfilename", this.currentFile!.name);
                window.location.href = "../RcEditor/"
            }
            else if(ext == 'bmp' || ext == 'bma'){
                const base64Data = encodeBase64(fileData!);
                localStorage.setItem('bmpfilename', filename);
                localStorage.setItem('bmpfiledata', base64Data);
                localStorage.setItem("mrpfile", encodeBase64(this.mrpInfo.data));
                localStorage.setItem("mrpfilename", this.currentFile!.name);
                window.location.href = "../BmaViewer/"
            }
            else if (ext === 'bin') {
                // 二进制文件，直接显示十六进制内容
                const hexContent = Array.from(fileData).map(byte => byte.toString(16).padStart(2, '0')).join(' ');
                console.log(`文件 ${filename} 十六进制内容:`, hexContent);
                alert(`文件 ${filename} 十六进制内容:\n\n${hexContent}`);
                this.showToast(`已查看文件 ${filename} 十六进制内容（查看控制台）`);
            }
            else {
                this.showToast('此文件类型不支持预览');
            }
        } catch (error) {
            this.showToast(`查看文件失败11: ${error}`);
            // 打印堆栈信息
            if (error instanceof Error) {
                console.error(error.message);
                console.error('错误堆栈:', error.stack); // 包含文件名和行号
            }
        }
    }


    private exportFile(filename: string, fileData: Uint8Array): void {
        try {
            let dataToExport = fileData;
            let exportFilename = filename;

            // 检查是否是 GZIP 文件
            if (this.mrpInfo?._isGzip(fileData)) {
                try {
                    dataToExport = pako.ungzip(fileData);
                    // 如果原始文件名有 .gz 后缀，去掉它
                    if (filename.toLowerCase().endsWith('.gz')) {
                        exportFilename = filename.slice(0, -3);
                    }
                    this.showToast('已自动解压 GZIP 文件');
                } catch (e) {
                    this.showToast('GZIP 解压失败，导出原始文件');
                }
            }

            // 获取文件扩展名
            const ext = exportFilename.split('.').pop()?.toLowerCase() || 'bin';

            // 创建 blob 对象
            const blob = new Blob([dataToExport], { type: 'application/octet-stream' });
            const url = URL.createObjectURL(blob);

            // 创建下载链接
            const a = document.createElement('a');
            a.href = url;
            a.download = exportFilename;
            document.body.appendChild(a);
            a.click();

            // 清理
            setTimeout(() => {
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
            }, 100);

            this.showToast(`文件 ${exportFilename} 导出成功`);
        } catch (error) {
            this.showToast(`导出文件失败: ${error}`);
        }
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
                if (error instanceof Error) {
                    console.error(error.message);
                    console.error('错误堆栈:', error.stack); // 包含文件名和行号
                }
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
            const offsetHex = `0x${file.fileOffset.toString(16).toUpperCase().padStart(8, '0')}`;

            row.innerHTML = `
            <td>${index + 1}</td>
            <td><i class="fas ${fileIconClass} file-icon"></i> ${file.filename}</td>
            <td>${offsetHex}</td>
            <td>${fileSizeKB} KB</td>
            <td>
                <div class="file-actions">
                    <button class="action-btn view-btn" title="查看"><i class="fas fa-eye"></i></button>
                    <button class="action-btn export-btn" title="导出"><i class="fas fa-download"></i></button>
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


