# 功能机计算器 Makefile
# 使用 mrpbuilder 编译器

# 目标文件名
TARGET = calculator

# 源文件
SRCS = main.c ui_system.c calculator.c expression_evaluator.c

# 头文件目录
INCLUDES = -I.

# 编译器选项
CFLAGS = -O2 -Wall -c $(INCLUDES)

# 链接器选项
LDFLAGS = 

# 编译器
CC = armcc

# 链接器
LD = armlink

# 默认目标
all: $(TARGET).mrp

# 编译目标
$(TARGET).mrp: $(SRCS:.c=.o)
	@echo "链接目标文件..."
	$(LD) $(LDFLAGS) -o $(TARGET).mrp $^

# 编译C文件
%.o: %.c
	@echo "编译 $<..."
	$(CC) $(CFLAGS) -o $@ $<

# 清理
clean:
	@echo "清理生成文件..."
	rm -f *.o $(TARGET).mrp

# 伪目标
.PHONY: all clean