import {encodeBase64, decodeBase64} from "./base64";

interface MAP565Data {
    header: string;
    width: number;
    height: number;
    trcolor: string;
    data: Uint8Array;
}

class Map565Loader {
    static async getMAP565Data(data: Uint8Array): Promise<MAP565Data> {
        // const header = String.fromCharCodes(data.slice(0, 4));
        const header = Array.from(data.slice(0, 4)).map(code => String.fromCharCode(code)).join('');
        if (header !== 'MAP5' && header !== 'MAP8') {
            const possibleWidths = [];
            const len = data.length / 2;
            
            for (let i = 8; i <= 240; i++) {
                if (len % i === 0) {
                    possibleWidths.push(i);
                }
            }
            
            const width = possibleWidths[Math.floor(possibleWidths.length / 2)];
            return this.getMAP565DataWidth(data, width);
        }

        const view = new DataView(data.buffer);
        const width = view.getUint16(4, true);
        const height = view.getUint16(8, true);
        const transparentColor = view.getUint16(12, true);
        
        return {
            header,
            width,
            height,
            trcolor: this.convert565ToColor(transparentColor),
            data: data.slice(14)
        };
    }

    static getMAP565DataWidth(data: Uint8Array, width: number): MAP565Data {
        const height = Math.floor(data.length / 2 / width);
        return {
            header: "",
            width,
            height,
            trcolor: this.convert565ToColor(0),
            data
        };
    }

    static convert565ToRGB(color565: number): number {
        const r = ((color565 >> 11) & 0x1F) << 3;
        const g = ((color565 >> 5) & 0x3F) << 2;
        const b = (color565 & 0x1F) << 3;
        return (r << 16) | (g << 8) | b;
    }

    static convert565ToColor(color565: number): string {
        const rgb = this.convert565ToRGB(color565);
        return `#${(0x1000000 + rgb).toString(16).slice(1)}`;
    }

    static convertColorTo565(color: string): number {
        const hex = color.replace('#', '');
        const r = parseInt(hex.substr(0, 2), 16) >> 3;
        const g = parseInt(hex.substr(2, 2), 16) >> 2;
        const b = parseInt(hex.substr(4, 2), 16) >> 3;
        return ((r << 11) | (g << 5) | b);
    }

    static async loadMap565Data(mapData: MAP565Data, isAlpha = false): Promise<ImageData> {
        const { width, height, data } = mapData;
        const canvas = document.createElement('canvas');
        canvas.width = width;
        canvas.height = height;
        const ctx = canvas.getContext('2d')!;
        const imageData = ctx.createImageData(width, height);
        const view = new DataView(data.buffer);
        const transparentColor = this.convertColorTo565(mapData.trcolor);

        for (let y = 0; y < height; y++) {
            for (let x = 0; x < width; x++) {
                const idx = (y * width + x) * 4;
                const pixelData = view.getUint16(((y * width + x) * 2), true);
                
                if (pixelData === transparentColor && isAlpha) {
                    imageData.data[idx] = 0;
                    imageData.data[idx + 1] = 0;
                    imageData.data[idx + 2] = 0;
                    imageData.data[idx + 3] = 0;
                } else {
                    imageData.data[idx] = ((pixelData >> 11) & 0x1F) << 3;
                    imageData.data[idx + 1] = ((pixelData >> 5) & 0x3F) << 2;
                    imageData.data[idx + 2] = (pixelData & 0x1F) << 3;
                    imageData.data[idx + 3] = 255;
                }
            }
        }

        return imageData;
    }
}

class Map565Viewer {
    private container: HTMLElement;
    private mapData: MAP565Data | null = null;
    private canvas: HTMLCanvasElement;
    private ctx: CanvasRenderingContext2D;
    private scale = 1;
    private isAlpha = true;
     private offsetX = 0;
    private offsetY = 0;
    private isDragging = false;
    private dragStartX = 0;
    private dragStartY = 0;

    constructor(container: HTMLElement) {
        this.container = container;
        
        // 检查是否已存在 canvas，如果存在则移除
        const existingCanvas = this.container.querySelector('canvas');
        if (existingCanvas) {
            this.container.removeChild(existingCanvas);
        }
        
        // 创建新的 canvas
        this.canvas = document.createElement('canvas');
        this.ctx = this.canvas.getContext('2d')!;
        this.container.appendChild(this.canvas);
        
        this.setupUI();
        this.setupInteractions();
    }

