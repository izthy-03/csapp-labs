#include <stdio.h>
int func4(int x, int y, int z)
{
    int tmp = z - y;
    unsigned int signbit = tmp;
    signbit >>= 31;
    tmp += (int)signbit;
    tmp >>= 1;
    int tmp2 = tmp + y;
    if (tmp2 > x)
        return func4(x, y, tmp2 - 1) * 2;
    else
    {
        if (tmp2 < x)
            return func4(x, tmp2 + 1, z) * 2 + 1;
        else
            return 0;
    }
}
int main()
{
    int a, b;
    scanf("%d", &a);
    printf("%d\n", func4(a, 0, 14));
    return 0;
}