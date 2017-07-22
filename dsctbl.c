#include "bootpack.h"













void set_segmdesc( struct SEGMENT_DESCRIPTOR *p_sd, unsigned int limit, int base, int ar)
{
	
	
	if( limit > 0xfffff)
	{
		ar |= 0x8000; //G_bit = 1 ?址空?以???位，最大可?4G
		limit /= 0x1000;
		
	}
	
	p_sd->limit_low = limit & 0xffff;
	p_sd->base_low = base & 0xffff;
	p_sd->base_mid = ( base >> 16) & 0xff;
	p_sd->access_right = ar & 0xff;
	p_sd->limit_high = ( ( limit >> 16) & 0x0f) | ( ( ar >> 8) & 0xf0);
	p_sd->base_high = ( base >> 24) & 0xff;
	
	
	
	return ;
	
}

void set_gatedesc( struct GATE_DESCRIPTOR *p_gd, int offset, int selector, int ar)
{
	//段号的低三位无效
	selector = selector << 3;
	p_gd->offset_low = offset & 0xffff;
	p_gd->selector = selector;
	p_gd->dw_count = ( ar >> 8) & 0xff;
	p_gd->access_right = ar & 0xff;
	p_gd->offset_high = ( offset >> 16) & 0xffff;
	
	
	
	
	return;
	
}

void init_gdtidt(void)
{
	
	struct SEGMENT_DESCRIPTOR *p_gdt = ( struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	struct GATE_DESCRIPTOR *p_idt = ( struct GATE_DESCRIPTOR *)ADR_IDT;
	int i;
	
	/* GDT 初始化 */
	for( i = 0; i < 8192; i ++) 
	{
		
		set_segmdesc( p_gdt + i, 0, 0, 0);
	}
	set_segmdesc(p_gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);
	set_segmdesc(p_gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
	load_gdtr(LIMIT_GDT, ADR_GDT);
	
	/* IDT 初始化		*/
	for( i = 0; i < 256; i ++) 
		set_gatedesc( p_idt + i, 0, 0, 0);
	
	load_idtr(LIMIT_IDT, ADR_IDT);
    set_gatedesc( p_idt + 0x20, (int)asm_inthandler20, 2, AR_INTGATE32);
	set_gatedesc( p_idt + 0x21, (int)asm_inthandler21, 2, AR_INTGATE32);
	set_gatedesc( p_idt + 0x2C, (int)asm_inthandler2c, 2, AR_INTGATE32);
	set_gatedesc( p_idt + 0x27, (int)asm_inthandler27, 2, AR_INTGATE32);

	
	return;
	
	
	
}

