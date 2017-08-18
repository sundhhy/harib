/*
 * file.c
 *
 *  Created on: 2017-8-14
 *      Author: Administrator
 */
#include "bootpack.h"
//将磁盘映像 的FAT解压缩
void file_readfat( int *fat, unsigned char *img)
{
	int i, j = 0;
	for( i = 0; i < 2880; i += 2) {
		fat[ i + 0] = ( img[j + 0] | img[j + 1] << 8) & 0xfff;
		fat[ i + 1] = ( img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
	return;
}

void file_loadfile( int clustno, int size, char *buf, int *fat, char *img)
{
	int i;
	for( ;; ) {
		if( size <= 512) {
			for( i = 0; i < size; i ++)	{
				buf[i] = img[clustno * 512 + i];

			}
			break;
		}
		for( i = 0; i < 512; i ++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}

	return;
}

struct FILEINFO *file_search( char *name, struct FILEINFO *finfo, int max)
{
	int y, x;
	char s[12];
	for( y = 0; y < 11; y++) {
		s[y] = ' ';
	}
	y = 0;

	//装载文件名到s
	for( x = 0; name[x] != 0; x ++) {
		if( y >= 11 ) return 0;

		if( name[x] =='.' && y <= 9) {
			y = 8;
		}	else {
			s[y] = name[x];
			if( 'a' <= s[y] && s[y] <= 'z') {
				s[y] -= 0x20;
			}
			y ++;
		}


	}
	for( x = 0; x < max; ) {
		if( finfo[x].name == 0x00) {
			break;

		}

		if((finfo[x].type & 0x18) == 0)	{
			for( y = 0; y < 11 ;y ++) {
				if( finfo[x].name[y] != s[y]) {
					goto next;
				}

			}
			return finfo + x;
		}
		next:
			x ++;
	}

	return 0;
}
