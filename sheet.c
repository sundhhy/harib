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
#define SHEET_USE		1

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void sheet_refreshsub( struct SHTCTL *p_ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
static void sheet_refreshmap( struct SHTCTL *p_ctl, int vx0, int vy0, int vx1, int vy1, int h0);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
struct SHTCTL* shtctl_init( struct MEMMAN *p_memman, uint8_t *vram, int xsize, int ysize)
{
	struct SHTCTL* p_ctl;
	int i;
    
    p_ctl = ( struct SHTCTL*)memman_alloc_4k( p_memman, sizeof( struct SHTCTL));
    
    if( p_ctl == 0)
        goto err;
    
    p_ctl->map = ( uint8_t *)memman_alloc_4k( p_memman, xsize * ysize);
    if( p_ctl == 0)
        goto err1;
	
	p_ctl->vram = vram;
    p_ctl->xsize = xsize;
    p_ctl->ysize = ysize;
    p_ctl->top = -1;        //堦槩sheet栫杤桳
    
    for( i = 0; i < MAX_SHEETS; i++)
    {
        p_ctl->arr_sheets[i].flags = 0;
        p_ctl->arr_sheets[i].p_shtctl = p_ctl;
        
    }
    
    return p_ctl;
err1:
   memman_free_4k(  p_memman, p_ctl, sizeof( struct SHTCTL));
err:	
	return p_ctl;
	
}

struct SHEET *sheet_alloc( struct SHTCTL *p_ctl)
{
    struct SHEET *p_sht;
    int i;
    for( i = 0; i < MAX_SHEETS; i++)
    {
        if(  p_ctl->arr_sheets[i].flags == 0)
        {
            p_sht = p_ctl->arr_sheets + i;
            p_sht->flags = SHEET_USE;
            p_sht->height = -1; //?錟
            return p_sht;
            
        }
        
    }
    
    return 0;
    
    
}

void sheet_setbuf( struct SHEET *p_sht, uint8_t *buf, int bxsize, int bysize, int col_inv)
{
    p_sht->buf = buf;
    p_sht->bxsize = bxsize;
    p_sht->bysize = bysize;
    p_sht->col_inv = col_inv;
    return;
    
}

void sheet_updown( struct SHEET *p_sht, int height)
{
    int h, old = p_sht->height;
    struct SHTCTL *p_ctl = p_sht->p_shtctl;
    
    //?惓旕朄揑?抲崅搙?
    if( height > p_ctl->top + 1)
    {
        height = p_ctl->top + 1;
    }
    
    if( height < -1)
    {
        height = -1;
    }
    p_sht->height = height;
    
    //埪徠崅搙丆廳怴攔楍p_arr_sheets   
    //攔彉揑嶼朄桳揰憸樃?壦忋???彉丆扅惀曽岦晄摨帶涍
    if( old > height)
    {
        if( height > 0)
        {
            for( h = old; h > height; h--)
            {
                p_ctl->arr_p_sheets[h] = p_ctl->arr_p_sheets[ h - 1];
                p_ctl->arr_p_sheets[h]->height = h;
            }
            
            p_ctl->arr_p_sheets[height] = p_sht;
            
            sheet_refreshmap( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bxsize,  p_sht->vy0 + p_sht->bysize, height + 1);
            sheet_refreshsub( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bxsize,  p_sht->vy0 + p_sht->bysize, height + 1, old);
            
        }
        else        
        {
            if( p_ctl->top > old) 
            {
                
                //?崅搙擵忋揑??壓堏
                for( h = old; h < p_ctl->top; h ++)
                {
                    p_ctl->arr_p_sheets[h] = p_ctl->arr_p_sheets[ h + 1];
                    p_ctl->arr_p_sheets[h]->height = h;
                }
                p_ctl->top --;
                
            }
            
            sheet_refreshmap( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bxsize,  p_sht->vy0 + p_sht->bysize, 0);
            sheet_refreshsub( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bxsize,  p_sht->vy0 + p_sht->bysize, 0, old - 1);
            
        }
        
       // sheet_refresh( p_ctl);
       
        
    }
    else if( old < height)
    {
        if( old >= 0)
        {
            for( h = old; h < height; h ++)
            {
                p_ctl->arr_p_sheets[h] = p_ctl->arr_p_sheets[ h + 1];
                p_ctl->arr_p_sheets[h]->height = h;
            }
            p_ctl->arr_p_sheets[height] = p_sht;
        }
        else //桼?錟忬??摓?帵忬?
        {
            for( h = p_ctl->top; h >= height; h--)
            {
                p_ctl->arr_p_sheets[h + 1] = p_ctl->arr_p_sheets[ h];
                p_ctl->arr_p_sheets[h + 1]->height = h + 1;
            }
            p_ctl->arr_p_sheets[height] = p_sht;
            p_ctl->top ++;
        }
        
        //sheet_refresh( p_ctl);
        sheet_refreshmap( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bxsize,  p_sht->vy0 + p_sht->bysize, height);
        sheet_refreshsub( p_ctl, p_sht->vx0, p_sht->vy0, p_sht->vx0 + p_sht->bxsize,  p_sht->vy0 + p_sht->bysize,height, height);
        
    }
    
    return;
    
}
#if 0
void sheet_refresh(  struct SHEET *p_sht)
{
    int h, bx, by, vx, vy;
    uint8_t *buf, c, *vram = p_ctl->vram;
    struct SHEET *p_sht;
    struct SHTCTL *p_ctl = p_sht->p_shtctl;
    for( h = 0; h <= p_ctl->top; h++)
    {
        p_sht = p_ctl->arr_p_sheets[h];
        buf = p_sht->buf;
        
        for( by = 0; by < p_sht->bysize; by ++)
        {
            vy = p_sht->vy0 + by;
            for( bx= 0; bx < p_sht->bxsize; bx ++)
            {
                vx = p_sht->vx0 + bx;
                c = buf[ by * p_sht->bxsize + bx];
                
                //扅桳梌摟柧?怓晄摨揑?怓嵥夛?惂摓夋柺忋
                if( c != p_sht->col_inv)
                {
                    vram[ vy * p_ctl->xsize + vx] = c;
                }
            }
            
        }
    }
    
    

    
    return;
}

#endif
void sheet_refresh( struct SHEET *p_sht, int bx0, int by0, int bx1, int by1)
{
    struct SHTCTL *p_ctl = p_sht->p_shtctl;
    if( p_sht->height >=0)
    {
        
        sheet_refreshsub( p_ctl, p_sht->vx0 + bx0, p_sht->vy0 + by0, p_sht->vx0 + bx1, p_sht->vy0 + by1, p_sht->height,  p_sht->height);
    }
    
}

void sheet_slide( struct SHEET *p_sht, int vx0, int vy0)
{
    struct SHTCTL *p_ctl = p_sht->p_shtctl;
    int old_vx0 = p_sht->vx0, old_vy0 = p_sht->vy0;
    p_sht->vx0 = vx0;
    p_sht->vy0 = vy0;
    
    if( p_sht->height >= 0)
    {
        sheet_refreshmap( p_ctl, old_vx0, old_vy0,old_vx0 + p_sht->bxsize, old_vy0 + p_sht->bysize, 0);
        sheet_refreshmap( p_ctl, vx0, vy0,vx0 + p_sht->bxsize, vy0 + p_sht->bysize,   p_sht->height);

        sheet_refreshsub( p_ctl, old_vx0, old_vy0,old_vx0 + p_sht->bxsize, old_vy0 + p_sht->bysize, 0,  p_sht->height - 1);
        sheet_refreshsub( p_ctl, vx0, vy0,vx0 + p_sht->bxsize, vy0 + p_sht->bysize,  p_sht->height ,  p_sht->height);

        
    }
    
    return;
    
}

void sheet_free( struct SHEET *p_sht)
{
    struct SHTCTL *p_ctl = p_sht->p_shtctl;
    if( p_sht->height >= 0)
    {
        sheet_updown(  p_sht, -1);
        
    }
    p_sht->flags = 0;
    return;
    
}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void sheet_refreshsub( struct SHTCTL *p_ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
    int h, bx, by, vx, vy, bx0, bx1, by0, by1;
    uint8_t *buf, *vram = p_ctl->vram, *map = p_ctl->map;
    struct SHEET *p_sht;
    uint8_t  c, side;
    

    if( vx0 < 0) { vx0 = 0;}
    if( vy0 < 0) { vy0 = 0;}
    if( vx1 > p_ctl->xsize) { vx1 = p_ctl->xsize;}
    if( vy1 > p_ctl->ysize) { vy1 = p_ctl->ysize;}
    
    for( h = h0; h <= h1; h++)
    {
        p_sht = p_ctl->arr_p_sheets[h];
        buf = p_sht->buf;
        
        side = p_sht - p_ctl->arr_sheets; 
        
        //倒退需要显示的范围
        bx0 = vx0 - p_sht->vx0;
        by0 = vy0 - p_sht->vy0;
        bx1 = vx1 - p_sht->vx0;
        by1 = vy1 - p_sht->vy0;
        
        if( bx0 < 0) { bx0 = 0;}
        if( by0 < 0) { by0 = 0;}
        if( bx1 > p_sht->bxsize) { bx1 = p_sht->bxsize;}
        if( by1 > p_sht->bysize) { by1 = p_sht->bysize;}
        
           
        
        for( by = by0; by < by1; by ++)
        {
            vy = p_sht->vy0 + by;
            for( bx = bx0; bx < bx1; bx ++)
            {
                vx = p_sht->vx0 + bx;
                
                c = buf[ by * p_sht->bxsize + bx];
                
                // if( c != p_sht->col_inv)
                if( map[  vy * p_ctl->xsize + vx] == side)
                {
                    vram[ vy * p_ctl->xsize + vx] = c;
                }
                
            }
            
        }
    }
    
    
}

static void sheet_refreshmap( struct SHTCTL *p_ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
    int h, bx, by, vx, vy, bx0, bx1, by0, by1;
    uint8_t *buf,  *vram = p_ctl->vram, *map = p_ctl->map;
    struct SHEET *p_sht;
    uint8_t c, side;
    
    if( vx0 < 0) { vx0 = 0;}
    if( vy0 < 0) { vy0 = 0;}
    if( vx1 > p_ctl->xsize) { vx1 = p_ctl->xsize;}
    if( vy1 > p_ctl->ysize) { vy1 = p_ctl->ysize;}
    
    for( h = h0; h <= p_ctl->top; h++)
    {
        p_sht = p_ctl->arr_p_sheets[h];
        buf = p_sht->buf;
        
        side = p_sht - p_ctl->arr_sheets;       //计算的图层号  这个计算方法是什么原理？
        
        bx0 = vx0 - p_sht->vx0;
        by0 = vy0 - p_sht->vy0;
        bx1 = vx1 - p_sht->vx0;
        by1 = vy1 - p_sht->vy0;
       
        
        if( bx0 < 0) { bx0 = 0;}
        if( by0 < 0) { by0 = 0;}
        if( bx1 > p_sht->bxsize) { bx1 = p_sht->bxsize;}
        if( by1 > p_sht->bysize) { by1 = p_sht->bysize;}
        
        
        
        for( by = by0; by < by1; by ++)
        {
            vy = p_sht->vy0 + by;
            for( bx = bx0; bx < bx1; bx ++)
            {
                vx = p_sht->vx0 + bx;
                
                c = buf[ by * p_sht->bxsize + bx];
                
                if( c != p_sht->col_inv)
                {
                    map[ vy * p_ctl->xsize + vx] = side;
                }
                
            }
            
        }
    }
    
    
}

