    private setupUI(): void {
        const controls = document.createElement('div');
        controls.style.position = 'absolute';
        controls.style.bottom = '10px';
        controls.style.left = '10px';
        controls.style.backgroundColor = 'rgba(0,0,0,0.7)';
        controls.style.padding = '10px';
        controls.style.borderRadius = '5px';
        controls.style.color = 'white';

        const info = document.createElement('div');
        info.id = 'map565-info';
        controls.appendChild(info);

        const alphaToggle = document.createElement('button');
        alphaToggle.textContent = '切换透明度';
        alphaToggle.addEventListener('click', () => {
            this.isAlpha = !this.isAlpha;
            this.render();
        });
        controls.appendChild(alphaToggle);

        const widthDown = document.createElement('button');
        widthDown.textContent = '宽度减';
        widthDown.addEventListener('click', () => this.adjustWidth(-1));
        controls.appendChild(widthDown);

        const widthUp = document.createElement('button');
        widthUp.textContent = '宽度加';
        widthUp.addEventListener('click', () => this.adjustWidth(1));
        controls.appendChild(widthUp);

        const savePng = document.createElement('button');
        savePng.textContent = '保存为PNG';
        savePng.addEventListener('click', () => this.saveAsPng());
        controls.appendChild(savePng);

        const saveTransPng = document.createElement('button');
        saveTransPng.textContent = '保存为透明PNG';
        saveTransPng.addEventListener('click', () => this.saveAsPng(true));
        controls.appendChild(saveTransPng);

        this.container.appendChild(controls);
    }

    private setupInteractions(): void {
        // 鼠标滚轮缩放
        this.container.addEventListener('wheel', (e) => {
            e.preventDefault();
            const delta = -e.deltaY;
            const zoomFactor = delta > 0 ? 1.1 : 0.9;
            this.zoom(zoomFactor, e.clientX, e.clientY);
        });
        // 判断是否为手机模式
        if ('ontouchstart' in window) {
            console.log("当前为手机模式");
            // 手机触摸缩放
            let startDistance = 0;
            let initialScale = this.scale;

            this.container.addEventListener('touchstart', (e) => {
                console.log("touchstart");
                if (e.touches.length === 2) {
                    startDistance = Math.hypot(
                        e.touches[0].clientX - e.touches[1].clientX,
                        e.touches[0].clientY - e.touches[1].clientY
                    );
                    initialScale = this.scale;
                }else if(e.touches.length == 1){
                    this.isDragging = true;
            this.dragStartX = e.touches[0].clientX - this.offsetX;
            this.dragStartY = e.touches[0].clientY - this.offsetY;
            this.canvas.style.cursor = 'grabbing';
                }
            });

            this.container.addEventListener('touchmove', (e) => {
                if (e.touches.length === 2) {
                    const currentDistance = Math.hypot(
                        e.touches[0].clientX - e.touches[1].clientX,
                        e.touches[0].clientY - e.touches[1].clientY
                    );
                    const scaleFactor = currentDistance / startDistance;
                    this.zoom(initialScale * scaleFactor, undefined, undefined);
                }
                else if(e.touches.length == 1 && this.isDragging){
                    this.offsetX = e.touches[0].clientX - this.dragStartX;
                    this.offsetY = e.touches[0].clientY - this.dragStartY;
                    this.updateCanvasPosition();
                }
            });
            this.container.addEventListener('touchend', () => {
                this.isDragging = false;
                this.canvas.style.cursor = 'grab';
            });
        } else {
            console.log("当前为电脑模式");
            // 鼠标拖动
        this.canvas.addEventListener('mousedown', (e) => {
            this.isDragging = true;
            this.dragStartX = e.clientX - this.offsetX;
            this.dragStartY = e.clientY - this.offsetY;
            this.canvas.style.cursor = 'grabbing';
        });
        document.addEventListener('mousemove', (e) => {
            if (!this.isDragging) return;
            this.offsetX = e.clientX - this.dragStartX;
            this.offsetY = e.clientY - this.dragStartY;
            this.updateCanvasPosition();
        });
        document.addEventListener('mouseup', () => {
            this.isDragging = false;
            this.canvas.style.cursor = 'grab';
        });
        }

        
    }

