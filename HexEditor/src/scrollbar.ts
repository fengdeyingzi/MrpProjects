import $ from 'jquery';

/**
 * 基于行数的滚动条类
 */
export class CustomScrollbar {
  private $scrollbar: JQuery;
  private $buttonUp: JQuery;
  private $buttonDown: JQuery;
  private $track: JQuery;
  private $thumb: JQuery;
  
  private isDragging: boolean = false;
  private maxRows: number = 0; // 文件的最大行数
  private visibleRows: number = 10; // 可见的行数
  private currentRow: number = 0; // 当前滚动到的行号
  private thumbHeight: number = 0;
  private maxThumbTop: number = 0;
  
  // 滚动事件回调
  private onScrollCallback: (currentRow: number) => void = () => {};

  constructor(scrollbarSelector: string) {
    this.$scrollbar = $(scrollbarSelector);
    this.$buttonUp = this.$scrollbar.find('.scrollbar-button-up');
    this.$buttonDown = this.$scrollbar.find('.scrollbar-button-down');
    this.$track = this.$scrollbar.find('.scrollbar-track-vertical');
    this.$thumb = this.$scrollbar.find('.scrollbar-thumb-vertical');
    
    this.initialize();
  }

  /**
   * 初始化滚动条
   */
  private initialize(): void {
    this.updateDimensions();
    this.bindEvents();
  }

  /**
   * 更新滚动条尺寸
   */
  private updateDimensions(): void {
    const trackHeight = this.$track.height() || 0;
    
    // 计算滑块高度：基于可见行数与总行数的比例
    if (this.maxRows > 0) {
      const visibilityRatio = Math.min(1, this.visibleRows / this.maxRows);
      this.thumbHeight = Math.max(20, trackHeight * visibilityRatio);
    } else {
      this.thumbHeight = 20;
    }
    
    this.$thumb.css('height', `${this.thumbHeight}px`);
    this.maxThumbTop = trackHeight - this.thumbHeight;
    
    // 更新滑块位置
    this.updateThumbPosition();
  }

  /**
   * 更新滑块位置
   */
  private updateThumbPosition(): void {
    if (this.maxRows <= this.visibleRows || this.maxThumbTop <= 0) {
      this.$thumb.css('top', '0px');
      return;
    }
    
    const scrollableRows = this.maxRows - this.visibleRows;
    const position = (this.currentRow / scrollableRows) * this.maxThumbTop;
    this.$thumb.css('top', `${position}px`);
  }

  /**
   * 绑定事件处理
   */
  private bindEvents(): void {
    // 向上按钮点击
    this.$buttonUp.on('click', () => this.scrollByRows(-1));
    
    // 向下按钮点击
    this.$buttonDown.on('click', () => this.scrollByRows(1));
    
    // 轨道点击
    this.$track.on('click', (e) => this.handleTrackClick(e));
    
    // 滑块拖动
    this.$thumb.on('mousedown', (e) => this.startDrag(e));
    
    // 鼠标释放和拖动
    $(document).on('mouseup', () => this.stopDrag());
    $(document).on('mousemove', (e) => this.handleDrag(e));
    
    // 滚轮事件
    this.$scrollbar.on('wheel', (e) => this.handleWheel(e.originalEvent as WheelEvent));
  }

  /**
   * 按行数滚动
   */
  private scrollByRows(rowDelta: number): void {
    const newRow = Math.max(0, Math.min(this.maxRows - this.visibleRows, this.currentRow + rowDelta));
    this.setCurrentRow(newRow);
  }

  /**
   * 处理轨道点击
   */
  private handleTrackClick(event: JQuery.MouseEventBase): void {
    const trackOffset = this.$track.offset()?.top || 0;
    const clickPosition = event.pageY - trackOffset;
    const thumbTop = parseInt(this.$thumb.css('top')) || 0;
    
    // 计算点击位置对应的行数
    const scrollStep = Math.max(1, Math.floor(this.visibleRows / 2));
    
    if (clickPosition < thumbTop) {
      this.scrollByRows(-scrollStep); // 向上滚动多行
    } else if (clickPosition > thumbTop + this.thumbHeight) {
      this.scrollByRows(scrollStep); // 向下滚动多行
    }
  }

