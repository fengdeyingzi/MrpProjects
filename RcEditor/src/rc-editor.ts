import RcList from "./rclist";
import {encodeBase64, decodeBase64} from "./base64";

class RCEditor {
    private rcList: RcList;
    private sheetBody: HTMLElement;
    private statusText: HTMLElement;
    private fileInput: HTMLInputElement;
    private activeCell: HTMLElement | null = null;
    private isReadStorage:Boolean = false;
    
    constructor() {
        this.rcList = new RcList();
        this.sheetBody = document.getElementById('sheetBody') as HTMLElement;
        this.statusText = document.getElementById('statusText') as HTMLElement;
        this.fileInput = document.getElementById('fileInput') as HTMLInputElement;
        
        this.initEventListeners();
        this.initSheet();

        // 从Storage读取rcfile
        const savedFile = localStorage.getItem('rcfile');
        const savedFileName = localStorage.getItem('rcfilename');
        if (savedFile && savedFileName && history.length > 1) {
            this.statusText.textContent = `正在加载: ${savedFile}`;
            //将saveFile进行base64解码
            const decodedData = decodeBase64(savedFile);
            const success = this.rcList.readData(savedFileName!, decodedData);
            if (success) {
                this.initSheet(this.rcList.size());
                this.updateSheet();
                this.isReadStorage = true;
                this.statusText.textContent = `已加载: ${savedFileName}`;
            } else {
                this.statusText.textContent = `加载失败: ${savedFileName}`;
            }
        }
            localStorage.removeItem('rcfile');
            localStorage.removeItem('rcfilename');
        
    }
    
    private initEventListeners(): void {
        document.getElementById('openFileBtn')?.addEventListener('click', () => this.fileInput.click());
        document.getElementById('saveFileBtn')?.addEventListener('click', () => this.saveFile());
        
        this.fileInput.addEventListener('change', (e) => this.handleFileSelect(e));
    }
    
    private async handleFileSelect(event: Event): Promise<void> {
        const input = event.target as HTMLInputElement;
        const file = input.files?.[0];
        if (!file) return;
        let data : Uint8Array = new Uint8Array(await file.arrayBuffer());
        let filename = file.name;
        this.statusText.textContent = `正在加载: ${file.name}`;
        
        try {
            const success = this.rcList.readData(filename, data);
            if (success) {
                this.initSheet(this.rcList.size());
                this.updateSheet();
                this.isReadStorage = false;
                this.statusText.textContent = `已加载: ${file.name}`;
            } else {
                this.statusText.textContent = `加载失败: ${file.name}`;
            }
        } catch (error) {
            console.error('加载文件错误:', error);
            this.statusText.textContent = `加载错误: ${file.name}`;
        }
    }
    
    private async saveFile(): Promise<void> {
        if (!this.rcList.filename) {
            this.statusText.textContent = '没有可保存的文件';
            return;
        }
        
        this.statusText.textContent = '正在保存...';
        if(!this.isReadStorage){
            try {
            const success = await this.rcList.save();
            if (success) {
                this.statusText.textContent = `已保存: ${this.rcList.filename}`;
            } else {
                this.statusText.textContent = '保存失败';
            }
        } catch (error) {
            console.error('保存文件错误:', error);
            this.statusText.textContent = '保存错误';
        }
        } else {
            // 如果是从Storage读取的文件，直接保存到Storage
            const data = this.rcList.getData();
            if(data == null){
                this.statusText.textContent = '生成数据失败';
                return;
            }
            const base64Data = encodeBase64(data!);
            localStorage.setItem('rcfile', base64Data);
            localStorage.setItem('rcfilename', this.rcList.filename);
            this.statusText.textContent = `已保存到Storage: ${this.rcList.filename}`;
            //判断是否存在上一页，如果存在就返回
            if (history.length > 1) {
                history.back();
            } else {
                this.statusText.textContent = '没有上一页可返回';
                console.log('没有上一页可返回');
            }
        }
    }
    
    private initSheet(rows: number = 50): void {
        this.sheetBody.innerHTML = '';
        
        for (let row = 1; row <= rows; row++) {
            const rowElement = document.createElement('div');
            rowElement.className = 'sheet-row';
            rowElement.dataset.row = row.toString();
            
            // 添加行号
            const rowNumber = document.createElement('div');
            rowNumber.className = 'row-number';
            rowNumber.textContent = row.toString();
            rowElement.appendChild(rowNumber);
            
            // 添加单元格
            const cell = document.createElement('div');
            cell.className = 'sheet-cell';
            cell.contentEditable = 'true';
            cell.dataset.row = row.toString();
            
            cell.addEventListener('focus', () => this.handleCellFocus(cell, row));
            cell.addEventListener('blur', () => this.handleCellBlur(cell, row));
            cell.addEventListener('input', () => this.handleCellInput(cell, row));
            cell.addEventListener('keydown', (e) => this.handleCellKeyDown(e, cell, row));
            
            rowElement.appendChild(cell);
            this.sheetBody.appendChild(rowElement);
        }
    }

