#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "user/user.h"

int checkNumber(char *str)
{
    int res = 1;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            res = 0;
            break;
        }
    }
    return res;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || (strcmp(argv[1], "--help") == 0)))
    {
        printf("Usage: setpriority pid newpriority\n");
        return -1;
    }
    else if (argc < 3)
    {
        printf("Incorrect Usage!\n");
        printf("Missing arguments!\n");
        printf("Use \'-h\' or \'--help\' for more information\n");
        return -1;
    }
    else if (argc > 3)
    {
        printf("Incorrect Usage!\n");
        printf("Extra arguments!\n");
        printf("Use \'-h\' or \'--help\' for more information\n");
        return -1;
    }
    else if (checkNumber(argv[1]) && checkNumber(argv[2]))
    {
        if (atoi(argv[2]) < 0 || atoi(argv[2]) > 100)
        {
            printf("Invalid priority value.\n");
        }
        else if (set_priority(atoi(argv[1]), atoi(argv[2])) == -1)
        {
            printf("No such process!\n");
        }
    }
    else
    {
        printf("Incorrect Usage!\n");
        printf("Invalid arguments!\n");
        printf("Use \'-h\' or \'--help\' for more information\n");
        return -1;
    }
    return 0;
}