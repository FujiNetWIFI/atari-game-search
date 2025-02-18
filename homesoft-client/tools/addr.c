#include <atari.h>
#include <stdio.h>

static char title[] = "\x00\x00homesoft\x00\x00search\x00\x00";
static char query[] = {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

char results[128][128];
unsigned char num_results=0;

static void _dlist =
    {
        DL_DLI(DL_BLK8),
        DL_BLK8,
        DL_BLK8,
        DL_LMS(DL_CHR20x16x2),
        &title,
        DL_LMS(DL_CHR40x8x1),
        &results[0],
        DL_LMS(DL_CHR40x8x1),
        &results[1],
        DL_LMS(DL_CHR40x8x1),
        &results[2],
        DL_LMS(DL_CHR40x8x1),
        &results[3],
        DL_LMS(DL_CHR40x8x1),
        &results[4],
        DL_LMS(DL_CHR40x8x1),
        &results[5],
        DL_LMS(DL_CHR40x8x1),
        &results[6],
        DL_LMS(DL_CHR40x8x1),
        &results[7],
        DL_LMS(DL_CHR40x8x1),
        &results[8],
        DL_LMS(DL_CHR40x8x1),
        &results[9],
        DL_LMS(DL_CHR40x8x1),
        &results[10],
        DL_LMS(DL_CHR40x8x1),
        &results[11],
        DL_LMS(DL_CHR40x8x1),
        &results[12],
        DL_LMS(DL_CHR40x8x1),
        &results[13],
        DL_LMS(DL_CHR40x8x1),
        &results[14],
        DL_LMS(DL_CHR40x8x1),
        &results[15],
        DL_LMS(DL_CHR40x8x1),
        &results[16],
        DL_LMS(DL_CHR40x8x1),
        &results[17],
        DL_LMS(DL_CHR40x8x1),
        &results[18],
        DL_DLI(DL_LMS(DL_CHR40x8x1)),
        &results[19],
        DL_BLK3,
        DL_LMS(DL_CHR20x8x2),
        &query,
        DL_CHR20x8x2,
	DL_BLK5,
        DL_JVB,
        &_dlist
    };

struct dlist_struct
{
    unsigned char blk1;
    unsigned char blk2;
    unsigned char blk3;
    struct title_area
    {
        unsigned char inst;
        unsigned short addr;
    } title;
    struct scroll_area
    {
        unsigned char inst;
        unsigned short addr;
    } line[20];
    unsigned char blk4;
    struct query_area
    {
        unsigned char inst;
        unsigned short addr;
        unsigned char inst2;
    } query;
    unsigned char blk5;
    struct jvb
    {
        unsigned char inst;
        unsigned short addr;
    } jvb;
};

void main(void)
{
    struct dlist_struct *p = &_dlist;
    int i=0;

    for(i=0;i<20;i++)
        printf("Line %02u addr %02x\n",i,p->line[i].addr);
    
    while(1);
}

