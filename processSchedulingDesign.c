#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READY 0
#define RUNNING 1
#define TIMESLICE 2

typedef struct _PCB
{
	int pid;
	int state;
	int time;
	struct _PCB *next;
}PCB, *pPCB;

void InitReadyQueue(pPCB readyQueue_head, int num_process); //初始化就绪队列
void FCFSRunningPCB(pPCB runningQueue_head); //运行队列按FCFS运行进程块
void FCFSFinishRunningPCB(pPCB readyQueue_head, pPCB runningQueue_head); //进程块运行完成，释放进程块或把进程块放回就绪队列
void FCFSTransferPCB(pPCB readyQueue_head, pPCB runningQueue_head); //从就绪队列取出要运行的进程块，搬到运行队列中
int IsEmptyQueue(PCB Queue_head); //队列是否为空
int InitFindMinTime(pPCB readyQueue_head); //初始化进程块的最短运行时间
pPCB FindMinTimePCB(pPCB readyQueue_head, int min_time); //找到运行时间最短的进程块
void SPFTransferPCB(pPCB r, pPCB readyQueue_head, pPCB runningQueue_head); //从就绪队列取出要运行的进程块，搬到运行队列中
void SPFRunningPCB(pPCB runningQueue_head); //运行队列按SPF运行进程块
void SPFFinishRunningPCB(pPCB l, pPCB readyQueue_head, pPCB runningQueue_head); //进程块运行完成，释放进程块或把进程块放回就绪队列

int main()
{
	int i;
	int flag_readyQueue;
	int flag_runningQueue;
	int min_time; //最短运行时间
	int num_process; //进程个数
	int count_SPF; //第几次运行SPF
	char process_func[5]; //调度方式

	pPCB p;
	pPCB q;
	pPCB r;
	pPCB l;

	PCB readyQueue_head; //就绪队列的头结点
	PCB runningQueue_head; //运行队列的头结点

	scanf("%s %d", process_func, &num_process); //输入调度方式，进程个数

	InitReadyQueue(&readyQueue_head, num_process); //初始化就绪队列

	if (!strcmp("fcfs", process_func))// FCFS
	{
		printf("fcfs order:");
		while (!(flag_readyQueue = IsEmptyQueue(readyQueue_head))) //若就绪队列不空，则说明还有进程块待运行
		{
			FCFSTransferPCB(&readyQueue_head, &runningQueue_head); //从就绪队列取出要运行的进程块，搬到运行队列中
			FCFSRunningPCB(&runningQueue_head); //运行进程块
			FCFSFinishRunningPCB(&readyQueue_head, &runningQueue_head); //进程块运行完成，释放进程块或把进程块放回就绪队列
		}
	}
	else //SPF
	{
		printf("spf order:");
		count_SPF = 0;
		while (!(flag_runningQueue = IsEmptyQueue(readyQueue_head))) //若就绪队列不空，则说明还有进程块待运行
		{
			//(第一次使用SDF)将就绪队列中，运行时间最少的进程块提取出来，插入到运行队列
			if (count_SPF == 0) //第一次使用SDF
			{
				min_time = InitFindMinTime(&readyQueue_head); //初始化进程块的最短运行时间
				r = FindMinTimePCB(&readyQueue_head, min_time); //找到运行时间最短的进程块
				SPFTransferPCB(r, &readyQueue_head, &runningQueue_head);  //从就绪队列取出要运行的进程块，搬到运行队列中
			}
			else
			{
				SPFTransferPCB(r, &readyQueue_head, &runningQueue_head); //从就绪队列取出要运行的进程块，搬到运行队列中
			}
			//运行进程块
			SPFRunningPCB(&runningQueue_head); //运行进程块

			l = r;

			//在运行队列中的进程块没有返回就绪队列之前，要在就绪队列里面重新选一个运行时间最短的
			if (!(flag_runningQueue = IsEmptyQueue(readyQueue_head))) //就绪队列不为空才能在里面选，否则进程块还是为r
			{
				min_time = InitFindMinTime(&readyQueue_head); //初始化进程块的最短运行时间
				r = FindMinTimePCB(&readyQueue_head, min_time); //找到运行时间最短的进程块
			}
			SPFFinishRunningPCB(l, &readyQueue_head, &runningQueue_head); //进程块运行完成，释放进程块或把进程块放回就绪队列
			count_SPF++;
		}
	}
	return 0;
}