    private handleCellKeyDown(event: KeyboardEvent, cell: HTMLElement, row: number): void {
        // Alt+Enter 换行
        if (event.key === 'Enter' && event.altKey) {
            document.execCommand('insertLineBreak');
            event.preventDefault();
            return;
        }
        
        // Enter 键跳转到下一行
        if (event.key === 'Enter' && !event.altKey) {
            event.preventDefault();
            this.moveToNextRow(row);
            return;
        }
        
        // Tab 键跳转到下一个单元格
        if (event.key === 'Tab') {
            event.preventDefault();
            this.moveToNextCell(row);
        }
    }

    private moveToNextRow(currentRow: number): void {
        const nextRow = currentRow + 1;
        const nextCell = this.sheetBody.querySelector(`.sheet-row[data-row="${nextRow}"] .sheet-cell`) as HTMLElement;
        
        if (nextCell) {
            this.activeCell?.classList.remove('editing');
            nextCell.focus();
        } else {
            // 如果已经是最后一行，添加新行
            this.addNewRow();
            const newCell = this.sheetBody.querySelector(`.sheet-row:last-child .sheet-cell`) as HTMLElement;
            newCell.focus();
        }
    }
    private moveToNextCell(currentRow: number): void {
        // RC编辑器只有一列，所以Tab键行为与Enter相同
        this.moveToNextRow(currentRow);
    }

    private addNewRow(): void {
        const rows = this.sheetBody.querySelectorAll('.sheet-row');
        const newRowNum = rows.length + 1;
        
        const rowElement = document.createElement('div');
        rowElement.className = 'sheet-row';
        rowElement.dataset.row = newRowNum.toString();
        
        // 添加行号
        const rowNumber = document.createElement('div');
        rowNumber.className = 'row-number';
        rowNumber.textContent = newRowNum.toString();
        rowElement.appendChild(rowNumber);
        
        // 添加单元格
        const cell = document.createElement('div');
        cell.className = 'sheet-cell';
        cell.contentEditable = 'true';
        cell.dataset.row = newRowNum.toString();
        
        // 添加事件监听器
        cell.addEventListener('focus', () => this.handleCellFocus(cell, newRowNum));
        cell.addEventListener('blur', () => this.handleCellBlur(cell, newRowNum));
        cell.addEventListener('input', () => this.handleCellInput(cell, newRowNum));
        cell.addEventListener('keydown', (e) => this.handleCellKeyDown(e, cell, newRowNum));
        
        rowElement.appendChild(cell);
        this.sheetBody.appendChild(rowElement);
        
        // 在RC列表中添加新项
        this.rcList.setText("", newRowNum - 1);
    }
    
    private updateSheet(): void {
        const rows = this.sheetBody.querySelectorAll('.sheet-row');
        
        for (let i = 0; i < rows.length; i++) {
            const row = rows[i];
            const cell = row.querySelector('.sheet-cell') as HTMLElement;
            const text = this.rcList.getText(i);
            
            if (text) {
                cell.textContent = text;
            } else {
                cell.textContent = '';
            }
        }
    }
    
    private handleCellFocus(cell: HTMLElement, row: number): void {
        if (this.activeCell) {
            this.activeCell.classList.remove('editing');
        }
        this.activeCell = cell;
        cell.classList.add('editing');
        this.updateStatus(row);
    }
    
    private handleCellBlur(cell: HTMLElement, row: number): void {
        cell.classList.remove('editing');
        this.adjustRowHeight(cell.parentElement as HTMLElement);
        this.saveCellContent(cell, row);
    }
    
    private handleCellInput(cell: HTMLElement, row: number): void {
        this.adjustRowHeight(cell.parentElement as HTMLElement);
    }
    
    private saveCellContent(cell: HTMLElement, row: number): void {
        const text = cell.textContent || '';
        this.rcList.setText(text, row - 1);
    }
    
    private adjustRowHeight(rowElement: HTMLElement): void {
        const cell = rowElement.querySelector('.sheet-cell') as HTMLElement;
        cell.style.height = 'auto';
        const height = Math.max(30, cell.scrollHeight);
        rowElement.style.height = `${height}px`;
        cell.style.height = '100%';
    }
    
    private updateStatus(row: number): void {
        this.statusText.textContent = `行 ${row}`;
    }
}


// 初始化编辑器
document.addEventListener('DOMContentLoaded', () => {
    new RCEditor();
});