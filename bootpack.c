#include <stdio.h>
#include "bootpack.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void set490( struct FIFO32 *p_fifo, int mode);
void task_b_main(struct SHEET *p_sht_back);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void HariMain(void)
{
	char *p_vram; /* pという変数は、BYTE [...]用の番地 */
	int xSize, ySize;
	int	mx, my;
	struct BOOTINFO *binfo;
	char s[40];
	struct MOUSE_DEC mdec;
	int  i = 0;
	
	uint32_t memtotal, count  = 0;
	struct MEMMAN *p_memman = ( struct MEMMAN *)MEMMAN_ADDR;
    
    struct SHTCTL *p_shtctl;
    struct SHEET *p_sht_back, *p_sht_mouse, *p_sht_win;
    uint8_t    *buf_back, *buf_win, buf_mouse[256];
    /*window   */
    int cursor_x, cursor_c;
    
    /*  fifo  */
    struct FIFO32 fifo;
    int fifobuf[128];
    /*  timer   */
    
    struct TIMER* p_timer, *p_timer2, *p_timer3;
    
    
    /* keyboard  */
    static char keytable[0x54] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
	};
    
    /* task  */
    struct TSS32 tss_a, tss_b;
    // struct TIMER* p_tssTimer;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    int task_b_esp;
	
	init_gdtidt();
	init_pic();
	io_sti();
    fifo32_init( &fifo, 128, fifobuf);
    init_pit();
	
	
    init_keyboard(&fifo, 256);
	endble_mouse( &fifo, 512, &mdec);
    
    io_out8(PIC0_IMR, 0xf8); /* PIC1とキーボードを許可(11111001) */
	io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */
    
  
    
    set490( &fifo, 0);
    p_timer = timer_alloc();
    timer_init( p_timer, &fifo, 10);
    timer_settime( p_timer, 1000);
    p_timer2 = timer_alloc();
    timer_init( p_timer2, &fifo, 3);
    timer_settime( p_timer2, 300);
    p_timer3 = timer_alloc();
    timer_init( p_timer3, &fifo,1);
    timer_settime( p_timer3, 50);
    
	init_palette(); /* パレットを設定 */
    
    memtotal = memtest( 0x00400000, 0xbfffffff);
	memman_init( p_memman);
	memman_free( p_memman, 0x000001000, 0x0009e000);  //0x00001000 - 0x0009efff
	memman_free( p_memman, 0x000400000, memtotal - 0x000400000);  //0x00001000 - 0x0009efff
	binfo = ( struct BOOTINFO *)0x0ff0;

	p_vram = binfo->vram; /* 番地を代入 */
	xSize = binfo->scrnx;
	ySize = binfo->scrny;
    
    //sheet init
    p_shtctl = shtctl_init( p_memman, binfo->vram, binfo->scrnx, binfo->scrny);
    
    p_sht_back = sheet_alloc( p_shtctl);
    buf_back = ( uint8_t *)memman_alloc_4k( p_memman, binfo->scrnx * binfo->scrny);
    sheet_setbuf( p_sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);    //无透明色
    init_screen8( buf_back, binfo->scrnx, binfo->scrny);
    sheet_slide(  p_sht_back, 0, 0);
   
    
    
    p_sht_win = sheet_alloc( p_shtctl);
    buf_win  = ( uint8_t *)memman_alloc_4k( p_memman, 160* 52);
    sheet_setbuf( p_sht_win, buf_win, 160, 52, -1);    //无透明色
    make_window8( buf_win, 160, 52, "counter");
    make_textbox8(  p_sht_win, 8, 28, 144, 16, COL8_FFFFFF);
    cursor_x = 8;
    cursor_c = COL8_FFFFFF;
    
    
    sheet_slide(  p_sht_win, 80, 72);
    
   
    
   
    p_sht_mouse = sheet_alloc( p_shtctl);
    sheet_setbuf( p_sht_mouse, buf_mouse, 16, 16, 99);      //99好像是随意指定的，但是不要跟?形的?色一?就行了
    init_mouse_cursor8( buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
	my = (binfo->scrny - 28 - 16) / 2;   
    sheet_slide(  p_sht_mouse, mx, my);        
    
    sheet_updown(  p_sht_back, 0);
    sheet_updown(  p_sht_win, 1);
    sheet_updown(  p_sht_mouse, 2);
    
    
    sprintf(s, "(%p, %p, %p)",buf_mouse, buf_back, buf_win);
	Putfont8_asc_sht( p_sht_back ,  0, 400, COL8_FFFFFF,  COL8_008484, s,  strlen( s));
	
	
	sprintf(s, "(%d, %d)", mx, my);
	Putfont8_asc_sht( p_sht_back ,  0, 0, COL8_FFFFFF,  COL8_008484, s,  strlen( s));
	
	
    
	
	sprintf(s, "memory %dMB, free:%dkB", memtotal/( 1024 * 1024), memman_total( p_memman)/1024);
	Putfont8_asc_sht( p_sht_back ,  0, 32, COL8_FFFFFF,  COL8_008484, s,  strlen( s));
    // sheet_refresh(  p_sht_back, 0, 0, binfo->scrnx,  48);
    
    // tss init  
   
    
    // p_tssTimer = timer_alloc();
    // timer_init( p_tssTimer, &fifo, 2);
    // timer_settime( p_tssTimer, 2);
    Mt_init();
    tss_a.ldtr = 0;
    tss_a.iomap = 0x40000000;
    tss_b.ldtr = 0;
    tss_b.iomap = 0x40000000;
    set_segmdesc(gdt + 3, 103,   &tss_a, AR_TSS32);
    set_segmdesc(gdt + 4, 103,   &tss_b, AR_TSS32);
    load_tr( 3 * 8);
    task_b_esp = memman_alloc_4k( p_memman, 64 * 1024) + 64 * 1024  - 8;     //ﾕ篋ﾚｴ賁ﾇﾔﾚ
     *( (int *) (task_b_esp + 4)) = (int)p_sht_back;
    tss_b.eip = ( int)&task_b_main;
    tss_b.eflags = 0x00000202; //IF = 1
    tss_b.eax = 0;
	tss_b.ecx = 0;
	tss_b.edx = 0;
	tss_b.ebx = 0;
	tss_b.esp = task_b_esp;
	tss_b.ebp = 0;
	tss_b.esi = 0;
	tss_b.edi = 0;
	tss_b.es = 1 * 8;
	tss_b.cs = 2 * 8;
	tss_b.ss = 1 * 8;
	tss_b.ds = 1 * 8;
	tss_b.fs = 1 * 8;
	tss_b.gs = 1 * 8;
    
    sprintf(s, "tss_a esp %x, eip:%x", tss_a.esp, tss_a.eip);
	Putfont8_asc_sht( p_sht_back ,  0,48, COL8_FFFFFF,  COL8_008484, s,  strlen( s));
    
	while(1) {
        
        count ++;
        
        //在窗口上?示?数器
        // sprintf(s, "%010d", timerctl.count);
      
       
        
		io_cli();
		if(  fifo32_status( &fifo) == 0)
		{
			
			io_sti();
		}
		else
		{
            i = fifo32_get( &fifo);
            io_sti();
            
            
			if( ( 256 <= i ) && ( i <= 511))
			{
				
				sprintf(s, "%02x", i - 256);
                Putfont8_asc_sht( p_sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, strlen( s));
                if( i < 256 + 0x54 && cursor_x  < 144)
                {
                    if( keytable[ i - 256] != 0)
                    {
                       s[0] =  keytable[ i - 256];
                       s[1] = '\0';
                       Putfont8_asc_sht( p_sht_win, cursor_x , 28, COL8_000000, COL8_C6C6C6, s, strlen( s));
                       cursor_x += 8;
                    }
                    
                }
                if( i == 256 + 0x0e && cursor_x > 8)
                {
                    
                    Putfont8_asc_sht( p_sht_win, cursor_x , 28, COL8_000000, COL8_FFFFFF, " ", 1);
                    cursor_x -= 8;
                }
                BoxFill8( p_sht_win->buf,  p_sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                sheet_refresh( p_sht_win,  cursor_x, 28, cursor_x + 8, 44); 
				
			}
			else if( ( 512 <= i) && ( i <= 767))
			{
				

				if( mouse_decode( &mdec, i - 512) == 1)
				{
					sprintf(s, "[lcr %04d %04d]", mdec.x, mdec.y);
					if(( mdec.btn & 0x01) != 0)
					{
						s[1] = 'L';
						
					}
					if(( mdec.btn & 0x02) != 0)
					{
						s[3] = 'p';
						
					}
					if(( mdec.btn & 0x04) != 0)
					{
						s[2] = 'C';
						
					}
					
					Putfont8_asc_sht( p_sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, strlen( s));
					/* 移?鼠? */
					
					
					//更新鼠?位置
					mx += mdec.x;
					my += mdec.y;
					
					//?界判断
					if( mx < 0)
						mx = 0;
					if( my < 0)
						my = 0;
					
					if( mx > binfo->scrnx - 1)
						mx =  binfo->scrnx - 1;
					if( my > binfo->scrny - 1)
						my =  binfo->scrny - 1;
					sprintf(s, "(%3d, %3d)", mx, my);
					//更新鼠?的坐??示
                    Putfont8_asc_sht( p_sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, strlen( s));
                    
					
					sheet_slide( p_sht_mouse, mx, my);
                    
                    //按下左?，移?光?
                    if( ( mdec.btn & 0x01) != 0)
                    {
                        sheet_slide( p_sht_win, mx - 80, my - 8);
                    }
					
				}
			}              
            else if( i == 10)
            {

                Putfont8_asc_sht( p_sht_back, 0, 64, COL8_FFFFFF, COL8_008484, "10[sec]", strlen( "10[sec]"));
                
            }
            
            else if( i == 3)
            {
                count = 0;
                Putfont8_asc_sht( p_sht_back, 0, 80, COL8_FFFFFF, COL8_008484, "3[sec]", strlen( "3[sec]"));
            }
            else if( i <= 1)
            {
             
                if( i != 0)
                {
                    timer_init( p_timer3, &fifo,0);
                    cursor_c = COL8_FFFFFF;
                    

                   
                }
                else
                {
                    timer_init( p_timer3, &fifo,1);
                    cursor_c = COL8_000000;
                }
                
                BoxFill8( p_sht_win->buf, p_sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                sheet_refresh( p_sht_win,  cursor_x, 28, cursor_x + 8, 44); 
                timer_settime( p_timer3, 50);

                
                 
               
            }
            //tss
            // else if( i == 2)
            // {

                // farjmp( 0, 4 * 8);
                // timer_settime( p_tssTimer, 2);
            // }
          
		}
		
	}
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static void set490( struct FIFO32 *p_fifo, int mode)
{
    int i;
    struct TIMER *p_timer;
    
    if( mode != 0)
    {
        for( i = 0; i < 490; i++)
        {
            p_timer = timer_alloc();
            timer_init( p_timer, p_fifo, 1024 + i);
            timer_settime( p_timer, 100 * 60* 60*24 + i * 100);
            
        }
    }
    return;
}

void task_b_main(struct SHEET *p_sht_back)
{
    
    /*  fifo  */
    struct FIFO32 fifo;
    int fifobuf[128];
    int i;
    /*  timer   */   
    // struct TIMER* p_timer;
    
    char    s[12];
    int count = 0, count0 = 0;
    struct TIMER* p_timerPut;
    
    //性能??
    struct TIMER* p_timer_1s;

	
    fifo32_init( &fifo, 128, fifobuf);

    // p_timer = timer_alloc();
    // timer_init( p_timer, &fifo, 2);
    // timer_settime( p_timer, 2);
    
    p_timerPut = timer_alloc();
    //timer_init( p_timerPut, &fifo, 1);
    //timer_settime( p_timerPut, 1);
    
    p_timer_1s = timer_alloc();
    timer_init( p_timer_1s, &fifo, 100);
    timer_settime( p_timer_1s, 100);
   
    
	while(1) { 
        count ++;
       
		io_cli();
        
		if(  fifo32_status( &fifo) == 0)
		{

			io_sti();
            io_hlt();
		}
		else
		{
            i = fifo32_get( &fifo);
            io_sti();
            
            
			// if( i == 2)
            // {
               // farjmp( 0, 3 * 8);
               // timer_settime( p_timer, 2);
            // }
            if( i == 1)
            {
                sprintf(s, "%10d", count);
                Putfont8_asc_sht( p_sht_back, 0, 144, COL8_FFFFFF, COL8_008484, s, strlen( s));
                timer_settime( p_timerPut, 1);
            }
            else if( i == 100)
            {
                sprintf(s, "%10d", count - count0);
                Putfont8_asc_sht( p_sht_back, 0, 128, COL8_FFFFFF, COL8_008484, s, strlen( s));
                count0 = count;
                timer_settime( p_timer_1s, 100);
            }
          

            
            
			
		}
		
	}
}












