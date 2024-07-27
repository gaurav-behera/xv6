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
        printf("Usage: getpriority pid\n");
        return -1;
    }
    else if (argc < 2)
    {
        printf("Incorrect Usage!\n");
        printf("Missing arguments!\n");
        printf("Use \'-h\' or \'--help\' for more information\n");
        return -1;
    }
    else if (argc > 2)
    {
        printf("Incorrect Usage!\n");
        printf("Extra arguments!\n");
        printf("Use \'-h\' or \'--help\' for more information\n");
        return -1;
    }
    else if (checkNumber(argv[1]))
    {
        int priority = get_priority(atoi(argv[1]));
        if (priority == -1)
        {
            printf("No such process!\n");
        }
        else
        {
            printf("static: %d\n", priority);
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