# JS解释器

## 简介

这是一个功能机平台上运行的 JavaScript 解释器，封装了功能机的 C 语言方法，实现了完整的图形、文件、字体等系统 API。

---

## 全局变量定义
SCRW // 屏幕宽度
SCRH // 屏幕高度

## 一、图形绘制 API (mrc_graphics)

### 1.1 基础图形绘制

#### 清屏 对应gl_clearScreen
```javascript
clearScreen(r, g, b)
```
参数：
- `r`: 红色分量 (0-255)
- `g`: 绿色分量 (0-255)
- `b`: 蓝色分量 (0-255)

#### 画实心矩形 对应gl_drawRect
```javascript
drawRect(x, y, w, h, color)
```
参数：
- `x, y`: 左上角坐标
- `w, h`: 宽度和高度
- `color`: ARGB颜色值 (如: 0xFFFF0000 表示红色)

#### 画空心矩形 对应gl_drawHollowRect
```javascript
drawHollowRect(x, y, width, height, color)
```

#### 画线段 对应gl_drawLine
```javascript
drawLine(x1, y1, x2, y2, color)
```
参数：
- `x1, y1`: 起点坐标
- `x2, y2`: 终点坐标

#### 画实心圆 对应gl_drawCir
```javascript
drawCir(x, y, r, color)
```
参数：
- `x, y`: 圆心坐标
- `r`: 半径

#### 画空心圆 对应gl_drawHollowCir
```javascript
drawHollowCir(x, y, r, color)
```

#### 画实心三角形 对应gl_drawTriangle
```javascript
drawTriangle(x1, y1, x2, y2, x3, y3, color)
```
参数：三个顶点的坐标

#### 画空心三角形 对应gl_drawHollowTriangle
```javascript
drawHollowTriangle(x1, y1, x2, y2, x3, y3, color)
```

#### 画多边形(填充) 对应gl_drawPolygon
```javascript
drawPolygon(points, count, color)
```
参数：
- `points`: 顶点坐标数组
- `count`: 顶点数量

#### 画空心多边形 对应gl_drawHollowPolygon
```javascript
drawHollowPolygon(points, count, color)
```

#### 画旋转矩形(实心) 对应gl_drawRotatedRect
```javascript
drawRotatedRect(centerX, centerY, width, height, bx, by, angle, color)
```
参数：
- `centerX, centerY`: 中心点坐标
- `width, height`: 矩形宽高
- `bx, by`: 旋转基点
- `angle`: 旋转角度(弧度)

#### 画旋转矩形(空心) 对应gl_drawRotatedHollowRect
```javascript
drawRotatedHollowRect(centerX, centerY, width, height, bx, by, angle, color)
```

#### 画渐变矩形 对应gl_drawShadeRect
```javascript
drawShadeRect(x, y, w, h, colorA, colorB, mode)
```
参数：
- `mode`: 渐变方向
  - `0`: SHADE_UPDOWN (从上到下)
  - `1`: SHADE_LEFTRIGHT (从左到右)
  - `2`: SHADE_DOWNUP (从下到上)
  - `3`: SHADE_RIGHTLEFT (从右到左)


### 刷新屏幕 对应mrc_refreshScreen
```javascript
ref(x, y, w, h)
```


---

### 1.2 位图 (Bitmap) 操作

#### 从资源文件读取位图 对应C语言函数readBitmap565FromAssets
```javascript
var bmp = readBitmapFromAssets("fish.png")
```

#### 从内存卡读取位图
```javascript
var bmp = readBitmap("path/to/image.png")
```

#### 创建空白位图，对应C语言函数：createBitmap565
```javascript
var bmp = createBitmap(width, height)
```

#### 绘制位图
```javascript
drawBitmap(bmp, x, y)
```


#### 绘制位图指定区域
```javascript
drawBitmapFlip(bmp, x, y, w, h, sx, sy)
```
参数：
- `sx, sy`: 源图像起始坐标
- `w, h`: 要绘制的区域宽高

