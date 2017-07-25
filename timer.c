
#include "bootpack.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define PIT_CTRL        0x0043
#define PIT_CNT0        0x0040

#define TIMER_FLAGS_ALLOC           1  //������
#define TIMER_FLAGS_USING           2   //������
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
struct TIMERCTL  timerctl;

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
void init_pit( void)
{
    int i ;
    struct TIMER *p;
    io_out8( PIT_CTRL, 0x34);
    io_out8( PIT_CNT0, 0x9c);
    io_out8( PIT_CNT0, 0x2e);
    timerctl.count = 0;
    timerctl.nextTimeout = 0xffffffff;
    for( i = 0; i < MAX_TIMER; i++)
    {
        timerctl.arr_timer[i].flags = 0;
        
    }
    
    //����һ����Ϊ�ڱ�
    p = timer_alloc();
    p->timeout =0xffffffff;
    p->next_p_timer = 0;
    timerctl.p_timerHead = p;
    return;
}
struct TIMER *timer_alloc( void)
{
    int i;
    for( i = 0; i < MAX_TIMER; i++)
    {
       if( timerctl.arr_timer[i].flags == 0)
       {
           timerctl.arr_timer[i].flags = TIMER_FLAGS_ALLOC;
           return timerctl.arr_timer + i;
           
       }
        
    }
    
    return 0;
    
}

void timer_free( struct TIMER *p_timer)
{
    p_timer->flags = 0;
    return;
    
}

void timer_init( struct TIMER *p_timer, struct FIFO32 *p_fifo, int data)
{
    p_timer->p_fifo = p_fifo;
    p_timer->data = data;
    return ;
    
}

void timer_settime( struct TIMER *p_timer, uint32_t timeout)
{
    struct TIMER *now_timer, *next_timer;
    int e, i, j;
    p_timer->timeout = timerctl.count + timeout;
    p_timer->flags = TIMER_FLAGS_USING;
    
    e = io_load_eflags();
    io_cli();
    
    next_timer = timerctl.p_timerHead;
     //���뵽����ͷ�������
    if( p_timer->timeout <= timerctl.p_timerHead->timeout)
    {
        timerctl.p_timerHead = p_timer;
        p_timer->next_p_timer = next_timer;
        timerctl.nextTimeout = timeout;
        io_store_eflags( e);
        return; 
    }
    
    //�ҵ��ŵ�ǰ�����ʱ����λ�ã�����һ���ȵ�ǰ�ĳ�ʱʱ����λ��
    // now_timer = timerctl.p_timerHead;
    // next_timer = now_timer->next_p_timer;
    for( ;;)
    {
        now_timer = next_timer;
        next_timer = next_timer->next_p_timer;
        //��>= ��Ϊ�˷�ֹ��timeout == 0xffffffff ʱ���ڱ�ʧЧ
        if( p_timer->timeout <= next_timer->timeout )
        {
            now_timer->next_p_timer = p_timer;
            p_timer->next_p_timer = next_timer;
            io_store_eflags( e);
            return;
        }
           
        
    }
    

    
	
	
	
	
    
}


void inthandler20( int *esp)
{
    struct TIMER *p_timer;
    short i, j;
    io_out8( PIC0_OCW2 , 0x60);
    
    timerctl.count ++;
    if( timerctl.nextTimeout > timerctl.count )
        return;
    
    p_timer = timerctl.p_timerHead;
    for(;;)
    {
        //��δ��ʱ�ģ�˵Ҫ֮��Ķ�������ֳ�ʱ�ˣ����˳�ѭ����
        if( p_timer->timeout > timerctl.count) 
            break;
        p_timer->flags = TIMER_FLAGS_ALLOC;
        fifo32_put( p_timer->p_fifo, p_timer->data);
        p_timer = p_timer->next_p_timer;
    }
   
    
    
    timerctl.p_timerHead = p_timer;
    timerctl.nextTimeout = p_timer->timeout;

    
    return;
    
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