    private zoom(factor: number, mouseX?: number, mouseY?: number): void {
        const oldScale = this.scale;
        this.scale *= factor;
        this.scale = Math.max(0.1, Math.min(this.scale, 10)); // 限制缩放范围
        // 以左上角为原点进行缩放
        if (mouseX !== undefined && mouseY !== undefined) {
            // 计算鼠标在容器中的相对位置
            const rect = this.container.getBoundingClientRect();
            const containerX = mouseX - rect.left;
            const containerY = mouseY - rect.top;
            
            // 计算鼠标在内容上的位置（相对于左上角）
            const contentX = containerX - this.offsetX;
            const contentY = containerY - this.offsetY;
            
            // 调整偏移量保持左上角固定
            this.offsetX = containerX - (contentX / oldScale) * this.scale;
            this.offsetY = containerY - (contentY / oldScale) * this.scale;
        }
        this.updateCanvasPosition();
    }
    private updateCanvasPosition(): void {
        this.canvas.style.transform = `translate(${this.offsetX}px, ${this.offsetY}px) scale(${this.scale})`;
    }
    public zoomIn(): void {
        // 以左上角为原点放大
        this.zoom(1.2, this.container.getBoundingClientRect().left, 
                      this.container.getBoundingClientRect().top);
    }
    public zoomOut(): void {
        // 以左上角为原点缩小
        this.zoom(0.8, this.container.getBoundingClientRect().left, 
                      this.container.getBoundingClientRect().top);
    }
    public resetZoom(): void {
        this.scale = 1;
        this.offsetX = 0;
        this.offsetY = 0;
        this.updateCanvasPosition();
    }

    private async adjustWidth(direction: number): Promise<void> {
        if (!this.mapData) return;

        const len = this.mapData.data.length / 2;
        let width = this.mapData.width;
        let newWidth = width;

        if (direction > 0) {
            for (let i = width + 1; i <= 320; i++) {
                if (len % i === 0) {
                    newWidth = i;
                    break;
                }
            }
        } else {
            for (let i = width - 1; i > 2; i--) {
                if (len % i === 0) {
                    newWidth = i;
                    break;
                }
            }
        }

        if (newWidth !== width) {
            this.mapData.width = newWidth;
            this.mapData.height = Math.floor(len / newWidth);
            await this.render();
            this.updateInfo();
        }
    }

    private updateInfo(): void {
        if (!this.mapData) return;
        
        const info = document.getElementById('map565-info')!;
        info.innerHTML = `
            Width: ${this.mapData.width} 
            Height: ${this.mapData.height}
            <div style="display:inline-block; width:20px; height:20px; background-color:${this.mapData.trcolor};"></div>
        `;
    }

    public async loadFile(file: File): Promise<void> {
        const data = new Uint8Array(await file.arrayBuffer());
        this.mapData = await Map565Loader.getMAP565Data(data);
        await this.render();
        this.updateInfo();
    }

    private async render(): Promise<void> {
        if (!this.mapData) return;

        const imageData = await Map565Loader.loadMap565Data(this.mapData, this.isAlpha);
        this.canvas.width = this.mapData.width;
        this.canvas.height = this.mapData.height;
        this.ctx.putImageData(imageData, 0, 0);
    }

    private saveAsPng(transparent = false): void {
        if (!this.mapData) return;

        const link = document.createElement('a');
        link.download = `map565_${this.mapData.width}x${this.mapData.height}.png`;
        link.href = this.canvas.toDataURL('image/png');
        link.click();
    }
}


class Map565ViewerApp {
    private viewerContainer: HTMLElement;
    private fileInput: HTMLInputElement;
    private openBtn: HTMLButtonElement;
    private viewer: Map565Viewer;
        private zoomInBtn: HTMLButtonElement;
    private zoomOutBtn: HTMLButtonElement;
    private resetZoomBtn: HTMLButtonElement;
    private menuToggle: HTMLButtonElement;
    private toolbarButtons: HTMLDivElement;

