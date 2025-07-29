class RcList {
    public filename: string | null = null;
    private texts: string[] = [];
    public gb: number = 0;
    public load: boolean = false;
    
    private firstItem: number = 0; // 链表首项，当前项
    private addrSize: number = 0;  // 偏移所用字节数，<64K时为2位，>64K为4位
    private rcStrCode: number = 0; // RC字符串编码，0 = Unicode Big，1 = GB2312

    public static readonly UNICODE_BIG = 0;
    public static readonly GB2312 = 1;
    public static readonly UTF8 = 2;

    // 空构造
    constructor() {
        this.texts = [];
        this.load = false;
    }

    // 新建空列表
    public static createEmpty(filename: string, capacity: number): RcList {
        const rcList = new RcList();
        rcList.filename = filename;
        for (let i = 0; i < capacity; i++) {
            rcList.texts.push("");
        }
        rcList.load = true;
        return rcList;
    }

    // 读取文件
    public async readFile(filename: string): Promise<boolean> {
        this.filename = filename;
        
        try {
            const response = await fetch(filename);
            if (!response.ok) {
                console.error("文件不存在或无法访问:", filename);
                return false;
            }

            const buffer = await response.arrayBuffer();
            const result = this.readRc(new Uint8Array(buffer));
            this.load = result > 0;
            return this.load;
        } catch (e) {
            console.error("读取文件错误:", filename, e);
            return false;
        }
    }

    public readData(filename:string, data:Uint8Array) : Boolean{
        this.filename = filename;
        const result = this.readRc(data);
        this.load = result > 0;
        return this.load;
    }

    private readRc(rc: Uint8Array): number {
        if (rc.length < 10) return -1;

        const rcInt: number[] = [];
        for (let i = 0; i < rc.length / 2; i++) {
            const r1 = rc[i * 2];
            const r2 = rc[i * 2 + 1];
            rcInt.push((r2 << 8) | (r1 & 0xff));
            
            if (i < 9) {
                console.log("信息:", (r2 * 256 + r1), rcInt[i]);
            }
        }
        // 判断是否为gbk编码
        if(rc[rc.length-1] == 0 && rc[rc.length-2]!=0){
            this.rcStrCode = RcList.GB2312;
        }
        const endPtr = this.findEndPointer(rcInt);
        console.log("信息: endptr", endPtr);

        const math = this.calculateItemCount(rcInt, endPtr);
        this.parseRcItems(rc, rcInt, math);

        return math;
    }

    private findEndPointer(rcInt: number[]): number {
        if (rcInt[rcInt.length - 2] === 0) {
            return rcInt.length - 2;
        }

        for (let i = rcInt.length - 2; i >= 0; i--) {
            if (rcInt[i] === 0) {
                return i + 1;
            }
        }
        return rcInt.length;
    }

    // 计算条目数量
    private calculateItemCount(rcInt: number[], endPtr: number): number {
        let math = 1;
        let rOffset = 0;
        let tempOffset;

        while (true) {
            if (math >= rcInt.length) break;
            
            tempOffset = rOffset;
            rOffset = rcInt[math] / 2;
            
            if (tempOffset >= rOffset) break;
            if (rOffset >= (rcInt.length - math)) break;
            if (rcInt[math] / 2 + math + 1 === endPtr) {
                math++;
                break;
            }
            math++;
        }
        
        return math;
    }

    private parseRcItems(rc: Uint8Array, rcInt: number[], itemCount: number): void {
        this.texts = [];
        
        for (let i = 0; i < itemCount; i++) {
            const temp = new Uint8Array(rc.length);
            this.wstrcpy(temp, rc, rcInt[i] + itemCount * 2);
            this.texts[i] = this.unToChars(temp);
        }
    }

    public async save(): Promise<boolean> {
        if (!this.filename) {
            return false;
        }

        try {
            const rcIntb = this.prepareSaveData();
            const data = new Uint8Array(rcIntb.length + this.texts.reduce((acc, text) => acc + text.length * 2 + 2, 0));
            
            let offset = 0;
            data.set(rcIntb, offset);
            offset += rcIntb.length;

            const rc0 = new Uint8Array([0, 0]);
            for (const text of this.texts) {
                const bytes = this.getBytes(text);
                data.set(bytes, offset);
                offset += bytes.length;
                data.set(rc0, offset);
                offset += 2;
                console.log("offset = "+offset+" data len="+data.length);
            }

            // 在浏览器环境中模拟文件保存
            const blob = new Blob([data], { type: 'application/octet-stream' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = this.filename;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);

            this.load = true;
            return true;
        } catch (e) {
            console.error("保存文件错误:", this.filename, e);
            return false;
        }
    }

    public getData(): Uint8Array|null{
        try {
            const rcIntb = this.prepareSaveData();
            const data = new Uint8Array(rcIntb.length + this.texts.reduce((acc, text) => acc + text.length * 2 + 2, 0));
            
            let offset = 0;
            data.set(rcIntb, offset);
            offset += rcIntb.length;

            const rc0 = new Uint8Array([0, 0]);
            for (const text of this.texts) {
                const bytes = this.getBytes(text);
                data.set(bytes, offset);
                offset += bytes.length;
                data.set(rc0, offset);
                offset += 2;
                console.log("offset = "+offset+" data len="+data.length);
            }

            
            return data;
        } catch (e) {
            console.error("保存文件错误:", this.filename, e);
            return null;
        }
    }

    private prepareSaveData(): Uint8Array {
        const rcIntb = new Uint8Array(this.size() * 2);
        let tempInt = 0;

        for (let i = 0; i < this.size(); i++) {
            rcIntb[i * 2] = tempInt & 0xff;
            rcIntb[i * 2 + 1] = (tempInt >> 8) & 0xff;
            tempInt += (this.getText(i).length * 2 + 2);
        }
        
        return rcIntb;
    }

    public size(): number {
        return this.texts.length;
    }

    public getText(n: number): string {
        if (n < 0 || n >= this.texts.length) {
            return "";
        }
        return this.texts[n];
    }

    public setText(text: string, n: number): void {
        if (n >= 0 && n < this.texts.length) {
            this.texts[n] = text;
        }
    }

    public addBack(text: string): void {
        this.texts.splice(this.gb + 1, 0, text);
    }

    public addHead(text: string): void {
        this.texts.splice(this.gb, 0, text);
    }

    public remove(i: number): void {
        if (i >= 0 && i < this.texts.length) {
            this.texts.splice(i, 1);
        }
    }

    private stringToUtf16BeArray(str: string): Uint8Array {
  // 创建一个足够大的ArrayBuffer（每个字符2字节 + 可能的BOM）
  const buffer = new ArrayBuffer(str.length * 2);
  const view = new DataView(buffer);
  
  // 写入UTF-16BE编码的字符
  for (let i = 0; i < str.length; i++) {
    const code = str.charCodeAt(i);
    view.setUint16(i * 2, code, false); // false表示大端字节序(BE)
  }
  
  return new Uint8Array(buffer);
}

    private getBytes(str: string): Uint8Array {
        return this.stringToUtf16BeArray(str);
    }

    private unToChars(temp: Uint8Array): string {
        const decoder = new TextDecoder('utf-16be');
        return decoder.decode(temp.slice(0, this.wstrlen(temp)));
    }

    private wstrcpy(dest: Uint8Array, src: Uint8Array, srcStart: number): void {
        let i = 0;
        while (srcStart + i + 1 < src.length && 
               i + 1 < dest.length && 
               !(src[srcStart + i] === 0 && src[srcStart + i + 1] === 0)) {
            dest[i] = src[srcStart + i];
            dest[i + 1] = src[srcStart + i + 1];
            i += 2;
        }
        
        if (i + 1 < dest.length) {
            dest[i] = 0;
            dest[i + 1] = 0;
        }
    }

    private wstrlen(temp: Uint8Array): number {
        let i = 0;
        while (i + 1 < temp.length && !(temp[i] === 0 && temp[i + 1] === 0)) {
            i += 2;
        }
        return i;
    }
}

// 导出类以便其他模块使用
export default RcList;
