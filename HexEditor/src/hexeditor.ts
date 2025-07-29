import $ from 'jquery';

interface JQuery {
    closest(selector: string): JQuery;
    data(key: string): any;
    toggleClass(className: string): JQuery;
    removeClass(className: string): JQuery;
    addClass(className: string): JQuery;
    empty(): JQuery;
    append(content: string | JQuery): JQuery;
    children(selector?: string): JQuery;
    last(): JQuery;
    hide(): JQuery;
    css(propertyName: string, value: string | number): JQuery;
    on(event: string, handler: (event: any) => void): JQuery;
    click(handler?: (event: any) => void): JQuery;
    change(handler?: (event: any) => void): JQuery;
    text(content: string): JQuery;
    length: number;
}

// declare const $: (selector: string | Element) => JQuery;

$(document).ready(function() {
    let fileData: Uint8Array | null = null;
    let selectionStart: number = -1;
    let selectionEnd: number = -1;
    let isSelecting: boolean = false;
    let touchStartTime: number = 0;
    let touchStartIndex: number = -1;
    let isTouchSelecting: boolean = false;
    let touchStartX: number = 0;
    let touchStartY: number = 0;
    let tempSelectStart: number = 0;
    let tempSelectEnd: number = 0;

    // 移动端菜单切换
    $('#mobile-menu-btn').on('click', function(e: JQuery.ClickEvent) {
        e.stopPropagation();
        $('#menu-bar').toggleClass('mobile-visible');
    });

    // 点击其他地方关闭菜单
    $(document).on('click', function(e: JQuery.ClickEvent) {
        if (!$(e.target).closest('#menu-bar').length && !$(e.target).is('#mobile-menu-btn')) {
            $('#menu-bar').removeClass('mobile-visible');
        }
    });

    // 初始化地址列
    function initAddressColumn(): void {
        const addressColumn = $('#address-column');
        addressColumn.empty();
        
        if (!fileData) return;
        
        const rows = Math.ceil(fileData.length / 16);
        for (let i = 0; i < rows; i++) {
            const address = i.toString(16).padStart(8, '0').toUpperCase();
            addressColumn.append(`<div>${address}</div>`);
        }
    }

    // 初始化十六进制和ASCII列
    function initHexView(): void {
        const hexColumn = $('#hex-column');
        const asciiColumn = $('#ascii-column');
        hexColumn.empty();
        asciiColumn.empty();
        
        if (!fileData) return;
        
        for (let i = 0; i < fileData.length; i++) {
            const byte = fileData[i];
            const hex = byte.toString(16).padStart(2, '0').toUpperCase();
            const ascii = (byte >= 32 && byte <= 126) ? String.fromCharCode(byte) : '.';
            
            hexColumn.append(`<div class="hex-byte" data-index="${i}">${hex}</div>`);
            
            if (i % 16 === 0) {
                asciiColumn.append('<div>');
            }
            
            $(asciiColumn.children().last()).append(ascii);
        }
    }

    // 打开文件
    $('#open-file, #mobile-open').click(function() {
        $('#file-input').click();
    });

    $('#file-input').change(function(e: JQuery.ChangeEvent) {
        const file = (e.target as HTMLInputElement).files?.[0];
        if (!file) return;
        
        $('#loader').css('display', 'flex');
        $('#loader-text').text(`加载 ${file.name}...`);
        $('#file-name').text(file.name);
        $('#file-size').text(`(${(file.size / 1024).toFixed(2)} KB)`);
        
        const reader = new FileReader();
        reader.onload = function(e: ProgressEvent<FileReader>) {
            const arrayBuffer = e.target?.result as ArrayBuffer;
            fileData = new Uint8Array(arrayBuffer);
            
            initAddressColumn();
            initHexView();
            
            $('#loader').hide();
        };
        reader.onerror = function() {
            $('#loader').hide();
            alert('文件加载失败');
        };
        reader.readAsArrayBuffer(file);
    });

    // 保存文件
    $('#save-file, #mobile-save').click(function() {
        if (!fileData) {
            alert('没有打开的文件');
            return;
        }
        
        const blob = new Blob([fileData], { type: 'application/octet-stream' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = $('#file-name').text() || 'untitled.bin';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
    });

    // 选择处理 - 鼠标
    $('.hex-column').on('mousedown', '.hex-byte', function(e: JQuery.MouseDownEvent) {
        e.preventDefault();
        if (e.button === 0) {
            console.log("左键点击");
            isSelecting = true;
            const index = parseInt($(this).data('index'));
            selectionStart = index;
            selectionEnd = index;
            updateSelection();
        }
    });

    $(document).on('mousemove', function(e: JQuery.MouseMoveEvent) {
        if (!isSelecting) return;
        e.preventDefault();
        
        const hexByte = $(e.target).closest('.hex-byte');
        if (hexByte.length) {
            console.log("hexByte:", hexByte.data('index'), hexByte);
            const index = parseInt(hexByte.data('index'));
            selectionEnd = index;
            updateSelection();
        }
    });

    $(document).on('mouseup', function() {
        isSelecting = false;
    });

    // 选择处理 - 触摸
    $('.hex-column').on('touchstart', '.hex-byte', function(this: HTMLElement,e: JQuery.Event) {
        e.preventDefault();
        touchStartTime = Date.now();
        touchStartIndex = parseInt($(this).data('index'));
        isTouchSelecting = false;
        tempSelectStart = touchStartIndex;
        tempSelectEnd = touchStartIndex;
        const touchEvent = (e as any).originalEvent as TouchEvent;
        touchStartY = touchEvent.touches[0].clientY;
        touchStartX = touchEvent.touches[0].clientX;
    });

    $('.hex-column').on('touchmove', '.hex-byte', function(e: JQuery.Event) {
        e.preventDefault();
        const touchEvent = (e as any).originalEvent as TouchEvent;
        const touch = touchEvent.touches[0];
        console.log("touchmove "+touch.clientY+" "+touch.clientX);
        
        if (!isTouchSelecting && (Math.abs(touch.clientY - touchStartY) > 10 || Math.abs(touch.clientX - touchStartX) > 10)) {
            isTouchSelecting = true;
        }
        
        if (isTouchSelecting) {
            const element = document.elementFromPoint(touch.clientX, touch.clientY);
            if (element) {
                const hexByte = $(element).closest('.hex-byte');
                console.log("hexByte:", hexByte.data('index'), hexByte);
                if (hexByte.length) {
                    const index = parseInt(hexByte.data('index'));
                    tempSelectEnd = index;
                    selectionStart = tempSelectStart;
                    selectionEnd = tempSelectEnd;
                    updateSelection();
                }
            }
        }
    });

    $('.hex-column').on('touchend', '.hex-byte', function(e: JQuery.Event) {
        e.preventDefault();
        const touchDuration = Date.now() - touchStartTime;
        const index = parseInt($(this).data('index'));
        
        if (touchDuration > 500 && !isTouchSelecting && touchStartIndex === index) {
            const rect = (this as HTMLElement).getBoundingClientRect();
            $('#context-menu').css({
                display: 'block',
                left: rect.left + 'px',
                top: rect.bottom + 'px'
            });
        } else if (touchDuration < 500 && !isTouchSelecting && touchStartIndex === index) {
            console.log("更新选择："+tempSelectStart+" "+tempSelectEnd);
            selectionStart = tempSelectStart;
            selectionEnd = tempSelectEnd;
            updateSelection();
        }
        console.log("touchStartIndex="+touchStartIndex+" index="+index+" isTouchSelecting="+isTouchSelecting);
        
        isTouchSelecting = false;
    });

    // 更新选择区域
    function updateSelection(): void {
        console.trace("当前调用堆栈:");
        $('.hex-byte').removeClass('selected');
        
        if (selectionStart === -1 || selectionEnd === -1) return;
        
        const start = Math.min(selectionStart, selectionEnd);
        const end = Math.max(selectionStart, selectionEnd);
        
        for (let i = start; i <= end; i++) {
            $(`.hex-byte[data-index="${i}"]`).addClass('selected');
        }
        
        const length = end - start + 1;
        $('#selection-info').text(`选择: ${length} 字节 (0x${start.toString(16).toUpperCase()} - 0x${end.toString(16).toUpperCase()})`);
    }

    // 右键菜单
    $(document).on('contextmenu', '.hex-byte', function(e: JQuery.ContextMenuEvent) {
        e.preventDefault();
        const index = parseInt($(this).data('index'));
        console.log("右键菜单："+selectionStart+","+index);
        
        if (selectionStart === -1) {
            selectionStart = index;
            selectionEnd = index;
            console.log("更新选择");
            updateSelection();
        }
        
        $('#context-menu').css({
            display: 'block',
            left: e.pageX + 'px',
            top: e.pageY + 'px'
        });
    });

    $(document).on('click', function(e: JQuery.ClickEvent) {
        if (!$(e.target).closest('.context-menu').length) {
            $('#context-menu').hide();
        }
    });

    // 显示详细信息
    $('#ctx-show-details, #toggle-details, #mobile-details').click(function() {
        showDetails();
    });

    $('#close-details').click(function() {
        $('#details-panel').hide();
    });

    function showDetails(): void {
        if (selectionStart === -1 || selectionEnd === -1) {
            alert('请先选择数据');
            return;
        }
        
        const start = Math.min(selectionStart, selectionEnd);
        const end = Math.max(selectionStart, selectionEnd);
        const length = end - start + 1;
        
        if (!fileData) return;
        const selectedData = fileData.slice(start, end + 1);
        
        // 十六进制
        let hexStr = '';
        for (let i = 0; i < selectedData.length; i++) {
            hexStr += selectedData[i].toString(16).padStart(2, '0').toUpperCase() + ' ';
            if ((i + 1) % 8 === 0) hexStr += '\n';
        }
        $('#detail-hex').text(hexStr.trim());
        
        // ASCII
        let asciiStr = '';
        for (let i = 0; i < selectedData.length; i++) {
            const byte = selectedData[i];
            asciiStr += (byte >= 32 && byte <= 126) ? String.fromCharCode(byte) : '.';
            if ((i + 1) % 16 === 0) asciiStr += '\n';
        }
        $('#detail-ascii').text(asciiStr.trim());
        
        // UTF-8
        try {
            const decoder = new TextDecoder('utf-8');
            $('#detail-utf8').text(decoder.decode(selectedData));
        } catch (e) {
            $('#detail-utf8').text('无效的UTF-8序列');
        }
        
        // 整数 (小端和大端)
        if (length <= 8) {
            let leValue = 0;
            let beValue = 0;
            
            for (let i = 0; i < length; i++) {
                leValue += selectedData[i] * Math.pow(256, i);
                beValue = beValue * 256 + selectedData[i];
            }
            
            $('#detail-le').text(`有符号: ${leValue}\n无符号: ${leValue >>> 0}`);
            $('#detail-be').text(`有符号: ${beValue}\n无符号: ${beValue >>> 0}`);
        } else {
            $('#detail-le').text('选择超过8字节');
            $('#detail-be').text('选择超过8字节');
        }
        
        // 浮点数 (IEEE 754)
        if (length === 4) {
            const view = new DataView(selectedData.buffer);
            $('#detail-float').text(`单精度: ${view.getFloat32(0, true)} (小端)`);
        } else if (length === 8) {
            const view = new DataView(selectedData.buffer);
            $('#detail-float').text(`双精度: ${view.getFloat64(0, true)} (小端)`);
        } else {
            $('#detail-float').text('选择4或8字节查看浮点数');
        }
        
        // 二进制
        let binaryStr = '';
        for (let i = 0; i < selectedData.length; i++) {
            binaryStr += selectedData[i].toString(2).padStart(8, '0') + ' ';
            if ((i + 1) % 4 === 0) binaryStr += '\n';
        }
        $('#detail-binary').text(binaryStr.trim());
        
        $('#details-panel').show();
    }

    // 复制功能
    $('#ctx-copy-hex, #copy-hex, #mobile-copy-hex').click(function() {
        if (selectionStart === -1 || selectionEnd === -1 || !fileData) {
            alert('请先选择要复制的数据');
            return;
        }
        
        const start = Math.min(selectionStart, selectionEnd);
        const end = Math.max(selectionStart, selectionEnd);
        const selectedData = fileData.slice(start, end + 1);
        
        let hexStr = '';
        for (let i = 0; i < selectedData.length; i++) {
            hexStr += selectedData[i].toString(16).padStart(2, '0').toUpperCase() + ' ';
        }
        // 关闭 ctx
        $('#context-menu').hide();

        navigator.clipboard.writeText(hexStr.trim()).then(() => {
            alert('十六进制数据已复制到剪贴板');
        }).catch(err => {
            alert('复制失败: ' + err);
        });
    });

    $('#ctx-copy-ascii, #copy-ascii').click(function() {
        if (selectionStart === -1 || selectionEnd === -1 || !fileData) {
            alert('请先选择要复制的数据');
            return;
        }
        
        const start = Math.min(selectionStart, selectionEnd);
        const end = Math.max(selectionStart, selectionEnd);
        const selectedData = fileData.slice(start, end + 1);
        
        let asciiStr = '';
        for (let i = 0; i < selectedData.length; i++) {
            const byte = selectedData[i];
            asciiStr += (byte >= 32 && byte <= 126) ? String.fromCharCode(byte) : '.';
        }
        
        navigator.clipboard.writeText(asciiStr).then(() => {
            alert('ASCII数据已复制到剪贴板');
        }).catch(err => {
            alert('复制失败: ' + err);
        });
    });

    // 其他功能占位
    $('#find-data').click(function() {
        alert('查找功能将在后续版本中实现');
    });

    $('#change-theme').click(function() {
        alert('主题切换功能将在后续版本中实现');
    });

    $('#show-help').click(function() {
        alert('帮助文档将在后续版本中实现');
    });

    $('#about-app').click(function() {
        alert('Hex Editor v1.0\n一个简单的十六进制编辑器\n支持查看和编辑二进制文件');
    });

    $('#exit-app').click(function() {
        if (confirm('确定要退出吗？')) {
            window.close();
        }
    });
});