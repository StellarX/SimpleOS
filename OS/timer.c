/* ^C}ÖW */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC		1	/* ßzuó? */
#define TIMER_FLAGS_USING		2	/* è?í?s */

void init_pit(void)
{
	int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timer[i].flags = 0; /* ¢gp */
	}
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timer[i].flags == 0) {
			timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timer[i];
		}
	}
	return 0; /* vQ */
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0; /* ¢gp */
	return;
}

void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data)
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
{
	timer->timeout = timeout;
	timer->flags = TIMER_FLAGS_USING;//2
	return;
}

void inthandler20(int *esp)
{
	io_out8(PIC0_OCW2, 0x60);	/* cIRQ-00MÚ¾®¹IM§Êm?PIC */
	timerctl.count++;
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timer[i].flags == TIMER_FLAGS_USING) {
			timerctl.timer[i].timeout--;
			if (timerctl.timer[i].timeout == 0) {
				timerctl.timer[i].flags = TIMER_FLAGS_ALLOC;//ø?zuó?CA?s?ú
				fifo8_put(timerctl.timer[i].fifo, timerctl.timer[i].data);
			}
		}
	}
	return;
}
/*
void settimer(unsigned int timeout, struct FIFO8 *fifo, unsigned char data)
{//@Ê?è?vL®S?©IRQ0IfA?I?CïøN¬CÈä?æÖ~fCR@®¬?èCÅ@Äcfó??´
    int eflags;
    eflags = io_load_eflags();
    io_cli();
    timerctl.timeout = timeout;
    timerctl.fifo = fifo;
    timerctl.data = data;
    io_store_eflags(eflags);
    return;
}*/

