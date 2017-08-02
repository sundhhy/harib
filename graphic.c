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

static void set_palette(int start, int end, unsigned char *rgb);
static void Putfont8( char *p_vram, int numXpix, int x, int y, char c, char *font);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黒 */
		0xff, 0x00, 0x00,	/*  1:明るい赤 */
		0x00, 0xff, 0x00,	/*  2:明るい緑 */
		0xff, 0xff, 0x00,	/*  3:明るい黄色 */
		0x00, 0x00, 0xff,	/*  4:明るい青 */
		0xff, 0x00, 0xff,	/*  5:明るい紫 */
		0x00, 0xff, 0xff,	/*  6:明るい水色 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:明るい灰色 */
		0x84, 0x00, 0x00,	/*  9:暗い赤 */
		0x00, 0x84, 0x00,	/* 10:暗い緑 */
		0x84, 0x84, 0x00,	/* 11:暗い黄色 */
		0x00, 0x00, 0x84,	/* 12:暗い青 */
		0x84, 0x00, 0x84,	/* 13:暗い紫 */
		0x00, 0x84, 0x84,	/* 14:暗い水色 */
		0x84, 0x84, 0x84	/* 15:暗い灰色 */
	};
	set_palette(0, 15, table_rgb);
	return;

	/* static char 命令は、データにしか使えないけどDB命令相当 */
}

void init_screen(char *p_vram, int xSize, int ySize)
{
	BoxFill8(p_vram, xSize, COL8_008484,  0,         0,          xSize -  1, ySize - 29);
	BoxFill8(p_vram, xSize, COL8_C6C6C6,  0,         ySize - 28, xSize -  1, ySize - 28);
	BoxFill8(p_vram, xSize, COL8_FFFFFF,  0,         ySize - 27, xSize -  1, ySize - 27);
	BoxFill8(p_vram, xSize, COL8_C6C6C6,  0,         ySize - 26, xSize -  1, ySize -  1);

	

	BoxFill8(p_vram, xSize, COL8_FFFFFF,  3,         ySize - 24, 59,         ySize - 24);
	BoxFill8(p_vram, xSize, COL8_FFFFFF,  2,         ySize - 24,  2,         ySize -  4);
	BoxFill8(p_vram, xSize, COL8_848484,  3,         ySize -  4, 59,         ySize -  4);
	BoxFill8(p_vram, xSize, COL8_848484, 59,         ySize - 23, 59,         ySize -  5);
	BoxFill8(p_vram, xSize, COL8_000000,  2,         ySize -  3, 59,         ySize -  3);
	BoxFill8(p_vram, xSize, COL8_000000, 60,         ySize - 24, 60,         ySize -  3);

	BoxFill8(p_vram, xSize, COL8_000000,  xSize - 47,         ySize -  25, xSize -  4,         ySize -  25);
	BoxFill8(p_vram, xSize, COL8_000000, xSize - 46,         ySize - 25, xSize -  46,         ySize -  3);

	BoxFill8(p_vram, xSize, COL8_848484, xSize - 47, ySize - 24, xSize -  4, ySize - 24);
	BoxFill8(p_vram, xSize, COL8_848484, xSize - 47, ySize - 23, xSize - 47, ySize -  4);
	BoxFill8(p_vram, xSize, COL8_FFFFFF, xSize - 47, ySize -  3, xSize -  4, ySize -  3);
	BoxFill8(p_vram, xSize, COL8_FFFFFF, xSize -  3, ySize - 24, xSize -  3, ySize -  3);
}

void init_screen8(char *buf, int xSize, int ySize)
{
	BoxFill8(buf, xSize, COL8_008484,  0,         0,          xSize -  1, ySize - 29);
	BoxFill8(buf, xSize, COL8_C6C6C6,  0,         ySize - 28, xSize -  1, ySize - 28);
	BoxFill8(buf, xSize, COL8_FFFFFF,  0,         ySize - 27, xSize -  1, ySize - 27);
	BoxFill8(buf, xSize, COL8_C6C6C6,  0,         ySize - 26, xSize -  1, ySize -  1);

	

	BoxFill8(buf, xSize, COL8_FFFFFF,  3,         ySize - 24, 59,         ySize - 24);
	BoxFill8(buf, xSize, COL8_FFFFFF,  2,         ySize - 24,  2,         ySize -  4);
	BoxFill8(buf, xSize, COL8_848484,  3,         ySize -  4, 59,         ySize -  4);
	BoxFill8(buf, xSize, COL8_848484, 59,         ySize - 23, 59,         ySize -  5);
	BoxFill8(buf, xSize, COL8_000000,  2,         ySize -  3, 59,         ySize -  3);
	BoxFill8(buf, xSize, COL8_000000, 60,         ySize - 24, 60,         ySize -  3);

	BoxFill8(buf, xSize, COL8_000000,  xSize - 47,         ySize -  25, xSize -  4,         ySize -  25);
	BoxFill8(buf, xSize, COL8_000000, xSize - 46,         ySize - 25, xSize -  46,         ySize -  3);

	BoxFill8(buf, xSize, COL8_848484, xSize - 47, ySize - 24, xSize -  4, ySize - 24);
	BoxFill8(buf, xSize, COL8_848484, xSize - 47, ySize - 23, xSize - 47, ySize -  4);
	BoxFill8(buf, xSize, COL8_FFFFFF, xSize - 47, ySize -  3, xSize -  4, ySize -  3);
	BoxFill8(buf, xSize, COL8_FFFFFF, xSize -  3, ySize - 24, xSize -  3, ySize -  3);
}