#### 绘制位图区域(带变换和锚点)
```javascript
drawBitmapRegion(src, x_src, y_src, width, height, transform, x_dest, y_dest, anchor)
```
参数：
- `x_src, y_src`: 源图像区域左上角坐标
- `width, height`: 区域宽高
- `transform`: 变换类型
- `x_dest, y_dest`: 目标位置坐标
- `anchor`: 锚点 (3=LEFT, 48=TOP, 5=RIGHT, 17=CENTER, 80=BOTTOM)

#### 扩展绘制位图
```javascript
drawBitmapEx(bmp, x, y, w, h, tx, ty, tw, th)
```
参数：
- `x, y, w, h`: 目标绘制区域
- `tx, ty, tw, th`: 源图像裁剪区域


#### 缩放位图(创建新位图)
```javascript
var newBmp = createBitmapFromBitmap(bmp, width, height)
```

#### 保存位图到文件
```javascript
saveBitmap(bmp, "filename.bmp")
```

#### 释放位图
```javascript
bitmapFree(bmp)
```

---



## 二、字体绘制 API (uc3_font)

### 2.2 文字绘制

#### 绘制文字 对应mrc_drawText
```javascript
drawText(text, x, y, r, g, b)
```


#### 获取文字宽度 对应mrc_textWidthHeight
```javascript
var width = getWidth(text)
```
返回文字的像素宽度，用于布局计算


### 获取文字高度 对应mrc_getFontHeight
```javascript
var height = getFontHeight()
```

---

## 三、文件操作 API (fopen)

### 3.1 文件基本操作

#### 打开文件
```javascript
var file = fopen(path, mode)
```
参数：
- `path`: 文件路径
- `mode`: 打开模式
  - `"r"`: 只读
  - `"w"`: 写入(覆盖)
  - `"a"`: 追加
  - `"rb"`: 二进制读
  - `"wb"`: 二进制写

#### 读取文件
```javascript
var bytesRead = fread(buffer, size, count, file)
```
参数：
- `buffer`: 读取到的缓冲区
- `size`: 每个元素的大小
- `count`: 元素数量
- 返回：实际读取的元素数量

#### 写入文件
```javascript
var bytesWritten = fwrite(buffer, size, count, file)
```

#### 移动文件指针
```javascript
fseek(file, offset, mode)
```
参数：
- `offset`: 偏移量
- `mode`: 起始位置
  - `0`: SEEK_SET (文件开头)
  - `1`: SEEK_CUR (当前位置)
  - `2`: SEEK_END (文件末尾)

#### 刷新文件缓冲
```javascript
fflush(file)
```

#### 关闭文件
```javascript
fclose(file)
```

---

## 四、定时器 API (mrc_timer)

### 4.1 定时器管理

#### 创建定时器
```javascript
var timer = timerCreate()
```
返回：
- 定时器ID（用于后续操作）

#### 启动定时器
```javascript
timerStart(timerId, interval, callbackName, loop)
```
参数：
- `timerId`: 定时器ID
- `interval`: 时间间隔（毫秒）
- `callbackName`: 回调函数名称（字符串）
- `loop`: 是否循环 (0: 不循环, 1: 循环)

**注意**：回调函数必须是全局函数，通过函数名字符串传递。

示例：
```javascript
// 定义全局回调函数
function gameLoop(timerId) {
    // 更新游戏逻辑
    updateGame();
    drawGame();
    ref(0, 0, SCRW, SCRH);
}

// 创建并启动定时器（30fps）
var timer = timerCreate();
timerStart(timer, 33, "gameLoop", 1);  // 循环执行
```

#### 停止定时器
```javascript
timerStop(timerId)
```
参数：
- `timerId`: 要停止的定时器ID

#### 删除定时器
```javascript
timerDelete(timerId)
```
参数：
- `timerId`: 要删除的定时器ID

**注意**：退出程序前必须停止并删除所有定时器！

---

## 五、资源读取 API (mrc_android)

### 5.1 Assets 资源访问

#### 从 assets 目录读取文件
```javascript
var data = readFileFromAssets(filename)
```
参数：
- `filename`: assets 目录下的文件名
- 返回：文件内容

