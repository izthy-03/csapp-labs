/*
 * Cachelab Part A
 * ==============================
 * StudentID: 521021911101
 * StudentName: 江天航
 * ==============================
 */

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "cachelab.h"

#define IS_BLANK(s) (*(s) == ' ' || *(s) == '\t')
#define IS_END(s) (*(s) == '\0')

#define SKIP_BLANK(s)                     \
    do                                    \
    {                                     \
        while (!IS_END(s) && IS_BLANK(s)) \
            (s)++;                        \
    } while (0);

#define ver_print(_s, _a...)       \
    do                             \
    {                              \
        if (verbose)               \
            printf(_s "\n", ##_a); \
    } while (0);

/* define cache struct */
typedef char byte_t;

/* rank lines from 1~E by LRU */
struct line_t
{
    unsigned long tag;
    byte_t valid;
    int LRU;
    byte_t *block;
};

struct cache_t
{
    /* define cache (S,E,B,m) */
    int S, s, E, B, b, C, t;
    struct line_t **line;
} cache;

/* declare some globals */
extern char *optarg;
int verbose = 0;
char filepath[] = "./";
FILE *fp;
unsigned long t_mask = 0, s_mask = 0, b_mask = 0;
int hitcnt = 0, misscnt = 0, evictcnt = 0;

/* parse the arguments with getopt() */
void parseLine(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
        exit(0);
    }

    int ch;
    while ((ch = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        // printf("argument: %s\n", optarg);
        switch (ch)
        {
        case 'h':
            printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
            printf("Options:\n");
            break;

        case 'v':
            verbose = 1;
            ver_print("==verbose mode on==");
            break;

        case 's':
            cache.s = strtol(optarg, NULL, 0);
            cache.S = (int)pow(2, cache.s);
            break;

        case 'E':
            cache.E = strtol(optarg, NULL, 0);
            break;

        case 'b':
            cache.b = strtol(optarg, NULL, 0);
            cache.B = (int)pow(2, cache.b);
            break;

        case 't':
            strcat(filepath, optarg);
            ver_print("reading trace file from %s", filepath);
            if (!(fp = fopen(filepath, "r")))
            {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                exit(0);
            }
            break;

        default:
            break;
        }
    }
}

/* allocate memory for cache */
void initCache()
{
    cache.C = cache.S * cache.E * cache.B;
    cache.t = 64 - cache.s - cache.b;
    // ver_print("Cache set: %d", cache.S);
    // ver_print("Cache lines-per-set: %d", cache.E);
    // ver_print("Cache block size: %d", cache.B);
    // ver_print("Cache capacity: %d", cache.C);

    if ((cache.line = (struct line_t **)malloc(cache.S * sizeof(struct line_t *))) == NULL)
    {
        fprintf(stderr, "oversized cache\n");
        exit(0);
    }
    for (int i = 0; i < cache.S; i++)
    {
        if ((cache.line[i] = (struct line_t *)malloc(cache.E * sizeof(struct line_t))) == NULL)
        {
            fprintf(stderr, "oversized cache\n");
            exit(0);
        }
    }
    for (int i = 0; i < cache.S; i++)
        for (int j = 0; j < cache.E; j++)
        {
            /* fake allocate for each block */
            // if ((cache.line[i][j].block = (byte_t *)malloc(cache.B * sizeof(byte_t))) == NULL)
            // {
            //     printf("oversized cache\n");
            //     exit(0);
            // }
            cache.line[i][j].valid = 0;
            cache.line[i][j].LRU = 0;
        }
}

/* generate t,s,b masks */
void generateMask()
{
    int i;
    for (i = 0; i < cache.b; i++)
        b_mask = b_mask << 1 | 1;
    for (i = 0; i < cache.s; i++)
        s_mask = s_mask << 1 | 1;
    s_mask <<= cache.b;
    t_mask = ~(s_mask | b_mask);
}

/* visit and update cache */
void visitCache(char *bufp)
{
    /* get visiting mode - Load, Store, Modify */
    SKIP_BLANK(bufp);
    char mode = *bufp++;
    SKIP_BLANK(bufp);
    /* get tag, set, and byte offset */
    unsigned long addr = strtoull(bufp, NULL, 16);
    unsigned long tag = (addr & t_mask) >> (cache.b + cache.s);
    unsigned long set = (addr & s_mask) >> cache.b;
    unsigned long offset = addr & b_mask;
    ver_print("%c 0x%lx, tag = 0x%lx, set = 0x%lx, offset = 0x%lx", mode, addr, tag, set, offset);

    int LRUindex[cache.E + 1], hit = -1;
    memset(LRUindex, 0, sizeof(LRUindex));
    LRUindex[0] = -1;
    for (int j = 0; j < cache.E; j++)
    {
        LRUindex[cache.line[set][j].LRU] = j;
        if (cache.line[set][j].valid && cache.line[set][j].tag == tag)
            hit = j;
    }
    /* update LRU rank */
    if (hit >= 0)
    {
        hitcnt += (mode == 'M') ? 2 : 1;
        ver_print("hit");

        int hitLRU = cache.line[set][hit].LRU;
        /* promote the visited line */
        cache.line[set][hit].LRU = 1;
        /* add LRU rank of lines before the hit line */
        for (int k = hitLRU - 1; k > 0; k--)
        {
            int update = LRUindex[k];
            cache.line[set][update].LRU++;
        }
    }
    else
    {
        misscnt++;
        ver_print("miss");
        hitcnt += (mode == 'M') ? 1 : 0;
        int selectline;
        /* there exists a cold line */
        if (LRUindex[0] >= 0)
        {
            selectline = LRUindex[0];
            for (int k = 0; k < cache.E; k++)
            {
                if (cache.line[set][k].valid)
                    cache.line[set][k].LRU++;
            }
            cache.line[set][selectline].valid = 1;
            cache.line[set][selectline].LRU = 1;
            cache.line[set][selectline].tag = tag;
        }
        /* cache full. Select the LRU line(rank=E) as victim */
        else
        {
            ver_print("evict");
            evictcnt++;
            selectline = LRUindex[cache.E];
            for (int k = 0; k < cache.E; k++)
                cache.line[set][k].LRU++;
            cache.line[set][selectline].LRU = 1;
            cache.line[set][selectline].tag = tag;
        }
    }
}

int main(int argc, char *argv[])
{
    parseLine(argc, argv);
    initCache();
    generateMask();

    char buf[64], *bufp;
    /* read trace file */
    while ((bufp = fgets(buf, 64, fp)) != NULL)
    {
        // printf("%s", bufp);
        if (*bufp == 'I')
            continue;
        visitCache(bufp);
    }
    printSummary(hitcnt, misscnt, evictcnt);
    fclose(fp);
    return 0;
}
