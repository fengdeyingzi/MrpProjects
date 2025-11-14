import $ from 'jquery';
import { CustomScrollbar } from './scrollbar';

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
    on(event: string, selector: string, handler: (event: any) => void): JQuery;
    click(handler?: (event: any) => void): JQuery;
    change(handler?: (event: any) => void): JQuery;
    text(content: string): JQuery;
    text(): string;
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

    let scrollbar: CustomScrollbar;
    // 显示的起始行数
    let viewStartRow: number = 0;
    // 最多可以显示多少行
    let viewMaxRows: number = 10;
    // 每行显示的数量 默认16
    const bytesPerRow: number = 16;
    
    // 编辑相关状态
    let isEditing: boolean = false;
    let editingIndex: number = -1;
    let editingNibble: number = 0; // 0: 高位 1: 低位
    

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
// 创建滚动条实例
   scrollbar = new CustomScrollbar('#scrollbar');

   // 添加键盘事件监听
   $(document).on('keydown', handleKeydown);
   $(document).on('keypress', handleKeypress);

    // 初始化地址列
    function initAddressColumn(): void {
        const addressColumn = $('#address-column');
        // 清空地址列
        addressColumn.empty();
        
        if (!fileData) return;
        
        // 根据当前的viewStartRow和viewMaxRows显示地址
        for (let row = viewStartRow; row < viewStartRow + viewMaxRows; row++) {
            const address = (row * bytesPerRow).toString(16).padStart(8, '0').toUpperCase();
            addressColumn.append(`<div>${address}</div>`);
        }
    }

    // 初始化进度
    function initView() {
        if (!fileData) {
            return;
        }
        
        // 计算hex-content的高度
        const hexContent = $('#hex-content');
        const hexContentHeight = hexContent.height();
        if (hexContentHeight) {
            viewMaxRows = Math.floor(hexContentHeight / 30);
        }
        
        // 计算文件的最大行数
        let fileMaxRows = Math.ceil(fileData.length / bytesPerRow);
        
        // 设置滚动条参数
        scrollbar.setMaxRows(fileMaxRows);
        scrollbar.setVisibleRows(viewMaxRows);
        scrollbar.scrollToTop(); // 初始化滚动到顶部
        
        // 监听滚动事件，同步内容区域滚动
        scrollbar.onScroll((currentRow) => {
            viewStartRow = currentRow;
            
            // 如果正在编辑的字节超出了可见范围，停止编辑
            if (isEditing) {
                const editingRow = Math.floor(editingIndex / bytesPerRow);
                if (editingRow < viewStartRow || editingRow >= viewStartRow + viewMaxRows) {
                    stopEditing();
                }
            }
            
            initAddressColumn();
            initHexView();
            
            // 滚动后恢复编辑状态（如果编辑的字节仍在可见范围内）
            if (isEditing && editingIndex >= 0) {
                const editingRow = Math.floor(editingIndex / bytesPerRow);
                if (editingRow >= viewStartRow && editingRow < viewStartRow + viewMaxRows) {
                    const $editingByte = $(`.hex-byte[data-index="${editingIndex}"]`);
                    $editingByte.addClass('editing');
                    if (editingNibble === 0) {
                        $editingByte.attr('data-cursor', 'high');
                    } else {
                        $editingByte.attr('data-cursor', 'low');
                    }
                }
            }
        });
    }

    // 初始化十六进制和ASCII列
    function initHexView(): void {
        const hexColumn = $('#hex-column');
        const asciiColumn = $('#ascii-column');
        hexColumn.empty();
        asciiColumn.empty();
        
        if (!fileData) return;
        
        const fileMaxRows = Math.ceil(fileData.length / bytesPerRow);
        const maxDisplayRows = Math.min(viewMaxRows, fileMaxRows - viewStartRow);
        
        for(let row = viewStartRow; row < viewStartRow + maxDisplayRows; row++){
            for(let col = 0; col < bytesPerRow; col++){
                const index = row * bytesPerRow + col;
                if (index >= fileData.length) break;
                
                const byte = fileData[index];
                const hex = byte.toString(16).padStart(2, '0').toUpperCase();
                const ascii = (byte >= 32 && byte <= 126) ? String.fromCharCode(byte) : '.';
                
                hexColumn.append(`<div class="hex-byte" data-index="${index}">${hex}</div>`);
                
                if (col % 16 === 0) {
                    asciiColumn.append('<div>');
                }
                
                $(asciiColumn.children().last()).append(ascii);
            }
        }
        
        // 重新渲染后，恢复编辑状态
        if (isEditing && editingIndex >= 0) {
            const editingRow = Math.floor(editingIndex / bytesPerRow);
            if (editingRow >= viewStartRow && editingRow < viewStartRow + viewMaxRows) {
                const $editingByte = $(`.hex-byte[data-index="${editingIndex}"]`);
                $editingByte.addClass('editing');
                if (editingNibble === 0) {
                    $editingByte.attr('data-cursor', 'high');
                } else {
                    $editingByte.attr('data-cursor', 'low');
                }
            }
        }
    }

    // 开始编辑指定位置的字节
    function startEditing(index: number): void {
        if (!fileData || index < 0 || index >= fileData.length) return;
        
        isEditing = true;
        editingIndex = index;
        editingNibble = 0;
        
        // 高亮编辑中的字节
        $('.hex-byte').removeClass('editing').removeAttr('data-cursor');
        const $editingByte = $(`.hex-byte[data-index="${index}"]`);
        $editingByte.addClass('editing').attr('data-cursor', 'high');
        
        // 清除选择
        selectionStart = -1;
        selectionEnd = -1;
        updateSelection();
    }

    // 停止编辑
    function stopEditing(): void {
        if (!isEditing) return;
        
        console.log("停止编辑，调用堆栈：", new Error().stack);
        
        isEditing = false;
        editingIndex = -1;
        editingNibble = 0;
        
        $('.hex-byte').removeClass('editing').removeAttr('data-cursor');
    }

    // 处理键盘按下事件
    function handleKeydown(e: JQuery.KeyDownEvent): void {
        if (!fileData) return;
        
        switch (e.key) {
            case 'Escape':
                stopEditing();
                break;
                
            case 'ArrowLeft':
            case 'ArrowRight':
            case 'ArrowUp':
            case 'ArrowDown':
                handleArrowKeys(e);
                break;
                
            case 'Delete':
            case 'Backspace':
                if (isEditing) {
                    e.preventDefault();
                    // 将当前字节设为0
                    fileData[editingIndex] = 0;
                    updateByteDisplay(editingIndex);
                }
                break;
        }
    }

    // 处理键盘输入事件
    function handleKeypress(e: JQuery.KeyPressEvent): void {
        if (!fileData || !isEditing) return;
        
        const char = e.key.toUpperCase();
        const hexValue = parseInt(char, 16);
        
        if (isNaN(hexValue)) return;
        
        e.preventDefault();
        
        const currentByte = fileData[editingIndex];
        let newByte: number;
        
        if (editingNibble === 0) {
            // 编辑高位
            newByte = (hexValue << 4) | (currentByte & 0x0F);
            editingNibble = 1;
            fileData[editingIndex] = newByte;
            updateByteDisplay(editingIndex);
        } else {
            // 编辑低位
            newByte = (currentByte & 0xF0) | hexValue;
            fileData[editingIndex] = newByte;
            updateByteDisplay(editingIndex);
            
            // 完成当前字节的编辑，移动到下一个字节
            editingNibble = 0;
            if (editingIndex + 1 < fileData.length) {
                startEditing(editingIndex + 1);
            } else {
                stopEditing();
            }
        }
    }

    // 处理方向键
    function handleArrowKeys(e: JQuery.KeyDownEvent): void {
        if (!fileData) return;
        
        e.preventDefault();
        
        let newIndex = isEditing ? editingIndex : (selectionStart >= 0 ? selectionStart : 0);
        
        switch (e.key) {
            case 'ArrowLeft':
                newIndex = Math.max(0, newIndex - 1);
                break;
            case 'ArrowRight':
                newIndex = Math.min(fileData.length - 1, newIndex + 1);
                break;
            case 'ArrowUp':
                newIndex = Math.max(0, newIndex - bytesPerRow);
                break;
            case 'ArrowDown':
                newIndex = Math.min(fileData.length - 1, newIndex + bytesPerRow);
                break;
        }
        
        // 确保新位置可见
        const newRow = Math.floor(newIndex / bytesPerRow);
        if (newRow < viewStartRow || newRow >= viewStartRow + viewMaxRows) {
            scrollbar.scrollToRow(Math.max(0, newRow - Math.floor(viewMaxRows / 2)));
        }
        
        if (isEditing) {
            startEditing(newIndex);
        } else {
            selectionStart = newIndex;
            selectionEnd = newIndex;
            updateSelection();
        }
    }

    // 更新单个字节的显示
    function updateByteDisplay(index: number): void {
        if (!fileData) return;
        
        const byte = fileData[index];
        const hex = byte.toString(16).padStart(2, '0').toUpperCase();
        const ascii = (byte >= 32 && byte <= 126) ? String.fromCharCode(byte) : '.';
        
        // 更新十六进制显示
        const $hexByte = $(`.hex-byte[data-index="${index}"]`);
        $hexByte.text(hex);
        
        // 如果正在编辑这个字节，恢复编辑状态
        if (isEditing && editingIndex === index) {
            $hexByte.addClass('editing');
            // 添加光标位置提示
            if (editingNibble === 0) {
                $hexByte.attr('data-cursor', 'high');
            } else {
                $hexByte.attr('data-cursor', 'low');
            }
        }
        
        // 更新ASCII显示 - 只更新对应的ASCII字符，不重新渲染整个视图
        updateAsciiDisplay(index, ascii);
    }

    // 更新单个字节对应的ASCII显示
    function updateAsciiDisplay(index: number, ascii: string): void {
        // 计算在当前视图中的行和列
        const row = Math.floor(index / bytesPerRow);
        const col = index % bytesPerRow;
        
        // 如果不在当前可见范围内，忽略
        if (row < viewStartRow || row >= viewStartRow + viewMaxRows) {
            return;
        }
        
        const visibleRow = row - viewStartRow;
        const asciiColumn = $('#ascii-column');
        const rowDiv = asciiColumn.children().eq(visibleRow);
        
        if (rowDiv.length) {
            const currentText = rowDiv.text();
            // 替换对应位置的字符
            const newText = currentText.substring(0, col) + ascii + currentText.substring(col + 1);
            rowDiv.text(newText);
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
        // xldebug
        reader.onload = function(e: ProgressEvent<FileReader>) {
            const arrayBuffer = e.target?.result as ArrayBuffer;
            fileData = new Uint8Array(arrayBuffer);
            
            // 重新计算可见行数（可能窗口大小改变了）
            const hexContent = $('#hex-content');
            const hexContentHeight = hexContent.height();
            if (hexContentHeight) {
                viewMaxRows = Math.floor(hexContentHeight / 30);
            }
            
            initView();
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
        
        const blob = new Blob([fileData!.buffer], { type: 'application/octet-stream' });
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
            // 如果正在编辑模式，不要干扰编辑
            if (isEditing) return;
            
            console.log("左键点击");
            isSelecting = true;
            const index = parseInt($(this).data('index'));
            selectionStart = index;
            selectionEnd = index;
            updateSelection();
        }
    });

    // 双击开始编辑
    $('.hex-column').on('dblclick', '.hex-byte', function(e: JQuery.DoubleClickEvent) {
        e.preventDefault();
        const index = parseInt($(this).data('index'));
        startEditing(index);
    });

    $(document).on('mousemove', function(e: JQuery.MouseMoveEvent) {
        if (!isSelecting || isEditing) return; // 编辑模式时不处理鼠标移动
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
        
        // 如果正在编辑，不要清除编辑样式
        if (isEditing) {
            $('.hex-byte').not('.editing').removeClass('selected');
        } else {
            $('.hex-byte').removeClass('selected');
        }
        
        if (selectionStart === -1 || selectionEnd === -1) return;
        
        const start = Math.min(selectionStart, selectionEnd);
        const end = Math.max(selectionStart, selectionEnd);
        
        for (let i = start; i <= end; i++) {
            // 不要给正在编辑的字节添加选择样式
            if (isEditing && i === editingIndex) continue;
            $(`.hex-byte[data-index="${i}"]`).addClass('selected');
        }
        
        const length = end - start + 1;
        $('#selection-info').text(`选择: ${length} 字节 (0x${start.toString(16).toUpperCase()} - 0x${end.toString(16).toUpperCase()})`);
    }

    // 右键菜单
    $(document).on('contextmenu', '.hex-byte', function(e: JQuery.ContextMenuEvent) {
        e.preventDefault();
        
        // 如果正在编辑，不显示右键菜单
        if (isEditing) return;
        
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
        // 如果点击的不是hex-byte，并且不是菜单相关，则停止编辑
        if (isEditing && !$(e.target).closest('.hex-byte').length && 
            !$(e.target).closest('.context-menu').length) {
            stopEditing();
        }
        
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