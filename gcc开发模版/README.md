# 使用gcc开发mrp

## 文件介绍

mrp.go 是一个go语言编写的mrp打包工具，主要用来打包资源文件

demo 文件夹：存放的是gcc编写的mrp源码，运行make生成bin.elf

lib 文件夹：存放mrp必需的启动文件start.mr和cfunction.ext

loader 文件夹：用来生成cfunction.ext，需要用斯凯的sdk进行编译，然后用工具把生成的elfloader.mrp中的cfunction.ext提取出来放进上面那个lib文件夹

start.mr 文件夹：用来生成start.mr，里面包含斯凯的编译器，如果不需要特别定制start.mr则直接使用elfloader.mrp内的start.mr即可

## 编译mrp

需要安装 arm-none-eabi-gcc ，我这里用的是"gcc version 9.3.1 20200408 (release) (GNU Arm Embedded Toolchain 9-2020-q2-update)"

1. 在demo文件夹里面输入make命令，生成bin.elf

2. 编辑pack.json
```
{
    "display": "测试gcc编译", // 显示名
    "filename": "./demo/demo.mrp",  // 生成的mrp文件
    "appid": 1,
    "version": 1,
    "vendor": "vendor",
    "description": "desc",
    "files": [  // 按这里的顺序打包成mrp
        "./lib/start.mr",
        "./demo/bin.elf",  // gcc 编译出来的可执行文件
        "./demo/test.bmp",
        "./lib/cfunction.ext"
    ]
}
```
3. 运行mrp.exe

需要注意的是目前还不能自动对bmp进行处理，因此这种特殊的bmp需要用其它工具生成

你也可以使用其它mrp打包工具，只需要将bin.elf、start.mr、cfunction.ext和你的其它需要打包的资源文件打包到一起就可以了

# License

GNU General Public License v3.0
