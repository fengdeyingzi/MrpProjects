

# 头文件目录
HEADER_DIR = src

# 源文件列表
SRCS = 2048.c game_menu.c \
	$(HEADER_DIR)/base.c \
	$(HEADER_DIR)/mpc.c \
	$(HEADER_DIR)/mrc_base.c \
	$(HEADER_DIR)/mrc_win.c \
	$(HEADER_DIR)/mrc_android.c \
	$(HEADER_DIR)/mrc_graphics.c \
	$(HEADER_DIR)/xl_debug.c \
	$(HEADER_DIR)/uc3_font.c \
	$(HEADER_DIR)/xl_bmp.c \
	$(HEADER_DIR)/encode.c \
	$(HEADER_DIR)/xl_coding.c

LIFE_SRCS = life.c \
	$(HEADER_DIR)/base.c \
	$(HEADER_DIR)/mrc_base.c \
	$(HEADER_DIR)/mrc_win.c \
	$(HEADER_DIR)/mrc_android.c \
	$(HEADER_DIR)/mrc_graphics.c \
	$(HEADER_DIR)/xl_debug.c \
	$(HEADER_DIR)/uc3_font.c \
	$(HEADER_DIR)/xl_bmp.c \
	$(HEADER_DIR)/encode.c \
	$(HEADER_DIR)/xl_coding.c

# 编译器设置
CC = arm-none-eabi-gcc
CFLAGS = -nostdlib -nostartfiles -pie -fPIC -marm -march=armv5te --entry=_start
CFLAGS += -I$(HEADER_DIR) -I. -Isrc

.PHONY: demo
demo:
	$(CC) -o bin.elf $(SRCS) $(CFLAGS) -lgcc -ffixed-r9

.PHONY: life
life:
	$(CC) -o bin.elf $(LIFE_SRCS) $(CFLAGS) -lgcc -ffixed-r9

.PHONY: clean
clean:
	rm -f bin.elf *.o

