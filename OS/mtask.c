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

void task_sleep(struct TASK *task)
{
    int i;
    char ts = 0;//task_switch
    if (task->flags == 2) {		/*如果指定任务处于唤醒状态*/
        if (task == taskctl->tasks[taskctl->now]) {
            ts = 1; /*让自己休眠的话，稍后需要进行任务切换*/
        }
        /*寻找task所在的位置*/
        for (i = 0; i < taskctl->running; i++) {
            if (taskctl->tasks[i] == task) {
                /*在这里*/
                break;
            }
        }
        taskctl->running--;
        if (i < taskctl->now) {
            taskctl->now--; /*需要移动成员，要相应地处理*/
        }
        /*移动成员*/
        for (; i < taskctl->running; i++) {
            taskctl->tasks[i] = taskctl->tasks[i + 1];
        }
        task->flags = 1; /*不工作的状态*/
        if (ts != 0) {
            /*任务切换*/
            if (taskctl->now >= taskctl->running) {
                /*如果now的值出现异常，则进行修正*/
                taskctl->now = 0;
            }
            farjmp(0, taskctl->tasks[taskctl->now]->sel);
        }
    }
    return;
}


