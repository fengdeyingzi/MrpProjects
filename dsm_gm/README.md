# 使用gcc开发mrp

## 文件介绍
本工程包含以下文件列表：
    -> bin.elf      gcc编译后的文件
    -> start.mr      启动文件
    -> cfunction.ext   ext启动器
    -> font.uc3      字库文件
    -> logo_240.bma      logo图片


## 编译mrp

需要安装 arm-none-eabi-gcc ，我这里用的是"gcc version 9.3.1 20200408 (release) (GNU Arm Embedded Toolchain 9-2020-q2-update)"

1. 在当前文件夹输入make命令，生成bin.elf

2. 使用mrpbuilder进行打包（参见build.bat文件）
```
mrpbuilder -t pack -i bin.elf  ./lib/start.mr ./lib/cfunction.ext assets/ -o demo.mrp
```
3. 运行mrp.exe