#### 释放 assets 文件
```javascript
freeFileFromAssets(data, len)
```


---

## 六、常用颜色值

### 6.1 ARGB 颜色格式

使用 32 位 ARGB 格式：`0xAARRGGBB`

```javascript
0xFFFF0000  // 红色
0xFF00FF00  // 绿色
0xFF0000FF  // 蓝色
0xFFFFFF00  // 黄色
0xFFFF00FF  // 洋红
0xFF00FFFF  // 青色
0xFFFFFFFF  // 白色
0xFF000000  // 黑色
0x80000000  // 半透明黑色 (alpha=128)
```

---

## 七、完整示例

### 7.1 绘制游戏界面
```javascript

// 清屏
clearScreen(200, 200, 200);

// 绘制背景
var bg = readBitmapFromAssets("bg.png");
drawBitmap(bg, 0, 0);

// 绘制文字
uc3_drawText("得分: 100", 10, 10, 255, 255, 255, 0);

// 绘制UI元素
gl_drawRect(10, 50, 100, 30, 0xFF3366CC);
gl_drawHollowRect(10, 90, 100, 30, 0xFF000000);
gl_drawCir(60, 140, 20, 0xFFFF0000);

// 刷新屏幕
ref(0, 0, SCRW, SCRH);

// 释放资源
bitmapFree(bg);
```

### 7.2 文件读写示例
```javascript
// 写入文件
var file = fopen("save.dat", "wb");
fwrite(data, 1, dataSize, file);
fclose(file);

// 读取文件
file = fopen("save.dat", "rb");
var buffer = new Array(dataSize);
fread(buffer, 1, dataSize, file);
fclose(file);
```

### 7.3 加载和显示图片
```javascript
// 从 assets 加载
var sprite = readBitmapFromAssets("player.png");
drawBitmap(sprite, 100, 100);

// 带透明度绘制
drawBitmapAlpha(sprite, 100, 100, 128);

// 旋转绘制
drawBitmapRotate(sprite, 120, 120, 10, 10, 45);

// 刷新屏幕
ref(0, 0, SCRW, SCRH);

// 释放
bitmapFree(sprite);
```

### 7.4 定时器游戏循环示例
```javascript
// 游戏状态
var score = 0;
var playerX = 100;
var playerY = 100;

// 初始化函数
function initGame() {
    score = 0;
    playerX = SCRW / 2;
    playerY = SCRH / 2;
}

// 更新游戏逻辑
function updateGame() {
    // 更新游戏状态
    score++;
}

// 绘制游戏画面
function drawGame() {
    clearScreen(200, 200, 200);

    // 绘制玩家
    drawCir(playerX, playerY, 10, 0xFFFF0000);

    // 绘制分数
    drawText("Score: " + score, 10, 10, 0, 0, 0);

    // 刷新屏幕
    ref(0, 0, SCRW, SCRH);
}

// 定时器回调（30fps游戏循环）
function gameLoop(timerId) {
    updateGame();
    drawGame();
}

// 启动游戏
initGame();
var gameTimer = timerCreate();
timerStart(gameTimer, 33, "gameLoop", 1);  // 33ms ≈ 30fps

// 游戏结束时清理
function exitGame() {
    timerStop(gameTimer);
    timerDelete(gameTimer);
}
```

---

## 八、注意事项

1. **颜色格式**：图形函数使用 ARGB 格式 (`0xAARRGGBB`)，字体函数使用 RGB 分量
2. **资源管理**：使用完位图后必须调用 `bitmapFree()` 释放内存
3. **屏幕刷新**：绘制完成后必须调用 `ref()` 才能显示
4. **文件操作**：使用完文件后必须调用 `fclose()` 关闭
5. **定时器管理**：
   - 定时器回调函数必须是全局函数，通过函数名字符串传递
   - 退出程序前必须停止并删除所有定时器
   - 支持最多 16 个同时运行的定时器
   - 游戏主循环建议使用 30fps（33ms）或 60fps（16ms）

---

## 九、参考文档

- [功能机开发规范](功能机开发规范.md)
- [程序编写说明](程序编写说明.md)
