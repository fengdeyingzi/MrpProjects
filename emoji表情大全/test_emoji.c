#include <stdio.h>
#include <string.h>

/* 判断UTF-8字符长度 */
int getUtf8CharLength(unsigned char first_byte)
{
    if ((first_byte & 0x80) == 0x00) {
        return 1;  /* ASCII字符 */
    } else if ((first_byte & 0xE0) == 0xC0) {
        return 2;  /* 2字节UTF-8 */
    } else if ((first_byte & 0xF0) == 0xE0) {
        return 3;  /* 3字节UTF-8 */
    } else if ((first_byte & 0xF8) == 0xF0) {
        return 4;  /* 4字节UTF-8 */
    }
    return 1;  /* 默认返回1 */
}

/* 获取第n个emoji字符的起始位置 */
int getEmojiCharOffset(const char* emoji_list, int emoji_list_len, int index)
{
    int i;
    int current_index;
    int char_len;

    current_index = 0;
    i = 0;

    while (current_index < index && i < emoji_list_len) {
        char_len = getUtf8CharLength(emoji_list[i]);
        i += char_len;
        current_index++;
    }

    return i;
}

/* 获取第n个emoji字符 */
void getEmojiChar(const char* emoji_list, int emoji_list_len, int index, char* buffer, int buffer_size)
{
    int offset;
    int char_len;
    int i;

    offset = getEmojiCharOffset(emoji_list, emoji_list_len, index);
    if (offset >= emoji_list_len) {
        buffer[0] = '\0';
        return;
    }

    char_len = getUtf8CharLength(emoji_list[offset]);
    if (char_len > buffer_size - 1) {
        char_len = buffer_size - 1;
    }

    for (i = 0; i < char_len; i++) {
        buffer[i] = emoji_list[offset + i];
    }
    buffer[char_len] = '\0';
}

int main()
{
    /* 测试数据：前几个emoji字符 */
    const char* test_emojis = "☀☁☂☃☄★☆☇☈☉☊☋☌☍☎☏☐☑☒☓☔☕☖☗☘☙☚☛☜☝☞☟☠☡☢☣☤☥☦☧☨☩☪☫☬☭☮☯☰☱☲☳☴☵☶☷☸☹☺☻☼☽☾☿♀♁♂♃♄♅♆♇♈♉♊♋♌♍♎♏♐♑♒♓♔♕♖♗♘♙♚♛♜♝♞♟♠♡♢♣♤♥♦♧♨♩♪♫♬♭♮♯♰♱♲♳♴♵♶♷♸♹♺♻♼♽♾♿";
    int len = strlen(test_emojis);
    char emoji_char[10];
    int i;
    int emoji_count = 0;

    printf("Total bytes: %d\n", len);

    /* 统计emoji数量 */
    i = 0;
    while (i < len) {
        int char_len = getUtf8CharLength(test_emojis[i]);
        printf("Emoji %d: offset=%d, len=%d, bytes: ", emoji_count, i, char_len);
        for (int j = 0; j < char_len && i+j < len; j++) {
            printf("%02X ", (unsigned char)test_emojis[i+j]);
        }
        printf("\n");
        i += char_len;
        emoji_count++;
    }

    printf("Total emojis: %d\n", emoji_count);

    /* 测试获取前10个emoji */
    for (i = 0; i < 10 && i < emoji_count; i++) {
        getEmojiChar(test_emojis, len, i, emoji_char, sizeof(emoji_char));
        printf("Emoji %d: ", i);
        for (int j = 0; emoji_char[j] != '\0'; j++) {
            printf("%02X ", (unsigned char)emoji_char[j]);
        }
        printf(" (string: %s)\n", emoji_char);
    }

    return 0;
}