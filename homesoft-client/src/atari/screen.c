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
#ifdef ENABLE_BOOTSEL
#include "fujinet-fuji.h"
#endif
#include "screen.h"
#include "bar.h"
#include "charset.h"

extern unsigned char _LSTACK_RUN__[];
extern unsigned char _LSTACK_LOAD__[];
extern unsigned char _LSTACK_SIZE__[];
extern unsigned char _LOADER_RUN__[];
extern unsigned char _LOADER_LOAD__[];
extern unsigned char _LOADER_SIZE__[];

extern void load_app(void);
extern void reset_screen(void);

extern char ascii_to_screen(char c);
extern char screen_to_ascii(char c);

extern void dli();
extern void dli2();

#pragma data-name (push,"SCREEN")
#define RESULTS_MAX 128
#define RESULTS_SIZE 128
char results[RESULTS_MAX][RESULTS_SIZE];

#pragma data-name (push,"DATA")
static char title[] = "\x00\x00homesoft\x00\x00search\x00\x00";
static char query[] = {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

char ds[256];

unsigned char num_results=0;
unsigned char pos_results=0;
unsigned char pos_pan=0;

#ifdef ENABLE_BOOTSEL
/*
 * 0 = N: over TNFS
 * 1 = N: over HTTPS
 * 2 = D: over TNFS
 * 3 = D: over HTTPS
 */
unsigned char boot_method=0;
#endif

#define BAR_TOP_OF_WINDOW 2
#define BAR_BOTTOM_OF_WINDOW 21
#define NUM_LINES 20

#define DS_BASE "N:HTTPS://apps.irata.online/homesoft?query="
//#define DS_BASE "N:http://localhost:1080/homesoft?query="

#ifdef ENABLE_BOOTSEL
#define DS_DISK_OPT "&d=1"
#define LO_BASE_PROTO1 "N:TNFS"
#define LO_BASE_PROTO2 "N:https"
#define LO_BASE_HOST "://apps.irata.online"
#define LO_BASE_DIR "/Atari_8-bit/Games/Homesoft/"
#else
#define LO_BASE "N:TNFS://apps.irata.online/Atari_8-bit/Games/Homesoft/"
#endif

#pragma data-name (push,"DISPLAY")
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


#pragma data-name (push,"CODE")
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

#ifdef ENABLE_BOOTSEL
    // append option to search for disk images too (ATR)
    strcat(ds, DS_DISK_OPT);
#endif
    
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

#ifdef ENABLE_BOOTSEL
char select_text[]={0x1C, 0x1D, 0x00, 0x6d, 0x6f, 0x76, 0x65, 0x00, 0x1E, 0x1F, 0x00, 0x70, 0x61, 0x6e, 0x00, 0x33, 0x25, 0x2c, 0x00, 0x00, 0x2f, 0x30, 0x34, 0x00, 0x62, 0x6f, 0x6f, 0x74, 0x00, 0x25, 0x33, 0x23, 0x00, 0x72, 0x65, 0x71, 0x75, 0x65, 0x72, 0x79};
char tmp[256];
#else
char select_text[]={0x00, 0x00, 0x00, 0x1C, 0x1D, 0x00, 0x6d, 0x6f, 0x76, 0x65, 0x00, 0x1E, 0x1F, 0x00, 0x70, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x2f, 0x30, 0x34, 0x00, 0x62, 0x6f, 0x6f, 0x74, 0x00, 0x25, 0x33, 0x23, 0x00, 0x72, 0x65, 0x71, 0x75, 0x65, 0x72, 0x79};
char tmp[128];
#endif

void load(void)
{
    unsigned char i=0;
    unsigned char l=RESULTS_SIZE;
    unsigned char sep;

    memset(ds,0,sizeof(ds));

#ifdef ENABLE_BOOTSEL
    if (boot_method & 1)
    {
        // HTTPS
        strcpy(ds, LO_BASE_PROTO2);
        sep = sizeof(LO_BASE_PROTO2 LO_BASE_HOST) - 1;
    }
    else
    {
        // TNFS
        strcpy(ds,LO_BASE_PROTO1);
        sep = sizeof(LO_BASE_PROTO1 LO_BASE_HOST) - 1;
    }
    strcat(ds, LO_BASE_HOST LO_BASE_DIR);
#else
    strcpy(ds,LO_BASE);
#endif


    memset(tmp,0,sizeof(tmp));

    while (l--)
        if (results[pos_results][l])
            break;
    
    for (i = 0; i <= l; i++) {
        // avoid screen corruption by copying to a temp buffer
        tmp[i] = screen_to_ascii(results[pos_results][i]);
    }
    strcat(ds, tmp);

#ifdef ENABLE_BOOTSEL
    // disk boot for ATR
    i = strlen(tmp);
    if (i >=4 && !strcasecmp(".atr", &tmp[i-4]))
        boot_method |= 2;
#endif

    // reset before network open to show the message for a short time while it does its thing
    reset_screen();

#ifdef ENABLE_BOOTSEL
    // this adds 113 bytes to the code size, but shows the short name of the game being loaded
    cputsxy(0, 0, "loading ");
    cputs(tmp);
    // wait a bit for restored screen before doing next SIO
    i = OS.rtclok[2] + 2;
    while (i != OS.rtclok[2]);

    if (boot_method & 2)
    {
        // disk boot

        // get hosts into tmp
        fuji_get_host_slots((unsigned char (*)[sizeof(HostSlot)])tmp, 8);
        // skip "N:TNFS://" or "N:" for HTTPS
        i = (boot_method & 1) ? 2 : 9;
        // split host and path part (first slash in path)
        ds[sep] = '\0';
        // host already exists?
        for(l = 0; l < 8; l++)
            if (!strcasecmp(&tmp[l*sizeof(HostSlot)], &ds[i]))
                break;
        if (l == 8)
        {
            // set new host to slot 8
            strcpy(&tmp[7*sizeof(HostSlot)], &ds[i]);
            fuji_put_host_slots((unsigned char (*)[sizeof(HostSlot)])tmp, 8);
            l = 7;
        }
        // restore first character in path
        ds[sep] = '/';
        // set file path into device slot
        fuji_set_device_filename(0, l, 0, &ds[sep]);
        // get ready
        fuji_mount_all();
        // cold start to boot from disk
        asm ("JMP $E477");
    }
#endif

    network_open(ds,4,0);

    // move the relocatable code to its target location. See custom-atari.cfg
    memcpy(_LSTACK_RUN__, _LSTACK_LOAD__, _LSTACK_SIZE__);
    memcpy(_LOADER_RUN__, _LOADER_LOAD__, _LOADER_SIZE__);

    // off we go!
    load_app();
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

#ifdef ENABLE_BOOTSEL
void toggle_boot(void)
{
    boot_method++;
    boot_method &= 3;
    query[19] = boot_method + 0x61;
    while (CONSOL_SELECT(GTIA_READ.consol));
}
#endif

void select(void)
{
    memcpy(query,select_text,sizeof(select_text));
#ifdef ENABLE_BOOTSEL
    query[19] = boot_method + 0x61;
#endif

    while (1)
    {
        if (CONSOL_OPTION(GTIA_READ.consol))
            load();

#ifdef ENABLE_BOOTSEL
        if (CONSOL_SELECT(GTIA_READ.consol))
            toggle_boot();
#endif

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
