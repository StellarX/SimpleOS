/* 多任务相关 */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	//由于struct TASKCTL是一个很庞大的结构，因此我们让它从memman_alloc来申请内存空间
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;//从gdt里选择哪个编号的段
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);//段长限制为103字节
	}
	task = task_alloc();
	task->flags = 2; /* 活动中标志 */
	taskctl->running = 1;
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	load_tr(task->sel);////任务寄存器，表示当前在运行哪个任务
	task_timer = timer_alloc();
	timer_settime(task_timer, 2);
	return task;
}

struct TASK *task_alloc(void)//任务分配，也就是TSS段的初始化等等，注意此时并没有运行任务
{
	int i;
	struct TASK *task;
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1; /* 正在使用的标志 */
			task->tss.eflags = 0x00000202; /* IF = 1; */
			task->tss.eax = 0; /* 这里先置为0 */
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; /* 没有空闲，全部在使用 */
}

void task_run(struct TASK *task)//准备运行，注意这里也还没有开始运行
{
	task->flags = 2; /* 活动中 */
	taskctl->tasks[taskctl->running] = task;//将task添加到tasks的末尾
	taskctl->running++;
	return;
}

void task_switch(void)
{
	timer_settime(task_timer, 2);
	if (taskctl->running >= 2) {
		taskctl->now++;//当前运行的任务  
		if (taskctl->now == taskctl->running) {
			taskctl->now = 0;//这里实际上是到了最后一个任务就又切回第一个
		}
		farjmp(0, taskctl->tasks[taskctl->now]->sel);
	}
	return;
}
