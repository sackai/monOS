#ifndef STRING_H
#define STRING_H

int strcmp(const char* a, const char* b);
int strlen(const char* s);
void strcpy(char* dest, const char* src);
void* memset(void* dest, int value, int count); 
void itoa(int value, char* buffer);
void* memcpy(void* dest, const void* src, int count);
void* memset(void* dest, int value, int count);


#endif

