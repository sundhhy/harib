/*
 * console.c
 *
 *  Created on: 2017-8-14
 *      Author: Administrator
 */
#include "bootpack.h"

struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
};

int cons_newline( struct CONSOLE *cons);
void cons_putchar( struct CONSOLE *cons, int chr, char move);
void cons_putstr0( struct CONSOLE *cons, char *s);
void cons_putstr1( struct CONSOLE *cons, char *s, int l);
void cons_runcmd( char *cmdline, struct CONSOLE *cons, int fat, unsigned int memtotal);
void cmd_mem( struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls( struct CONSOLE *cons);
void cmd_dir( struct CONSOLE *cons);
void cmd_cat( struct CONSOLE *cons, int *fat, char *cmdline);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);

void hrb_api( int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	struct CONSOLE *cons = ( struct CONSOLE *)*((int *)0x0fec);

	if( edx == 1) {
		cons_putchar(cons, eax& 0xff, 1);
	} else if( edx == 2) {
		cons_putstr0(cons, (char *)ebx);
	} else if( edx == 3) {
		cons_putstr1(cons, (char *)ebx,ecx);
	}
	return;
}

void console_task( struct SHEET *sheet, unsigned int memtotal)
{

	struct TIMER	*timer;
	struct TASK	*task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, fifobuf[128], *fat = memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;
	char  cmdline[30];

	cons.sht = sheet;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	*((int *)0x0fec) = (int)&cons;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init( timer, &task->fifo, 1);
	timer_settime( timer, 50);
	file_readfat( fat, ( unsigned char *)( ADR_DISKIMG + 0x200));

	cons_putchar( &cons,'>', 1);
	for(;;) {
		io_cli();
		if( fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if( i <= 1) { /* 光标定时器	*/
				if( i != 0) {
					timer_init( timer, &task->fifo, 0);
					if( cons.cur_c  >= 0) {
						cons.cur_c  = COL8_FFFFFF;
					}
				} else {
					timer_init( timer, &task->fifo, 1);
					if( cons.cur_c  >= 0) {
						cons.cur_c  = COL8_000000;
					}
				}
				timer_settime( timer, 50);

			}
			if( i == 2) {		//光标on
				cons.cur_c  = COL8_FFFFFF;
			} else if( i == 3) {	//光标off
				//隐藏光标
				boxfill8(cons.sht->buf, cons.sht->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				cons.cur_c = -1;
			}
			if( 256 <= i && i <=511) {
				if( i == 8 + 256) { /* 退格键 */
					if( cons.cur_x > 16) {
						cons_putchar( &cons,' ', 0);
						cons.cur_x -= 8;
					}
				} else if( i == 10 + 256) { /* 回车键 */

					cons_putchar( &cons,' ', 0);
					cmdline[cons.cur_x/8 -2] = 0;
					cons_newline( &cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);
					cons_putchar( &cons,'>', 1);



				}  else {	//普通按键
					if( cons.cur_x < 240) {

						cmdline[cons.cur_x/8 -2] = i - 256;
						cons_putchar( &cons, i - 256, i);


					}
				}
			}
			if( cons.cur_c >= 0) {
				boxfill8(cons.sht->buf, cons.sht->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			}
			sheet_refresh(cons.sht, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}		//else

	}

}

int cons_newline( struct CONSOLE *cons)
{
	int x, y;

	if( cons->cur_y < 28 + 112) {
		cons->cur_y += 16;
	} else { /* 滚动 */

		//向上移动一行
		for( y = 28; y < 28 +112; y++) {
			for( x = 8; x < 8 + 240; x ++) {
				cons->sht->buf[ x + y * cons->sht->bxsize] = cons->sht->buf[ x + ( y + 16)* cons->sht->bxsize];
			}
		}
		//最下面这行涂黑
		for( y = 28 + 112; y < 28 +128; y++) {
			for( x = 8; x < 8 + 240; x ++) {
				cons->sht->buf[ x + y * cons->sht->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(cons->sht, 8, 28, 8 + 240, 28 + 128);

	}
	cons->cur_x = 8;
	return cons->cur_y;

}

void cons_putchar( struct CONSOLE *cons, int chr, char move)
{
	char s[2];
	s[0] = chr;
	s[1] = 0;

	if( s[0] == 0x09) { //制表符
		for(;;) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000," ", 1);
			cons->cur_x += 8;
			if( cons->cur_x == 8 + 240) {
				cons_newline( cons);
			}
			if( ( ( cons->cur_x - 8) & 0x1f) == 0)	{
				break;		//被32整除就退出,即4个空格
			}

		}
	}	else if( s[0] == 0x0a) { //换行
		cons_newline( cons);
	}	else if( s[0] == 0x0d) { //回车

	} else {

		putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000,s, 1);
		if( move) {
			cons->cur_x += 8;
			if( cons->cur_x == 8 + 240) {
				cons_newline( cons);
			}
		}
	}
}

void cons_putstr0( struct CONSOLE *cons, char *s)
{
	for(;*s != 0; s++) {
		cons_putchar( cons, *s, 1);
	}
	return;
}

void cons_putstr1( struct CONSOLE *cons, char *s, int l)
{
	int i;
	for( i = 0; i < l; i++) {
		cons_putchar( cons, s[i], 1);
	}
	return;
}

void cons_runcmd( char *cmdline, struct CONSOLE *cons, int fat, unsigned int memtotal)
{
	if( strcmp( cmdline, "mem") == 0) {
		cmd_mem( cons, memtotal);
	} else if( strcmp( cmdline, "cls") == 0) {
		cmd_cls(cons);
	} else if( strcmp( cmdline, "dir") == 0) {
		cmd_dir(cons);
	} else if( strncmp( cmdline, "cat", 3) == 0) {
		cmd_cat(cons, fat, cmdline);
	}  else if( cmdline[0] != 0) {
		if( cmd_app(cons, fat,cmdline) == 0) {
			putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "Bad command.\n\n", 12);

		}
	}
}

void cmd_mem( struct CONSOLE *cons, unsigned int memtotal)
{
	struct MEMMAN *memman = ( struct MEMMAN *)MEMMAN_ADDR;
	char s[30];


	sprintf(s, "total : %dMB\nfree : %dkB \n\n",memtotal / ( 1024 * 1024),memman_total(memman) / 1024);
	cons_putstr0( cons, s);

	return;
}

void cmd_cls( struct CONSOLE *cons)
{
	int x, y;
	for( y = 28 ; y < 28 +128; y++) {
		for( x = 8; x < 8 + 240; x ++) {
			cons->sht->buf[ x + y * cons->sht->bxsize] = COL8_000000;
		}
	}
	sheet_refresh( cons->sht, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir( struct CONSOLE *cons)
{
	struct FILEINFO *finfo = ( struct FILEINFO *)( ADR_DISKIMG + 0x002600);
	int x, y;
	char s[30];
	//在img中最多能够存放224个文件的信息
	//img存放文件信息的位置是：0x2600 - 0x4200
	for( x = 0; x < 224; x ++) {


		if( finfo[x].name[0] == 0x00) {
			break;
		}

		if( finfo[x].name[0] != 0xe5) {	//0xe5是删除标志
			if( ( finfo[x].type & 0x18) == 0) {
				sprintf( s, "filename.ext  %7d", finfo[x].size);
			}
			for( y = 0; y < 8; y++) {
				s[y] = finfo[x].name[y];
			}
			s[9] = finfo[x].ext[0];
			s[10] = finfo[x].ext[1];
			s[11] = finfo[x].ext[2];
			cons_putstr0( cons, s);
			cons_newline( cons);
		}


	}
	cons_newline( cons);
	return;

}

void cmd_cat( struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = ( struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search( cmdline + 4, ( struct FILEINFO *)( ADR_DISKIMG + 0x002600), 224);
	char *p;
	char s[30];
	int i;

	if( finfo != 0)	{
		p = (char *)memman_alloc_4k(memman, finfo->size);
		file_loadfile( finfo->clusno, finfo->size, p, fat,( char *)( ADR_DISKIMG + 0x003e00));
		cons_putstr1( cons, p, finfo->size);
		memman_free_4k( memman, (int)p, finfo->size);
	} else {
		strcat( s, "Not Found.\n" );
		cons_putstr0( cons, s);
	}
	cons_newline( cons);
	cons_newline( cons);
	return;



}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = ( struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = ( struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char name[18], *p;
	int i;

	/* 根据命令行生成文件名		*/
	for( i = 0; i < 13; i++) {
		if(cmdline[i] <= ' ')	{
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0;  /* 暂且将文件的后缀置为0		*/

	finfo = file_search( name, ( struct FILEINFO *)( ADR_DISKIMG + 0x002600), 224);
	if( finfo == 0 && name[ i - 1] != '.') {
		/* 找不到文件，在文件名后面加上".hrb"后缀后重新寻找	*/
		name[i] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search( name, ( struct FILEINFO *)( ADR_DISKIMG + 0x002600), 224);


	}

	if( finfo != 0 ) {
		p = (char *)memman_alloc_4k(memman, finfo->size);
		file_loadfile( finfo->clusno, finfo->size, p, fat,( char *)( ADR_DISKIMG + 0x003e00));
		set_segmdesc( gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER);
		farcall( 0, 1003 * 8);
		memman_free_4k( memman, (int)p, finfo->size);
		cons_newline( cons);
		return 1;

	}
	putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
	cons_newline( cons);


	return 0;
}
