/* 多任务相关 */

#include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;//任务切换间隔时间定时器

struct TASK *task_now(void)//返回现在活动中的struct TASK的内存地址
{
    struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];//先取得当前活动的层级
    return tl->tasks[tl->now];
}

void task_add(struct TASK *task)//向struct TASKLEVEL中添加一个任务
{
    struct TASKLEVEL *tl = &taskctl->level[task->level];
    tl->tasks[tl->running] = task;//添加到最后
    tl->running++;
    task->flags = 2; /*活动中*/
    return;
}

void task_remove(struct TASK *task)//从struct TASKLEVEL中删除一个任务
{
    int i;
    struct TASKLEVEL *tl = &taskctl->level[task->level];

    /*寻找task所在的位置*/
    for (i = 0; i < tl->running; i++) {
        if (tl->tasks[i] == task) {
            /*在这里  */
            break;
        }
    }

    tl->running--;
    if (i < tl->now) {
        tl->now--; /*需要移动成员，要相应地处理  */
    }
    if (tl->now >= tl->running) {
        /*如果now的值出现异常，则进行修正*/
        tl->now = 0;
    }
    task->flags = 1; /* 休眠中 */

    /* 移动 */
    for (; i < tl->running; i++) {
        tl->tasks[i] = tl->tasks[i + 1];
    }

    return;
}

void task_switchsub(void)//在任务切换时决定接下来切换到哪个LEVEL
{
    int i;
    /*寻找最上层的LEVEL */
    for (i = 0; i < MAX_TASKLEVELS; i++) {//从最高层级开始找
        if (taskctl->level[i].running > 0) {
            break; /*找到了*/
        }
    }
    taskctl->now_lv = i;
    taskctl->lv_change = 0;//在下次任务切换时是否需要改变LEVEL
    return;
}

void task_idle(void)//闲置的任务
{
    for (;;) {
        io_hlt();
    }
}

struct TASK *task_init(struct MEMMAN *memman)
{
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	//由于struct TASKCTL是一个很庞大的结构，因此我们让它从memman_alloc来申请内存空间
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;//从gdt里选择哪个编号的段
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);//段长限制为103字节
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
        taskctl->level[i].running = 0;
        taskctl->level[i].now = 0;
    }
	task = task_alloc();
	task->flags = 2; /* 活动中标志 */
	task->priority = 2; /* 0.02秒  */
	task->level = 0;    /*最高LEVEL */
	task_add(task);
    task_switchsub();   /* LEVEL 设置*/
	load_tr(task->sel);////任务寄存器，表示当前在运行哪个任务
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);
	
	idle = task_alloc();
    idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
    idle->tss.eip = (int) &task_idle;
    idle->tss.es = 1 * 8;
    idle->tss.cs = 2 * 8;
    idle->tss.ss = 1 * 8;
    idle->tss.ds = 1 * 8;
    idle->tss.fs = 1 * 8;
    idle->tss.gs = 1 * 8;
    task_run(idle, MAX_TASKLEVELS - 1, 1);
	
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

void task_run(struct TASK *task, int level, int priority)//准备运行，注意这里也还没有开始运行
{
	if (level < 0) {
        level = task->level; /*不改变LEVEL */
    }
	if (priority > 0) {//为0时则表示不改变当前已经设定的优先级,这个主要是为了唤醒休眠任务时
        task->priority = priority;
    }
	if (task->flags == 2 && task->level != level) { /*改变活动中的LEVEL 要先移出当前level*/
        task_remove(task); /*这里执行之后flag的值会变为1，于是下面的if语句块也会被执行*/
    }
	if (task->flags != 2) {
        /*从休眠状态唤醒的情形*/
        task->level = level;
        task_add(task);
    }
	taskctl->lv_change = 1; /*下次任务切换时需要检查LEVEL */
	return;
}

void task_sleep(struct TASK *task)
{
    struct TASK *now_task;
    if (task->flags == 2) {
        /*如果处于活动状态*/
        now_task = task_now();
        task_remove(task); /*执行此语句的话flags将变为1  */
        if (task == now_task) {
            /*如果是让自己休眠，则需要进行任务切换*/
            task_switchsub();
            now_task = task_now(); /*在设定后获取当前任务的值*/
            farjmp(0, now_task->sel);
        }
    }
    return;
}

void task_switch(void)
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
    struct TASK *new_task, *now_task = tl->tasks[tl->now];
    tl->now++;
    if (tl->now == tl->running) {
        tl->now = 0;
    }
    if (taskctl->lv_change != 0) {
        task_switchsub();//检查层级
        tl = &taskctl->level[taskctl->now_lv];
    }
    new_task = tl->tasks[tl->now];
    timer_settime(task_timer, new_task->priority);
    if (new_task != now_task) {
        farjmp(0, new_task->sel);
    }
    return;
}
