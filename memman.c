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
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

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
unsigned int memtest( unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;
	
	//cpu is 386 or 486
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT;
	io_store_eflags( eflg);
	eflg = io_load_eflags();
	
	if( ( eflg & EFLAGS_AC_BIT) != 0)
		flg486 = 1;
	
	if( flg486)
	{
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0( cr0);
	}
	
	i = memtest_sub( start, end);
	
	if( flg486)
	{
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
		store_cr0( cr0);
	}
	
	return i;
	
}

void memman_init( struct MEMMAN *p_man)
{
	p_man->frees = 0;
	p_man->maxfrees = 0;
	p_man->lostsize = 0;
	p_man->losts = 0;
	
	return ;
	
	
}

unsigned int memman_total( struct MEMMAN *p_man)
{
	unsigned int i, t = 0;
	for( i = 0; i < p_man->frees; i++)
	{
		t += p_man->free[i].size;
		
	}
	
	return t;
	
}

unsigned int memman_alloc( struct MEMMAN *p_man, unsigned int size)
{
	unsigned int i, a;
	for( i = 0; i < p_man->frees; i++)
	{
		if( p_man->free[i].size >= size)
		{
			a = p_man->free[i].addr;
			p_man->free[i].addr += size;
			p_man->free[i].size -= size;
			if( p_man->free[i].size == 0)
			{
				p_man->frees --;
				for(; i < p_man->frees; i++)
				{
					p_man->free[i] = p_man->free[i + 1];
					
				}
				
			}
			
			return a;
		}
			
	}
	
	return 0;
	
	
}

int memman_free( struct MEMMAN *p_man, unsigned int addr, unsigned int size)
{
	int i,j;
	
	//à¬ínö¨ìIëÂè¨?èòîróÒ
	for( i = 0; i < p_man->frees; i++)
	{
		if( p_man->free[i].addr > addr)
			break;
		
	}
	
	
	if( i > 0)
	{
		//ó^ëOñ ìIì‡ë∂çáõÛ
		if( p_man->free[ i - 1].addr + p_man->free[ i - 1].size == addr)
		{
			p_man->free[i - 1].size += size;
		
			//ó^ç@ñ ìIì‡ë∂çáõÛ
			if( i < p_man->frees)
			{
				
				if( addr + size == p_man->free[ i].addr)
				{
					p_man->free[i - 1].size += p_man->free[ i].size;
					//?èúù{ç@ñ ?ò¢â¬ópãÛ?
					p_man->frees --;
					for( ; i < p_man->frees; i++)
					{
						p_man->free[i] = p_man->free[ i + 1];
					}
					
				}
				
				
				
			}
			
			return 0;
			
		}
		
		
		
		
		
	}
	
	//ó^ç@ñ ìIì‡ë∂çáõÛ
	if( i < p_man->frees)
	{
		if( addr + size == p_man->free[ i].addr)
		{
			p_man->free[i].addr = addr;
			p_man->free[i].size += size;
			return 0;
			
		}
		
	}
	
	//ùŸñ@çáõÛìûîCâΩãÊ?íÜÅCî@â óLãÛ?ìIãÊ?ÅCèAêV˙ùàÍò¢
	if( p_man->frees < MEMMAN_FREES)
	{
		//ò∏i?énîcç@ñ ìIãÊ?ìsâùç@à⁄
		for( j = p_man->frees; j > i; j--)
		{
			p_man->free[j] = p_man->free[ j - 1];
		}
		
		p_man->frees ++;
		if( p_man->maxfrees < p_man->frees)
		{
			p_man->maxfrees = p_man->frees;
		}
		
		p_man->free[i].addr = addr;
		p_man->free[i].size = size;
		return 0;
	}
	
	//???é∏ìI
	p_man->losts ++;
	p_man->lostsize += size;
	
	return -1;
	
	
	
	
	
}

unsigned int memman_alloc_4k( struct MEMMAN *p_man, unsigned int size)
{
    unsigned int a;
    
    size = ( size + 0xfff) & 0xfffff000;
    a = memman_alloc( p_man, size);
    return a;
    
}

int memman_free_4k( struct MEMMAN *p_man, unsigned int addr, unsigned int size)
{
    int i;
    size = ( size + 0xfff) & 0xfffff000;
    i = memman_free( p_man, addr, size);
    return i;
    
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


















