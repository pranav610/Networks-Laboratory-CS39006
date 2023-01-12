#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define buffsize 256

int main()
{
    char *in;
    size_t insize = 32;
    int lenrem;
    in = (char *)malloc(insize * sizeof(char));

    while (1)
    {
        printf("Enter an expression: \n");
        lenrem = getline(&in, &insize, stdin);
        if (strlen(in) == 3)
            if (in[0] == '-' && in[1] == '1' && in[2] == '\n')
                break;
        printf("%ld\n", insize);
        in[--lenrem] = '\0';
        printf("%s\n", in);
        int exprsize = strlen(in);
        printf("%d\n", exprsize);

        printf("HERE1\n");
        int fill = 0;
        for (int j = 0; j < exprsize; j++)
        {
            if (in[j] == ' ')
                continue;
            else
                in[fill++] = in[j];
        }
        in[fill] = '\0';
        printf("HERE2\n");
        printf("%s\n", in);
        double ans;
        int itr = 0;
        int start = 0;
        char neg = 0;
        char glbop = 0;
        char invalid = 0;

        if (in[itr] == '-')
        {
            neg = 1;
            itr++;
            start = itr;
        }
        if (in[itr] == '+')
        {
            itr++;
            start = itr;
        }
        if (in[itr] == '/' || in[itr] == '*')
            invalid = 1;
        
        printf("HERE3\n");
        printf("%d\n", itr);

        for (; itr < exprsize; itr++)
        {
            if (in[itr] == '-' || in[itr] == '+' || in[itr]=='/' || in[itr]=='*')
            {
                char op = in[itr];
                in[itr] = '\0';
                if (start == itr)
                {
                    invalid = 1;
                    break;
                }
                ans = atof(in + start);
                in[itr] = op;
                glbop = op;
                if (neg)
                    ans = -ans;
                neg = 0;
                itr++;
                start = itr;
                break;
            }
        }
        for (; itr < exprsize; itr++)
        {
            if (in[itr] == '+' || in[itr] == '-' || in[itr] == '*' || in[itr] == '/')
            {
                char op = in[itr];
                in[itr] = '\0';
                if (start == itr)
                {
                    invalid = 1;
                    break;
                }
                double num = atof(in + start);
                in[itr] = op;
                if (glbop == '+')
                    ans += num;
                else
                {
                    if (glbop == '-')
                        ans -= num;
                    else
                    {
                        if (glbop == '*')
                            ans *= num;
                        else
                        {
                            if (glbop == '/')
                            {
                                if (num == 0)
                                {
                                    invalid = 1;
                                    break;
                                }
                                else
                                    ans /= num;
                            }
                        }
                    }
                }
                glbop = op;
                start = itr + 1;
            }
        }
        if (itr == exprsize)
        {
            if (start == itr)
                invalid = 1;
            else
            {
                double num = atof(in + start);
                printf("HERE4\n");
                printf("%lf\n", num);
                if (glbop == '+')
                    ans += num;
                else
                {
                    if (glbop == '-')
                        ans -= num;
                    else
                    {
                        if (glbop == '*')
                            ans *= num;
                        else
                        {
                            if (glbop == '/')
                            {
                                if (num == 0)
                                {
                                    invalid = 1;
                                }
                                else
                                    ans /= num;
                            }
                        }
                    }
                }
                if (glbop == 0)
                    ans = num;
            }
        }
        if (invalid)
        {
            printf("Invalid expression\n");
            continue;
        }
        else
        {
            printf("%lf\n", ans);
        }
    }
}
