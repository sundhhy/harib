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

struct TIMER *p_mt_timer;
struct TASKCTL  *p_taskctl;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define TASK_USE        1
#define TASK_RUN        2
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
struct TASK *Task_init( struct MEMMAN *p_mem)
{
    int i;
    struct TASK *p_tk;
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
    
    p_taskctl =  memman_alloc_4k( p_mem, sizeof( struct TASKCTL));  
    
    for( i = 0; i < MAX_TASKS; i++)
    {
        
        p_taskctl->arr_tasks[i].flags = 0;
        p_taskctl->arr_tasks[i].sel = ( TASK_GDT0 + i) * 8;
        set_segmdesc( gdt + TASK_GDT0 + i, 103, (int )&p_taskctl->arr_tasks[i].sel, AR_TSS32);
    }
    
    p_tk = Task_alloc();
    p_tk->flags = TASK_RUN;
    
    p_taskctl->numRunning = 1;
    p_taskctl->now = 0;
    p_taskctl->arr_p_tasks[0] = p_tk;
    load_tr( p_tk->sel);
    p_mt_timer = timer_alloc();
    timer_settime( p_mt_timer, 2);
    
    return p_tk;
    
}

struct TASK *Task_alloc( void)
{
    
    int i;
    struct TASK *p_tk;
    
    for( i = 0; i < MAX_TASKS; i++)
    {
        if( p_taskctl->arr_tasks[i].flags == 0)
        {
            p_tk = &p_taskctl->arr_tasks[i];         
            p_tk->flags = TASK_USE;
            
            p_tk->tss.eflags = 0x00000202; //IF = 1
            p_tk->tss.eax = 0;
            p_tk->tss.ecx = 0;
            p_tk->tss.edx = 0;
            p_tk->tss.ebx = 0;
            p_tk->tss.esp = 0;
            p_tk->tss.ebp = 0;
            p_tk->tss.esi = 0;
            p_tk->tss.edi = 0;
            p_tk->tss.es = 0;
            p_tk->tss.cs = 0;
            p_tk->tss.ss = 0;
            p_tk->tss.ds = 0;
            p_tk->tss.fs = 0;
            p_tk->tss.gs = 0;
            p_tk->tss.ldtr = 0;
            p_tk->tss.iomap = 0x40000000;
            
            return  p_tk;
        }
       
    }
    
    return 0;
    
}

void Task_run( struct TASK *p_task)
{
    p_task->flags = TASK_RUN;
    p_taskctl->arr_p_tasks[ p_taskctl->numRunning] = p_task;
    p_taskctl->numRunning ++;
    return;
    
    
}

void Task_switch(void)
{
    timer_settime( p_mt_timer, 2);
    
    if( p_taskctl->numRunning >= 2)
    {
        p_taskctl->now ++;
        if( p_taskctl->now ==  p_taskctl->numRunning)
        {
            p_taskctl->now = 0;
        }
        farjmp(0, p_taskctl->arr_p_tasks[ p_taskctl->now]->sel );
        
    }
    
    return;
    
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{













