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
void fifo8_init( struct FIFO8 *fifo, int size, unsigned char *buf)
{
	fifo->buf = buf;
	fifo->size = size;
	fifo->free = size;
	fifo->p = 0;
	fifo->q = 0;
	fifo->flags = 0;
	return;
	
}

void fifo32_init( struct FIFO32 *fifo, int size, int *buf)
{
	fifo->buf = buf;
	fifo->size = size;
	fifo->free = size;
	fifo->p = 0;
	fifo->q = 0;
	fifo->flags = 0;
	return;
	
}



int fifo8_put( struct FIFO8 *fifo, unsigned char data)
{
	if( fifo->free == 0) 
	{
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
		
	}
	
	fifo->buf[ fifo->p] = data;
	fifo->p ++;
	if( fifo->p == fifo->size)
		fifo->p = 0;
	fifo->free --;
	return 0;
	
}

int fifo32_put( struct FIFO32 *fifo, int data)
{
	if( fifo->free == 0) 
	{
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
		
	}
	
	fifo->buf[ fifo->p] = data;
	fifo->p ++;
	if( fifo->p == fifo->size)
		fifo->p = 0;
	fifo->free --;
	return 0;
	
}

int fifo8_get( struct FIFO8 *fifo)
{
	int data;
	if( fifo->free == fifo->size)
		return -1;
	
	data = fifo->buf[ fifo->q];
	fifo->q ++;
	if( fifo->q == fifo->size)
		fifo->q = 0;
	fifo->free ++;
	return data;
}

int fifo32_get( struct FIFO32 *fifo)
{
	int data;
	if( fifo->free == fifo->size)
		return -1;
	
	data = fifo->buf[ fifo->q];
	fifo->q ++;
	if( fifo->q == fifo->size)
		fifo->q = 0;
	fifo->free ++;
	return data;
}

int fifo8_status( struct FIFO8 *fifo)
{
	
	return fifo->size - fifo->free;
}

int fifo32_status( struct FIFO32 *fifo)
{
	
	return fifo->size - fifo->free;
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{













