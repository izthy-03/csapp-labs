#include <iostream>
#include <cstring>
using namespace std;
#define IS_DIGIT(s) ((*(s) >= '0' && *(s) <= '9') || *(s) == '-' || *(s) == '+')
#define IS_LETTER(s) ((*(s) >= 'a' && *(s) <= 'z') || (*(s) >= 'A' && *(s) <= 'Z'))
#define IS_COMMENT(s) (*(s) == '#')
#define IS_REG(s) (*(s) == '%')
#define IS_IMM(s) (*(s) == '$')

#define IS_BLANK(s) (*(s) == ' ' || *(s) == '\t')
#define IS_END(s) (*(s) == '\0')

#define HPACK(hi, lo) ((((hi)&0xF) << 4) | ((lo)&0xF))
#define HIGH(pack) ((pack) >> 4 & 0xF)
#define LOW(pack) ((pack)&0xF)

#define SKIP_BLANK(s)                     \
    do                                    \
    {                                     \
        while (!IS_END(s) && IS_BLANK(s)) \
            (s)++;                        \
    } while (0);

void test(char **ptr, char **name)
{
    *name = (char *)malloc(100);
    int len = 0;
    while (IS_DIGIT(*ptr + len))
        len++;
    strncpy(*name, *ptr, len);
    cout << (*ptr)++ << endl;
    cout << *ptr << endl;
    // cout << *name << endl;
}
void test_hang(char **ptr)
{
    SKIP_BLANK(*ptr);
    (*ptr)++;
}
void test_set()
{
    char bytes[10];
    memset(bytes, 0, sizeof(bytes));
    char *p = bytes;
    *p = 'a';
    p++;
    *p = 'b';
    cout << bytes << endl;
    cout << p << endl;
}
void little_end_hex(unsigned char *dest, long long value, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        char h1 = value & 0xF;
        char h2 = (value >> 4) & 0xF;
        cout << h2 << " " << h1 << endl;
        dest[i] = HPACK(h2, h1);
        value >>= 8;
    }
}
union conv
{
    unsigned char a[8];
    long long x;
};
typedef unsigned char byte_t;
typedef struct bin
{
    int64_t addr;
    byte_t codes[10];
    int bytes;
} bin_t;
void test_strtol(char *ptr)
{
    long value = strtoul(ptr, NULL, 0);
    printf("0x%lx\n", value);
}
int main()
{
    char *s, *b;
    char ss[] = "0xfff1342809062Cac";
    s = (char *)malloc(100);
    memset(s, 0, sizeof(s));
    strcpy(s, ss);
    char *res;
    b = s;
    test_strtol(s);
    return 0;
}