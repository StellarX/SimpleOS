/* タイマ関係 */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC		1	/* 已配置状态 */
#define TIMER_FLAGS_USING		2	/* 定时器运行中 */

void init_pit(void)
{
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;

	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timer0[i].flags = 0; /* 未使用 */
	}
	t = timer_alloc(); /* 取得一个 */
    t->timeout = 0xffffffff;
    t->flags = TIMER_FLAGS_USING;
    t->next = 0; /* 末尾 */
    timerctl.t0 = t; /* 因为现在只有哨兵，所以他就在最前面*/
    timerctl.next = 0xffffffff; /* 因为只有哨兵，所以下一个超时时刻就是哨兵的时刻 */
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timer0[i].flags == 0) {
			timerctl.timer0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timer0[i];
		}
	}
	return 0; /* 没找到 */
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0; /* 未使用 */
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;//2
	e = io_load_eflags();
    io_cli();
	t = timerctl.t0;
    if (timer->timeout <= t->timeout) {
        /* 插入最前面的情况下  类似与单链表的头插法*/
        timerctl.t0 = timer;
        timer->next = t; /* 下面是t */
        timerctl.next = timer->timeout;
        io_store_eflags(e);
        return;
    }
    /* 搜寻插入位置 */
    for (;;) {
        s = t;
        t = t->next;
        if (timer->timeout <= t->timeout) {
            /* 插入到s和t之间时  类似于插入单链表的中间*/
            s->next = timer; /* s的下一个是timer  */
            timer->next = t; /* timer的下一个是t */
            io_store_eflags(e);
            return;
        }
    }
}

void inthandler20(int *esp)//定时器中断程序，1s会自动产生100次中断，也就是自动执行这个程序
{
	char ts = 0;//mt_timer超时标记
	struct TIMER *timer;
	io_out8(PIC0_OCW2, 0x60);	/* 把IRQ-00信号接收完了的信息通知给PIC */
	timerctl.count++;
	
	if (timerctl.next > timerctl.count) {
        return; /* 还不到下一个时刻，所以结束*/
    }
	
	timer = timerctl.t0; /* 首先把最前面的地址赋给timer */
	for (;;) {
        /* timers的定时器都处于动作中，所以不确认flags */
        if (timer->timeout > timerctl.count) {
            break;
        }
        /* 超时*/
        timer->flags = TIMER_FLAGS_ALLOC;
        if (timer != mt_timer) {
            fifo32_put(timer->fifo, timer->data);
        } else {
            ts = 1; /* mt_timer超时*/
        }
        timer = timer->next; /*将下一个计时器的地址赋给timer */
    }
	/* 新移位 */
    timerctl.t0 = timer;
	/* timerctl.next的设定 */
	timerctl.next = timerctl.t0->timeout; 
	if (ts != 0) {
        mt_taskswitch();
    }
	return;
}