void InitReadyQueue(pPCB readyQueue_head, int num_process) //初始化就绪队列
{
	pPCB r = readyQueue_head;
	for (int i = 0; i < num_process; i++) //就绪队列初始化
	{
		pPCB p = malloc(sizeof(PCB));

		p->pid = i;
		p->state = READY;
		scanf("%d", &p->time); //给进程块赋值

		r->next = p;
		p->next = NULL;
		r = p;
	}
	return readyQueue_head;
}

void FCFSRunningPCB(pPCB runningQueue_head) //运行队列运行进程块
{
	pPCB r = runningQueue_head->next;
	r->state = RUNNING; //该进程块的状态由“就绪”改为“运行”
	if (r->time >= 2) //若该进程块的(剩余)运行时间大于时间片，则(剩余)运行时间-2
		r->time -= 2;
	else //若该进程块的(剩余)运行时间小于时间片，则(剩余)运行时间直接赋为0
		r->time = 0;
	printf("P0%d ", r->pid);
}

void FCFSFinishRunningPCB(pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB r = runningQueue_head->next;
	pPCB p = readyQueue_head;
	if (r->time == 0) //该进程块运行完成，释放
		free(r);
	else
	{
		r->state = READY; //该进程块的状态由“运行”改为“就绪”

		//进程块插入到就绪队列末尾
		if (readyQueue_head->next == NULL) //就绪队列只剩头结点
			readyQueue_head->next = r;
		else //就绪队列除了头结点还有其他进程块
			for (p; p != NULL; p = p->next)
				if (p->next == NULL)
				{
					p->next = r;
					break;
				}
	}
	runningQueue_head->next = NULL; //运行队列删去运行完成的进程块
}

void FCFSTransferPCB(pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB r = readyQueue_head->next;
	readyQueue_head->next = r->next; //就绪队列删除第一个进程块

	runningQueue_head->next = r; //将该进程块插入到运行队列
	r->next = NULL;
}

int IsEmptyQueue(PCB Queue_head)
{
	if (Queue_head.next == NULL)
		return 1;
	else
		return 0;
}

int InitFindMinTime(pPCB readyQueue_head)
{
	int min_time;
	min_time = readyQueue_head->next->time;
	return min_time;
}

pPCB FindMinTimePCB(pPCB readyQueue_head, int min_time)
{
	pPCB p = readyQueue_head->next;
	pPCB r;
	for (p; p != NULL; p = p->next) //找出就绪队列中各进程块最短的运行时间
		if (p->time < min_time)
			min_time = p->time;

	for (r = readyQueue_head->next; r != NULL; r = r->next) //将就绪队列中运行时间最少的进程块提取出来，进程块为r
		if (r->time == min_time)
			break;
	return r;
}

void SPFTransferPCB(pPCB r, pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB p;
	pPCB q;
	p = readyQueue_head;
	q = readyQueue_head->next;
	for (q; q != NULL; q = q->next) //就绪队列中删去进程块r
	{
		if (q == r)
		{
			p->next = q->next;
			break;
		}
		p = p->next;
	}
	runningQueue_head->next = r; //将进程块r插入到运行队列中
	r->next = NULL;
}

void SPFRunningPCB(pPCB runningQueue_head)
{
	pPCB r;
	r = runningQueue_head->next;
	r->state = RUNNING; //该进程块的状态由“就绪”改为“运行”
	if (r->time >= 2) //若该进程块的(剩余)运行时间大于时间片，则(剩余)运行时间-2
		r->time -= 2;
	else //若该进程块的(剩余)运行时间小于时间片，则(剩余)运行时间直接赋为0
		r->time = 0;
	printf("P0%d ", r->pid);
}

void SPFFinishRunningPCB(pPCB l, pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB p;
	p = readyQueue_head;

	if (l->time == 0) //该进程块运行完成，释放
		free(l);
	else
	{
		l->state = READY; //该进程块的状态由“运行”改为“就绪”

		//进程块插入到就绪队列末尾
		if (readyQueue_head->next == NULL) //就绪队列只剩头结点
			readyQueue_head->next = l;
		else //就绪队列除了头结点还有其他进程块
			for (p; p != NULL; p = p->next)
				if (p->next == NULL)
				{
					p->next = l;
					break;
				}
	}
	runningQueue_head = NULL;
}