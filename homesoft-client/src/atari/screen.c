/**
 * @brief   Homesoft Client
 * @author  Thomas Cherryhomes
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3, see LICENSE for details
 * @verbose Screen routines
 */

#include <atari.h>
#include <conio.h>
#include <string.h>
#include "fujinet-network.h"
#include "screen.h"
#include "bar.h"
#include "loader.h"
#include "charset.h"

static char title[] = "\x00\x00homesoft\x00\x00search\x00\x00";
static char query[] = {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define RESULTS_MAX 128 
#define RESULTS_SIZE 128 

char results[RESULTS_MAX][RESULTS_SIZE];
unsigned char num_results=0;
unsigned char pos_results=0;
unsigned char pos_pan=0;

#define BAR_TOP_OF_WINDOW 2
#define BAR_BOTTOM_OF_WINDOW 21
#define NUM_LINES 20

#define DS_BASE "N:HTTPS://apps.irata.online/homesoft?query="
#define LO_BASE "N:TNFS://apps.irata.online/Atari_8-bit/Games/Homesoft/"

char ds[256];

extern void dli();
extern void dli2();

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
    } line[NUM_LINES];
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

void fetch(void)
{
    unsigned int bw=0;
    unsigned char c=0, err=0;
    
    network_init();
    network_open(ds,12,0);
    network_status(ds,&bw,&c,&err);
    network_read(ds,(unsigned char *)&results,bw);
    network_close(ds);
}

void count(void)
{
    int i;

    num_results=0;
    
    for (i=0;i<RESULTS_MAX;i++)
    {
        if (results[i][0])
            num_results++;
    }
}

const char splash_1[]={0x00,0x00,0x00,0x34, 0x68, 0x69, 0x73, 0x00, 0x73, 0x65, 0x61, 0x72, 0x63, 0x68, 0x00, 0x74, 0x6f, 0x6f, 0x6c, 0x00, 0x61, 0x6c, 0x6c, 0x6f, 0x77, 0x73, 0x00, 0x79, 0x6f, 0x75, 0x00, 0x74, 0x6f, 0x00, 0x66, 0x69, 0x6e, 0x64,0x00,0x00};

const char splash_2[]={0x00, 0x00, 0x00, 0x61, 0x6e, 0x79, 0x00, 0x67, 0x61, 0x6d, 0x65, 0x00, 0x69, 0x6e, 0x00, 0x74, 0x68, 0x65, 0x00, 0x28, 0x6f, 0x6d, 0x65, 0x73, 0x6f, 0x66, 0x74, 0x00, 0x23, 0x6f, 0x6c, 0x6c, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x00};

const char splash_3[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x6e, 0x74, 0x65, 0x72, 0x00, 0x61, 0x6e, 0x79, 0x00, 0x70, 0x61, 0x72, 0x74, 0x00, 0x6f, 0x66, 0x00, 0x61, 0x00, 0x67, 0x61, 0x6d, 0x65, 0x00, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00};

