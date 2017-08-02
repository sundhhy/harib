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
	int	mx, my;
	struct BOOTINFO *binfo;
	char s[40];
	struct MOUSE_DEC mdec;
	int  i = 0;
	
	uint32_t memtotal, count  = 0;
	struct MEMMAN *p_memman = ( struct MEMMAN *)MEMMAN_ADDR;
    
    struct SHTCTL *p_shtctl;
    struct SHEET *p_sht_back, *p_sht_mouse, *p_sht_win, *p_sht_winb[3];
    uint8_t    *buf_back, *buf_win, *buf_winb, buf_mouse[256];
    /*window   */
    int cursor_x, cursor_c;
    
    /*  fifo  */
    struct FIFO32 fifo;
    int fifobuf[128];
    /*  timer   */
    
    struct TIMER* p_timer;
    
    
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
    struct TSS32  *p_tka, *p_tkb[3];
    // struct TIMER* p_tssTimer;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    int task_b_esp;
	
	init_gdtidt();
	init_pic();
	io_sti();
    fifo32_init( &fifo, 128, fifobuf, 0);
    init_pit();
	
	
    init_keyboard(&fifo, 256);
	endble_mouse( &fifo, 512, &mdec);
    
    io_out8(PIC0_IMR, 0xf8); /* PIC1とキーボードを許可(11111001) */
	io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */
    
  
    
    // set490( &fifo, 0);
   
    
    
	init_palette(); /* パレットを設定 */
    
    memtotal = memtest( 0x00400000, 0xbfffffff);
	memman_init( p_memman);
	memman_free( p_memman, 0x000001000, 0x0009e000);  //0x00001000 - 0x0009efff
	memman_free( p_memman, 0x000400000, memtotal - 0x000400000);  //0x00001000 - 0x0009efff
	binfo = ( struct BOOTINFO *)0x0ff0;


    
    p_tka = Task_init( p_memman);
    fifo.p_tsk = p_tka;
    
    //sheet init
    p_shtctl = shtctl_init( p_memman, binfo->vram, binfo->scrnx, binfo->scrny);
    
    p_sht_back = sheet_alloc( p_shtctl);
    buf_back = ( uint8_t *)memman_alloc_4k( p_memman, binfo->scrnx * binfo->scrny);
    sheet_setbuf( p_sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);    //无透明色
    init_screen8( buf_back, binfo->scrnx, binfo->scrny);
    
    for( i = 0; i < 3; i++)
    {
        p_sht_winb[i] = sheet_alloc( p_shtctl);
        buf_winb = ( uint8_t *)memman_alloc_4k( p_memman,144 * 52);
        sheet_setbuf( p_sht_winb[i], buf_winb, 144, 52, -1);    //无透明色
        sprintf( s, "taskb%d", i);
        make_window8( buf_winb, 144, 52, s, 0);
        
        
        p_tkb[i] = Task_alloc();
        p_tkb[i]->esp = memman_alloc_4k( p_memman, 64 * 1024) + 64 * 1024  - 8;
        p_tkb[i]->es = 1 * 8;
        p_tkb[i]->cs = 2 * 8;
        p_tkb[i]->ss = 1 * 8;
        p_tkb[i]->ds = 1 * 8;
        p_tkb[i]->fs = 1 * 8;
        p_tkb[i]->gs = 1 * 8;
         *( (int *) (p_tkb[ i]->esp + 4)) = (int) p_sht_winb[i];
        p_tkb[i]->eip = (int)&task_b_main;      
        Task_run( p_tkb[i]);
        
    }
    
    
   
    
    
    p_sht_win = sheet_alloc( p_shtctl);
    buf_win  = ( uint8_t *)memman_alloc_4k( p_memman, 160* 52);
    sheet_setbuf( p_sht_win, buf_win, 144, 52, -1);    //无透明色
    make_window8( buf_win, 144, 52, "task_a", 1);
    make_textbox8(  p_sht_win, 8, 28, 128, 16, COL8_FFFFFF);
    cursor_x = 8;
    cursor_c = COL8_FFFFFF;
    
    p_timer = timer_alloc();
    timer_init( p_timer, &fifo, 1);
    timer_settime( p_timer, 50);
    
    
    // 
    p_sht_mouse = sheet_alloc( p_shtctl);
    sheet_setbuf( p_sht_mouse, buf_mouse, 16, 16, 99);      //99好像是随意指定的，但是不要跟?形的?色一?就行了
    init_mouse_cursor8( buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
	my = (binfo->scrny - 28 - 16) / 2;   
    
    sheet_slide(  p_sht_back, 0, 0);
    sheet_slide(  p_sht_winb[0], 168, 56);
    sheet_slide(  p_sht_winb[1], 8, 116);
    sheet_slide(  p_sht_winb[2], 168, 116);
    sheet_slide(  p_sht_win, 8, 56);
    sheet_slide(  p_sht_mouse, mx, my);        


    sheet_updown(  p_sht_back, 0);
    sheet_updown(  p_sht_winb[0], 1);
    sheet_updown(  p_sht_winb[1], 2);
    sheet_updown(  p_sht_winb[2], 3);
    sheet_updown(  p_sht_win, 4);
    sheet_updown(  p_sht_mouse, 5);
    
    
    // sprintf(s, "(%p, %p, %p)",buf_mouse, buf_back, buf_win);
	// Putfont8_asc_sht( p_sht_back ,  0, 400, COL8_FFFFFF,  COL8_008484, s,  strlen( s));
	
	
	sprintf(s, "(%d, %d)", mx, my);
	Putfont8_asc_sht( p_sht_back ,  0, 0, COL8_FFFFFF,  COL8_008484, s,  strlen( s));
	
	
    
	
	sprintf(s, "memory %dMB, free:%dkB", memtotal/( 1024 * 1024), memman_total( p_memman)/1024);
	Putfont8_asc_sht( p_sht_back ,  0, 32, COL8_FFFFFF,  COL8_008484, s,  strlen( s));

    
   
   
    
	while(1) {
        
        count ++;
        
        //在窗口上?示?数器
        // sprintf(s, "%010d", timerctl.count);
      
       
        
		io_cli();
		if(  fifo32_status( &fifo) == 0)
		{
			Task_sleep( p_tka);
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
                if( i < 256 + 0x54 && cursor_x  < 128)
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
            else if( i <= 1)
            {
             
                if( i != 0)
                {
                    timer_init( p_timer, &fifo,0);
                    cursor_c = COL8_FFFFFF;
                    

                   
                }
                else
                {
                    timer_init( p_timer, &fifo,1);
                    cursor_c = COL8_000000;
                }
                
                BoxFill8( p_sht_win->buf, p_sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
                sheet_refresh( p_sht_win,  cursor_x, 28, cursor_x + 8, 44); 
                timer_settime( p_timer, 50);

                
                 
               
            }
           
          
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

    
    char    s[12];
    
    
    //性能??
    int count = 0, count0 = 0;
    struct TIMER* p_timerPut;
    struct TIMER* p_timer_1s;

	
    fifo32_init( &fifo, 128, fifobuf, 0);

    
    
    p_timerPut = timer_alloc();
    timer_init( p_timerPut, &fifo, 1);
    timer_settime( p_timerPut, 1);

    
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
            
            
	
            if( i == 1)
            {
                sprintf(s, "%10d", count);
                Putfont8_asc_sht( p_sht_back, 0, 144, COL8_FFFFFF, COL8_008484, s, strlen( s));
                timer_settime( p_timerPut, 1);
            }
            else if( i == 100)
            {
                sprintf(s, "%10d", count - count0);
                Putfont8_asc_sht( p_sht_back, 24, 28, COL8_FFFFFF, COL8_C6C6C6, s, strlen( s));
                count0 = count;
                timer_settime( p_timer_1s, 100);
            }
          

            
            
			
		}
		
	}
}












