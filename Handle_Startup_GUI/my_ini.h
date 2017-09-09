#ifndef MY_INI_H
#define MY_INI_H
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
class my_ini
{
public:
    my_ini(char *file_name);

    int GetIniKeyInt(char *title, char *key);
    int test();
    char *GetIniKeyString(char *title, char *key);
private:
    char *filename;
};

#endif // MY_INI_H
