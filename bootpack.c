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
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void HariMain(void)
{
	char *p_vram; /* p�Ƃ����ϐ��́ABYTE [...]�p�̔Ԓn */
	int xSize, ySize;
	int	mx, my;
	struct BOOTINFO *binfo;
	char s[40];
	struct MOUSE_DEC mdec;
	int  i = 0;
	
	uint32_t memtotal, count  = 0;
	struct MEMMAN *p_memman = ( struct MEMMAN *)MEMMAN_ADDR;
    
    struct SHTCTL *p_shtctl;
    struct SHEEL *p_sht_back, *p_sht_mouse, *p_sht_win;
    uint8_t    *buf_back, *buf_win, buf_mouse[256];
    
    /*  fifo  */
    struct FIFO32 fifo;
    int fifobuf[128];
    /*  timer   */
    
    struct TIMER* p_timer, *p_timer2, *p_timer3;
	
	init_gdtidt();
	init_pic();
	io_sti();
    fifo32_init( &fifo, 128, fifobuf);
    init_pit();
	
	
    init_keyboard(&fifo, 256);
	endble_mouse( &fifo, 512, &mdec);
    
    io_out8(PIC0_IMR, 0xf8); /* PIC1�ƃL�[�{�[�h������(11111001) */
	io_out8(PIC1_IMR, 0xef); /* �}�E�X������(11101111) */
    
  
    
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
    
	init_palette(); /* �p���b�g��ݒ� */
    
    memtotal = memtest( 0x00400000, 0xbfffffff);
	memman_init( p_memman);
	
	memman_free( p_memman, 0x000400000, memtotal - 0x000400000);  //0x00001000 - 0x0009efff
	memman_free( p_memman, 0x000001000, 0x0009e000);  //0x00001000 - 0x0009efff
	binfo = ( struct BOOTINFO *)0x0ff0;

	p_vram = binfo->vram; /* �Ԓn���� */
	xSize = binfo->scrnx;
	ySize = binfo->scrny;
    
    //sheet init
    p_shtctl = shtctl_init( p_memman, binfo->vram, binfo->scrnx, binfo->scrny);
    
    p_sht_back = sheet_alloc( p_shtctl);
    buf_back = ( uint8_t *)memman_alloc_4k( p_memman, binfo->scrnx * binfo->scrny);
    sheet_setbuf( p_sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);    //�ٓ����F
    init_screen8( buf_back, binfo->scrnx, binfo->scrny);
    sheet_slide(  p_sht_back, 0, 0);
   
    
    
    p_sht_win = sheet_alloc( p_shtctl);
    buf_win  = ( uint8_t *)memman_alloc_4k( p_memman, 160* 52);
    sheet_setbuf( p_sht_win, buf_win, 160, 52, -1);    //�ٓ����F
    make_window8( buf_win, 160, 52, "counter");
    sheet_slide(  p_sht_win, 80, 72);
    
   
    
   
    p_sht_mouse = sheet_alloc( p_shtctl);
    sheet_setbuf( p_sht_mouse, buf_mouse, 16, 16, 99);      //99�D�������ӎw��I�C�A���s�v��?�`�I?�F��?�A�s��
    init_mouse_cursor8( buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* ��ʒ����ɂȂ�悤�ɍ��W�v�Z */
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
    
    
    
	while(1) {
        
        count ++;
        
        //���x����?��?����
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
				
				sprintf(s, "%02x", i);
                Putfont8_asc_sht( p_sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, strlen( s));
				
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
					/* ��?�l? */
					
					
					//�X�V�l?�ʒu
					mx += mdec.x;
					my += mdec.y;
					
					//?�E���f
					if( mx < 0)
						mx = 0;
					if( my < 0)
						my = 0;
					
					if( mx > binfo->scrnx - 1)
						mx =  binfo->scrnx - 1;
					if( my > binfo->scrny - 1)
						my =  binfo->scrny - 1;
					sprintf(s, "(%3d, %3d)", mx, my);
					//�X�V�l?�I��??��
                    Putfont8_asc_sht( p_sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, strlen( s));
                    
					
					sheet_slide( p_sht_mouse, mx, my);
					
				}
			}              
            else if( i == 10)
            {
                sprintf(s, "%010d", count);
                Putfont8_asc_sht( p_sht_win, 40, 28, COL8_000000, COL8_C6C6C6, s, strlen( s));
                Putfont8_asc_sht( p_sht_back, 0, 64, COL8_FFFFFF, COL8_008484, "10[sec]", strlen( "10[sec]"));
               
            }
            else if( i == 3)
            {
                count = 0;
                Putfont8_asc_sht( p_sht_back, 0, 80, COL8_FFFFFF, COL8_008484, "3[sec]", strlen( "3[sec]"));
            }
            else if( i == 1)
            {
             
               timer_init( p_timer3, &fifo,0);
               BoxFill8( buf_back, binfo->scrnx, COL8_FFFFFF, 8, 96, 15, 111);
              
               timer_settime( p_timer3, 50);
               sheet_refresh( p_sht_back, 8, 96, 15, 111); 
               
            }
            else if( i == 0)
            {
              
               
               timer_init( p_timer3, &fifo,1);
               BoxFill8( buf_back, binfo->scrnx, COL8_000000, 8, 96, 15, 111);
                   
               
               timer_settime( p_timer3, 50);
               sheet_refresh( p_sht_back, 8, 96, 15, 111); 
               
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











