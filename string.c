#include <stdint.h>

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

int strlen(const char* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void* memset(void* dest, int value, int count) {
    unsigned char* ptr = (unsigned char*)dest;
    while (count--) {
        *ptr++ = (unsigned char)value;
    }
    return dest;
}

void itoa(int value, char* str) {
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value > 0) {
        str[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // reverse
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}

void* memcpy(void* dest, const void* src, int count) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    for (int i = 0; i < count; i++)
        d[i] = s[i];
    return dest;
}

/*void* memset(void* dest, int value, int count) {
    unsigned char* d = dest;
    for (int i = 0; i < count; i++)
        d[i] = (unsigned char)value;
    return dest;
}
*/