void init_mouse_cursor8(char *mouse, char bc)
/* マウスカーソルを準備（16x16） */
{
	static char cursor[16][16] = {
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

void PutBlock8_8( char *p_vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize)
{
	int x, y;
	for( y = 0; y < pysize; y++)
	{
		for( x = 0; x < pxsize; x ++)
		{
			p_vram[ ( py0 + y) * vxsize + ( px0 + x)] = buf[ y * bxsize + x];
			
		}
		
	}
	
}

void Putfont8_asc( char *p_vram, int numXpix, int x, int y, char c, char *s)
{
	
	extern char hankaku[4096];
	for(; *s != '\0'; s++)
	{
		Putfont8( p_vram, numXpix, x, y, c, hankaku + *s * 16);
		x += 8;
		
	}
	
}

void BoxFill8( unsigned char *p_vram, int numXpix, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x,y;
	for( y = y0; y <= y1; y++) 
	{
		for( x = x0; x <= x1; x++)
		{
			p_vram[ y * numXpix + x] = c;
			
		}
		
		
	}
	
	return;
	
	
}

void Putfont8_asc_sht( struct SHEET *p_sht, int x, int y, char c, int b, char *s, int len)
{
	
	BoxFill8( p_sht->buf, p_sht->bxsize, b, x, y, x + len * 8 - 1, y + 15);
    Putfont8_asc( p_sht->buf, p_sht->bxsize, x, y, c, s);
    sheet_refresh(  p_sht, x, y, x + len * 8,  y + 16);
}

void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act)
{
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c, tc, tbc;
    
    if( act != 0)
    {
       tc =  COL8_FFFFFF;
       tbc =  COL8_000084;
    }
    else
    {
        tc =  COL8_C6C6C6;
       tbc =  COL8_848484;
        
    }
	BoxFill8(buf, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
	BoxFill8(buf, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
	BoxFill8(buf, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
	BoxFill8(buf, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
	BoxFill8(buf, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
	BoxFill8(buf, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
	BoxFill8(buf, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
	BoxFill8(buf, xsize, tbc, 3,         3,         xsize - 4, 20       );
	BoxFill8(buf, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
	BoxFill8(buf, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
	Putfont8_asc(buf, xsize, 24, 4, tc, title);
	for (y = 0; y < 14; y++) {
		for (x = 0; x < 16; x++) {
			c = closebtn[y][x];
			if (c == '@') {
				c = COL8_000000;
			} else if (c == '$') {
				c = COL8_848484;
			} else if (c == 'Q') {
				c = COL8_C6C6C6;
			} else {
				c = COL8_FFFFFF;
			}
			buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
		}
	}
	return;
    
    
    
    
}

void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c)
{
	int x1 = x0 + sx, y1 = y0 + sy;
	BoxFill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
	BoxFill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
	BoxFill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
	BoxFill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
	BoxFill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
	BoxFill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
	BoxFill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
	BoxFill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
	BoxFill8(sht->buf, sht->bxsize, c,           x0 - 1, y0 - 1, x1 + 0, y1 + 0);
	return;
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 割り込み許可フラグの値を記録する */
	io_cli(); 					/* 許可フラグを0にして割り込み禁止にする */
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 割り込み許可フラグを元に戻す */
	return;
}



static void Putfont8( char *p_vram, int numXpix, int x, int y, char c, char *font)
{
	int i;
	char*p;
	char d;
	
	for( i = 0; i < 16; i++)
	{
		d = font[i];
		p = p_vram + (y + i ) * numXpix + x;
		if( d & 0x80)	 p[0] = c;
		if( d & 0x40)	 p[1] = c;
		if( d & 0x20)	 p[2] = c;
		if( d & 0x10)	 p[3] = c;
		if( d & 0x08)	 p[4] = c;
		if( d & 0x04)	 p[5] = c;
		if( d & 0x02)	 p[6] = c;
		if( d & 0x01)	 p[7] = c;
		
	}
	
	return ;
	
	
	
}