const char splash_4[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x6f, 0x00, 0x66, 0x69, 0x6e, 0x64, 0x00, 0x6d, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x00, 0x67, 0x61, 0x6d, 0x65, 0x73, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

char ascii_to_screen(char c)
{
    c &= 0x7F;

    if (c<32)
        c+=64;
    else if (c<96)
        c-=32;
        
    return c;
}

char screen_to_ascii(char d)
{
    unsigned char ud = d;
    if (ud >= 64 && ud < 96) {
        return ud - 64;
    } else if (ud < 64) {
        return ud + 32;
    } else {
        return ud;
    }
}

void screen_splash(void)
{
    memcpy(&results[4],splash_1,sizeof(splash_1));
    memcpy(&results[5],splash_2,sizeof(splash_2));
    memcpy(&results[7],splash_3,sizeof(splash_3));
    memcpy(&results[8],splash_4,sizeof(splash_4));
}

void scroll_reset()
{
    struct dlist_struct *p = &_dlist;
    unsigned char i;
    
    for (i=0;i<NUM_LINES;i++)
        p->line[i].addr = (unsigned short)&results[i];
}

void input(void)
{
    unsigned char pos=0;
    unsigned char k=0;
    unsigned char i=0;
    
    OS.shflok=0x40; // default for OS. CC65 turns on lowercase.
    
    memset(query,0,sizeof(query));
    
    query[0]=0x3F; // _
    
    while (k != 0x9B)
    {
        switch(k=cgetc())
        {
        case 0x7E: // BACKSPACE
            if (!pos)
                break;
            query[pos--] = 0x00;
            query[pos]   = 0x3F;
            break;
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x7D:
        case 0x7F:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xFD:
        case 0xFE:
        case 0xFF:
        case 0x9B: // RETURN
            break;
        default:
            if (pos>18)
                break;
            query[pos++] = ascii_to_screen(k);
            query[pos]   = 0x3F;
        }
    }

    // Remove cursor.
    query[pos] = 0x00;

    // Clear and reset devicespec
    memset(ds,0,sizeof(ds));
    strcpy(ds,DS_BASE);

    k=strlen(ds);

    /* Put query into devicespec */
    for (i=0;i<pos;i++)
        ds[k++] = screen_to_ascii(query[i]);

    ds[k] = 0x00;
    
    /* turn spaces into + */
    for (i=0;i<strlen(ds);i++)
            if (ds[i] == ' ')
        	    ds[i] = '+';

    memset(results,0,sizeof(results));
    memcpy(title,query,20);
    
    scroll_reset();
    bar_show(BAR_TOP_OF_WINDOW);
    pos_results=0;

    fetch();
    count();
}

char select_text[]={0x00, 0x00, 0x00, 0x1C, 0x1D, 0x00, 0x6d, 0x6f, 0x76, 0x65, 0x00, 0x1E, 0x1F, 0x00, 0x70, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x2f, 0x30, 0x34, 0x00, 0x62, 0x6f, 0x6f, 0x74, 0x00, 0x25, 0x33, 0x23, 0x00, 0x72, 0x65, 0x71, 0x75, 0x65, 0x72, 0x79};

void load(void)
{
    unsigned char i=0;
    unsigned char l=RESULTS_SIZE;
    
    memset(ds,0,sizeof(ds));
    strcpy(ds,LO_BASE);

    while (l--)
        if (results[pos_results][l])
            break;
    
    for (i=0;i<l;i++)
        results[pos_results][i] = screen_to_ascii(results[pos_results][i]);

    strcat(ds,results[pos_results]);

    network_open(ds,4,0);

    memcpy((void *)0x0120,loader_stack_bin,sizeof(loader_stack_bin));
    memcpy((void *)0x3FD,loader_bin,sizeof(loader_bin)); 

    ANTIC.pmbase = 0x00;
    
    OS.sdmctl = 0x0;
    OS.chbas = 0xe0;
    OS.sdlst = (void *)0xBC20;

    OS.pcolr0 =
        OS.pcolr1 =
        OS.pcolr2 =
        OS.pcolr3 = 0;

    OS.color0 = 0x28;
    OS.color1 = 0xca;
    OS.color2 = 0x94;
    OS.color3 = 0x46;
    OS.color4 = 0x00;
    
    GTIA_WRITE.hposp0 =
        GTIA_WRITE.hposp1 =
        GTIA_WRITE.hposp2 =
        GTIA_WRITE.hposp3 =
        GTIA_WRITE.hposm0 =
        GTIA_WRITE.hposm1 =
        GTIA_WRITE.hposm2 =
        GTIA_WRITE.hposm3 = 0;
    
    GTIA_WRITE.gractl = 0;
    GTIA_WRITE.sizep0 =
        GTIA_WRITE.sizep1 =
        GTIA_WRITE.sizep2 =
        GTIA_WRITE.sizep3 = 
        GTIA_WRITE.sizem = 0;
    
    __asm__("JMP $03FD");

    while(1);
}

/**
 * @brief world's simplest joystick slow-down
 */
unsigned char joystick(void)
{
    OS.rtclok[2] &= 0x03;

    if (!OS.rtclok[2])
    {
        while (!OS.rtclok[2]);
        return OS.stick0;
    }
    else
        return 0x0F;
}

void scroll_up(void)
{
    struct dlist_struct *p = &_dlist;
    int i=0;
    
    for (i=0;i<NUM_LINES;i++)
        p->line[i].addr -= RESULTS_SIZE;
}

void scroll_down(void)
{
    struct dlist_struct *p = &_dlist;
    int i=0;

    for (i=0;i<NUM_LINES;i++)
        p->line[i].addr += RESULTS_SIZE;
}

void select_up(void)
{
    if (!pos_results)
        return;

    if (bar_get()==BAR_TOP_OF_WINDOW)
        scroll_up();
    else
        bar_up();

    pos_results--;
}

void select_down(void)
{
    if (pos_results==num_results-1)
        return;

    if (bar_get()==BAR_BOTTOM_OF_WINDOW)
        scroll_down();
    else
        bar_down();

    pos_results++;
}

void pan_left(void)
{
    unsigned char i=0;
    struct dlist_struct *p = &_dlist;
    
    if (!pos_pan)
        return;

    for (i=0;i<NUM_LINES;i++)
        p->line[i].addr--;

    pos_pan--;
}

void pan_right(void)
{
    unsigned char i=0;
    struct dlist_struct *p = &_dlist;
    
    if (pos_pan==(RESULTS_SIZE-1) >> 1)
        return;

    for (i=0;i<NUM_LINES;i++)
        p->line[i].addr++;

    pos_pan++;
}

void select(void)
{
    memcpy(query,select_text,sizeof(select_text));

    while (1)
    {
        if (CONSOL_OPTION(GTIA_READ.consol))
            load();

        switch (joystick())
        {
        case 14: // up
            select_up();
            break;
        case 13: // dn
            select_down();
            break;
        case 11: // lf
            pan_left();
            break;
        case 7:  // rt
            pan_right();
            break;
        }

        if (!OS.strig0)
            load();
        
        if (!kbhit())
            continue;

        switch(cgetc())
        {
        case 0x1B:
            return;
        case 0x8E:
        case 0x1C:
        case '-':
            select_up();
            break;
        case 0x8F:
        case 0x1D:
        case '=':
            select_down();
            break;
        case 0x86:
        case 0x1E:
        case '+':
            pan_left();
            break;
        case 0x87:
        case 0x1F:
        case '*':
            pan_right();
            break;
        }
    }
}

void screen_query(void)
{    
    unsigned char caddr = (unsigned short)&charset[0] >> 8;

    PIA.portb = 0xFF; // Turn off BASIC
    OS.coldst = 1;    // Force coldstart
    
    OS.color0=0x0E;
    OS.color1=0x4A;
    OS.color2=0x42;
    OS.color4=0x42;
    OS.sdlst=&_dlist;
    OS.chbas=caddr;
    OS.vdslst=dli2;
    /* ANTIC.nmien=0xC0; */
    
    bar_setup_regs();
    bar_show(BAR_TOP_OF_WINDOW);

    while(1)
    {
        input();
        select();
    }
}
