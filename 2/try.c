#include<stdio.h>
#include<unistd.h>

int main()
{
    char *a = getlogin();
    printf("%s\n", a);
}