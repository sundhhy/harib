//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdio.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
 /* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;

    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

/* asmhead.nas	*/
#define ADR_BOOTINFO		0x00000ff0

/* bootpack.c */
#define PORT_KEYDAT					0x0060
#define PORT_KEYSTA					0x0064
#define PORT_KEYCMD					0x0064
#define KEYSTA_SEND_NOTREADY		0x02
#define KBC_MODE					0x47
#define KEYCMD_WRITE_MODE			0X60
#define KEYCMD_SENDTO_MOUSE			0xd4
#define MOUSECMD_ENABLE				0xf4
/* graphic.c  */
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/* dsctbl.c  */
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e

/* int.c */
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1



/* fifo.c  */
#define FLAGS_OVERRUN		0x0001

/* memman.c   */
#define MEMMAN_FREES			0x4090		//32KB 
#define MEMMAN_ADDR			0x003c0000

/* sheet.c   */
#define MAX_SHEETS          256

/* timer.c   */
#define MAX_TIMER           500

/* task  */
#define MAX_TASKS       1000
#define TASK_GDT0       3  
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

/* asmhead.nas	*/
struct BOOTINFO {
	char		cyls, leds, vmode, reserve;
	short		scrnx,scrny;
	char		*vram;
};

/* dsctbl.c  */
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
	
	
};

struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
	
	
};


/* fifo.c 		*/
struct FIFO8 {
	unsigned char *buf;
	int p, q, size, free, flags;
};

struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;
};

/* keyboard.c */
struct MOUSE_DEC {
	unsigned char buf[3];
	unsigned char phase;
	int x, y, btn;
};

/* memman.c   */
struct FREEINFO {
	unsigned int addr, size;
};

struct MEMMAN {
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[ MEMMAN_FREES];
};

/* sheet.c   */
struct SHEET {
    uint8_t *buf;
    int bxsize, bysize, vx0, vy0, col_inv, height, flags;
    struct SHTCT *p_shtctl;
};

struct SHTCTL {
    uint8_t *vram, *map;
    int xsize, ysize, top;
    struct SHEET *arr_p_sheets[ MAX_SHEETS];
    struct SHEET  arr_sheets[ MAX_SHEETS];
};

/* timer.c   */
struct TIMER {
    struct TIMER    *next_p_timer;
    uint32_t timeout, flags;
    struct FIFO32    *p_fifo;
    int     data;
};

struct TIMERCTL {
    uint32_t count, nextTimeout;
    struct TIMER* p_timerHead;
    struct TIMER arr_timer[ MAX_TIMER];
    
};

/* task   */
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK {
    int sel, flags;
    struct TSS32    tss;
};

struct TASKCTL {
    int numRunning;
    int now;
    struct TASK *arr_p_tasks[MAX_TASKS];
    struct TASK arr_tasks[MAX_TASKS];
};   
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

/* int.c    */
extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;
/* timer.c    */
extern struct TIMERCTL  timerctl;

/* mstask.c  */
extern struct TIMER* p_mt_timer;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

/* naskfunc.nas	*/
void io_hlt(void);
void io_cli(void);
void io_sli(void);
void io_slihlt(void);
void io_out8(int port, int data);
int io_in8(int port);
int io_load_eflags(void);
void io_store_eflags(int eflags);
int io_load_eflags(void);
void io_store_eflags(int eflags);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub( unsigned int start, unsigned int end);
void farjmp(int rip, int cs);

/* graphic.c  */
void init_palette(void);
void init_screen8(char *buf, int xSize, int ySize);
void Putfont8_asc( char *p_vram, int numXpix, int x, int y, char c, char *s);
void init_mouse_cursor8(char *mouse, char bc);
void PutBlock8_8( char *p_vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize);
void init_screen(char *vram, int x, int y);
void BoxFill8( unsigned char *p_p_vram, int numXpix, unsigned char c, int x0, int y0, int x1, int y1);
void Putfont8_asc_sht( struct SHEET *p_sht, int x, int y, char c, int b, char *s, int len);
void make_window8(unsigned char *buf, int xsize, int ysize, char *title);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);

/* dsctbl.c  */
void init_gdtidt(void);
void set_segmdesc( struct SEGMENT_DESCRIPTOR *p_sd, unsigned int limit, int base, int ar);
/* int.c */
void init_pic(void);

void inthandler27(int *esp);









/* fifo.c */
void fifo8_init( struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_status( struct FIFO8 *fifo);
int fifo8_put( struct FIFO8 *fifo, unsigned char data);
int fifo8_get( struct FIFO8 *fifo);
void fifo32_init( struct FIFO32 *fifo, int size, int *buf);
int fifo32_status( struct FIFO32 *fifo);
int fifo32_put( struct FIFO32 *fifo, int data);
int fifo32_get( struct FIFO32 *fifo);

/* keyboard.c */
void init_keyboard(struct FIFO32 *p_fifo, int data);
void inthandler21(int *esp);
void wait_KBC_sendready(void);


/* mouse.c  */
void endble_mouse(struct FIFO32 *p_fifo, int data0, struct MOUSE_DEC *p_mdec);
int mouse_decode( struct MOUSE_DEC *p_mdec, int dat);
void inthandler2c(int *esp);

/* memman.c   */
unsigned int memtest( unsigned int start, unsigned int end);
void memman_init( struct MEMMAN *p_man);
unsigned int memman_total( struct MEMMAN *p_man);
unsigned int memman_alloc( struct MEMMAN *p_man, unsigned int size);
int memman_free( struct MEMMAN *p_man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k( struct MEMMAN *p_man, unsigned int size);
int memman_free_4k( struct MEMMAN *p_man, unsigned int addr, unsigned int size);

/* sheet.c */
struct SHTCTL* shtctl_init( struct MEMMAN *p_memman, uint8_t *vram, int xsize, int ysize);
struct SHEET *sheet_alloc( struct SHTCTL *p_ctl);
void sheet_setbuf( struct SHEET *p_sht, uint8_t *buf, int bxsize, int bysize, int col_inv);
void sheet_updown( struct SHEET *p_sht, int height);

// void sheet_refresh( struct SHTCTL *p_ctl);
void sheet_refresh( struct SHEET *p_sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(  struct SHEET *p_sht, int vx0, int vy0);
void sheet_free( struct SHEET *p_sht);

/* timer.c  */
void init_pit( void);
void inthandler20( int *esp);

struct TIMER *timer_alloc( void);
void timer_free( struct TIMER *p_timer);
void timer_init( struct TIMER *p_timer, struct FIFO32 *p_fifo, int data);
void timer_settime( struct TIMER *p_timer, uint32_t timeout);
// void settimer( uint32_t timeout, struct FIFO8 *p_fifo, uint8_t data);


/* mtask.c   */

struct TASK *Task_init( struct MEMMAN *p_mem);
struct TASK *Task_alloc( void);
void Task_run( struct TASK *p_task);
void Task_switch(void);

