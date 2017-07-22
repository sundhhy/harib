#include "bootpack.h"
#include <stdio.h>

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



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void init_pic(void)
{
	io_out8( PIC0_IMR, 0XFF);	//Ö~Lf
	io_out8( PIC1_IMR, 0XFF);
	
	io_out8( PIC0_ICW1, 0x11); 			//?G?
	io_out8( PIC0_ICW2, 0x20);		 	// irq 0 - 7 RINT20 -27Ú¾
	io_out8( PIC0_ICW3, 1 << 2);	 	//PIC1RIRQ2?Ú
	io_out8( PIC0_ICW4, 0x1); 			//Ù?¶æÍ®
	
	io_out8( PIC1_ICW1, 0x11); 			//?G?
	io_out8( PIC1_ICW2, 0x28);		 	// irq 0 - 7 RINT20 -27Ú¾
	io_out8( PIC1_ICW3, 2);	 			//PIC1?ÚPIC0IIRQ2ã
	io_out8( PIC1_ICW4, 0x1); 			//Ù?¶æÍ®
	
	io_out8( PIC0_IMR, 0XFB);	//Ö~PIC1ÈOIf
	io_out8( PIC1_IMR, 0XFF);
	
	return;
	
}

//??f


void inthandler27( int *esp)
{
	#if 0
	struct BOOTINFO *binfo = ( struct BOOTINFO *)ADR_BOOTINFO;
	BoxFill8( binfo->vram, binfo->scrnx, COL8_000000, 0, 0, 32 * 8 - 1, 15);
	Putfont8_asc( binfo->vram, binfo->scrnx, 0, 0,  COL8_FFFFFF, "INT 27 (IRQ-7): PIC");
	
	io_out8(PIC0_OCW2, 0x67); /* IRQ-07ót®¹ðPICÉÊm(7-1QÆ) */
	#endif
	return;	
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{













