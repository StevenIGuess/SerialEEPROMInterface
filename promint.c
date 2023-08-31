#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
    char str[200] = "promint.py ";
    if (argc >= 4)
    {
        printf("%s", "Too many arguments");
    }
    for (int i = 0; i < argc; i++)
    {
        if(strlen(argv[i]) > 50)
        {
            printf("%s%i%s", "Argument at index [", i, "] is too long");
        }else {
            strncat(str, argv[i], 50);
        }
    }
    return system(str);
}