    constructor() {
        // 初始化所有属性
        this.viewerContainer = document.getElementById('viewer-container') as HTMLElement;
        this.fileInput = document.getElementById('file-input') as HTMLInputElement;
        this.openBtn = document.getElementById('open-btn') as HTMLButtonElement;

         this.zoomInBtn = document.getElementById('zoom-in-btn') as HTMLButtonElement;
        this.zoomOutBtn = document.getElementById('zoom-out-btn') as HTMLButtonElement;
        this.resetZoomBtn = document.getElementById('reset-zoom-btn') as HTMLButtonElement;
        // 添加到工具栏
        const toolbar = document.getElementById('toolbar') as HTMLElement;
        // toolbar.appendChild(this.zoomInBtn);
        // toolbar.appendChild(this.zoomOutBtn);
        // toolbar.appendChild(this.resetZoomBtn);

        this.viewer = new Map565Viewer(this.viewerContainer);

        // 确保元素存在
        if (!this.viewerContainer || !this.fileInput || !this.openBtn) {
            throw new Error('Required HTML elements not found');
        }

        // 初始化元素
        this.menuToggle = document.getElementById('menu-toggle') as HTMLButtonElement;
        this.toolbarButtons = document.getElementById('toolbar-buttons') as HTMLDivElement;
        // 设置移动端菜单切换
        this.menuToggle.addEventListener('click', () => {
            this.toolbarButtons.classList.toggle('show');
        });
        // 点击按钮后关闭移动菜单
        this.toolbarButtons.querySelectorAll('button').forEach(button => {
            button.addEventListener('click', () => {
                if (window.innerWidth <= 768) {
                    this.toolbarButtons.classList.remove('show');
                }
            });
        });
        // 点击外部关闭菜单
        document.addEventListener('click', (e) => {
            if (window.innerWidth > 768) return;
            
            const target = e.target as HTMLElement;
            if (!toolbar.contains(target) && target !== this.menuToggle) {
                this.toolbarButtons.classList.remove('show');
            }
        });
        // 窗口大小变化时调整
        window.addEventListener('resize', () => {
            if (window.innerWidth > 768) {
                this.toolbarButtons.classList.remove('show');
            }
        });
        this.setupEventListeners();

        // 从storage加载文件
        let bmpfilename = localStorage.getItem("bmpfilename");
        let bmpfiledata = localStorage.getItem("bmpfiledata");
        if(bmpfilename && bmpfiledata){
            localStorage.removeItem("bmpfilename");
            localStorage.removeItem("bmpfiledata");
            // 对filedata进行base64解码
            const decodedData = decodeBase64(bmpfiledata);
            // 封装成file
            const file = new File([decodedData], bmpfilename, { type: 'image/bmp' });
            // 调用loadFile方法加载
            this.viewer.loadFile(file).then(() => {
                console.log(`加载文件: ${bmpfilename}`);
            });
            
        }
    }

    


    private createButton(text: string, id: string): HTMLButtonElement {
        const btn = document.createElement('button');
        btn.textContent = text;
        btn.id = id;
        btn.style.marginRight = '10px';
        btn.style.padding = '8px 16px';
        btn.style.borderRadius = '4px';
        btn.style.border = 'none';
        btn.style.cursor = 'pointer';
        btn.style.backgroundColor = '#0d5bbc';
        btn.style.color = 'white';
        return btn;
    }


    private setupEventListeners(): void {
        // Open file dialog when button is clicked
        this.openBtn.addEventListener('click', () => this.fileInput.click());

        // Handle file selection
        this.fileInput.addEventListener('change', async (e: Event) => {
            const target = e.target as HTMLInputElement;
            const file = target.files?.[0];
            if (file) {
                await this.viewer.loadFile(file);
            }
        });

        // Enable drag and drop
        this.viewerContainer.addEventListener('dragover', (e: DragEvent) => {
            e.preventDefault();
            this.viewerContainer.style.backgroundColor = '#444';
        });

        this.viewerContainer.addEventListener('dragleave', () => {
            this.viewerContainer.style.backgroundColor = '#333';
        });

        this.viewerContainer.addEventListener('drop', async (e: DragEvent) => {
            e.preventDefault();
            this.viewerContainer.style.backgroundColor = '#333';
            
            const file = e.dataTransfer?.files[0];
            if (file) {
                await this.viewer.loadFile(file);
            }
        });
        // 缩放按钮监听器
        this.zoomInBtn.addEventListener('click', () => this.viewer.zoomIn());
        this.zoomOutBtn.addEventListener('click', () => this.viewer.zoomOut());
        this.resetZoomBtn.addEventListener('click', () => this.viewer.resetZoom());
        // 拖放监听器
        this.setupDragAndDrop();
    }

    private setupDragAndDrop(): void {
        this.viewerContainer.addEventListener('dragover', (e: DragEvent) => {
            e.preventDefault();
            this.viewerContainer.style.backgroundColor = '#444';
        });
        this.viewerContainer.addEventListener('dragleave', () => {
            this.viewerContainer.style.backgroundColor = '#333';
        });
        this.viewerContainer.addEventListener('drop', async (e: DragEvent) => {
            e.preventDefault();
            this.viewerContainer.style.backgroundColor = '#333';
            const file = e.dataTransfer?.files[0];
            if (file) await this.viewer.loadFile(file);
        });
    }
}

// Initialize the application when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    new Map565ViewerApp();
});