  /**
   * 开始拖动滑块
   */
  private startDrag(event: JQuery.MouseEventBase): void {
    event.preventDefault();
    this.isDragging = true;
    this.$scrollbar.addClass('dragging');
  }

  /**
   * 停止拖动滑块
   */
  private stopDrag(): void {
    if (this.isDragging) {
      this.isDragging = false;
      this.$scrollbar.removeClass('dragging');
    }
  }

  /**
   * 处理拖动
   */
  private handleDrag(event: JQuery.MouseEventBase): void {
    if (!this.isDragging || this.maxThumbTop <= 0) return;
    
    const trackOffset = this.$track.offset()?.top || 0;
    const mouseY = event.pageY;
    const newTop = Math.max(0, Math.min(this.maxThumbTop, mouseY - trackOffset - (this.thumbHeight / 2)));
    
    // 根据滑块位置计算对应的行号
    const scrollableRows = this.maxRows - this.visibleRows;
    const newRow = Math.round((newTop / this.maxThumbTop) * scrollableRows);
    this.setCurrentRow(newRow);
  }

  /**
   * 处理滚轮事件
   */
  private handleWheel(event: WheelEvent): void {
    event.preventDefault();
    
    const delta = event.deltaY;
    const scrollLines = Math.ceil(Math.abs(delta) / 100); // 每100像素滚动一行
    this.scrollByRows(delta > 0 ? scrollLines : -scrollLines);
  }

  /**
   * 设置当前行号并触发回调
   */
  private setCurrentRow(row: number): void {
    const clampedRow = Math.max(0, Math.min(this.maxRows - this.visibleRows, row));
    if (this.currentRow !== clampedRow) {
      this.currentRow = clampedRow;
      this.updateThumbPosition();
      this.onScrollCallback(this.currentRow);
    }
  }

  // ========== 公共接口方法 ==========

  /**
   * 设置最大行数
   */
  public setMaxRows(maxRows: number): void {
    this.maxRows = Math.max(0, maxRows);
    this.currentRow = Math.min(this.currentRow, Math.max(0, this.maxRows - this.visibleRows));
    this.updateDimensions();
  }

  /**
   * 设置可见行数
   */
  public setVisibleRows(visibleRows: number): void {
    this.visibleRows = Math.max(1, visibleRows);
    this.currentRow = Math.min(this.currentRow, Math.max(0, this.maxRows - this.visibleRows));
    this.updateDimensions();
  }

  /**
   * 获取当前行号
   */
  public getCurrentRow(): number {
    return this.currentRow;
  }

  /**
   * 滚动到指定行
   */
  public scrollToRow(row: number): void {
    this.setCurrentRow(row);
  }

  /**
   * 滚动到顶部
   */
  public scrollToTop(): void {
    this.setCurrentRow(0);
  }

  /**
   * 滚动到底部
   */
  public scrollToBottom(): void {
    this.setCurrentRow(Math.max(0, this.maxRows - this.visibleRows));
  }

  /**
   * 注册滚动事件回调
   */
  public onScroll(callback: (currentRow: number) => void): void {
    this.onScrollCallback = callback;
  }

  /**
   * 获取滚动进度百分比 (0-100)
   */
  public getScrollProgress(): number {
    if (this.maxRows <= this.visibleRows) return 0;
    const scrollableRows = this.maxRows - this.visibleRows;
    return (this.currentRow / scrollableRows) * 100;
  }

  /**
   * 设置滚动进度百分比 (0-100)
   */
  public setScrollProgress(percentage: number): void {
    percentage = Math.max(0, Math.min(100, percentage));
    if (this.maxRows <= this.visibleRows) return;
    
    const scrollableRows = this.maxRows - this.visibleRows;
    const targetRow = Math.round((percentage / 100) * scrollableRows);
    this.setCurrentRow(targetRow);
  }
}