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

void InitReadyQueue(pPCB readyQueue_head, int num_process); //��ʼ����������
void FCFSRunningPCB(pPCB runningQueue_head); //���ж��а�FCFS���н��̿�
void FCFSFinishRunningPCB(pPCB readyQueue_head, pPCB runningQueue_head); //���̿�������ɣ��ͷŽ��̿��ѽ��̿�Żؾ�������
void FCFSTransferPCB(pPCB readyQueue_head, pPCB runningQueue_head); //�Ӿ�������ȡ��Ҫ���еĽ��̿飬�ᵽ���ж�����
int IsEmptyQueue(PCB Queue_head); //�����Ƿ�Ϊ��
int InitFindMinTime(pPCB readyQueue_head); //��ʼ�����̿���������ʱ��
pPCB FindMinTimePCB(pPCB readyQueue_head, int min_time); //�ҵ�����ʱ����̵Ľ��̿�
void SPFTransferPCB(pPCB r, pPCB readyQueue_head, pPCB runningQueue_head); //�Ӿ�������ȡ��Ҫ���еĽ��̿飬�ᵽ���ж�����
void SPFRunningPCB(pPCB runningQueue_head); //���ж��а�SPF���н��̿�
void SPFFinishRunningPCB(pPCB l, pPCB readyQueue_head, pPCB runningQueue_head); //���̿�������ɣ��ͷŽ��̿��ѽ��̿�Żؾ�������

int main()
{
	int i;
	int flag_readyQueue;
	int flag_runningQueue;
	int min_time; //�������ʱ��
	int num_process; //���̸���
	int count_SPF; //�ڼ�������SPF
	char process_func[5]; //���ȷ�ʽ

	pPCB p;
	pPCB q;
	pPCB r;
	pPCB l;

	PCB readyQueue_head; //�������е�ͷ���
	PCB runningQueue_head; //���ж��е�ͷ���

	scanf("%s %d", process_func, &num_process); //������ȷ�ʽ�����̸���

	InitReadyQueue(&readyQueue_head, num_process); //��ʼ����������

	if (!strcmp("fcfs", process_func))// FCFS
	{
		printf("fcfs order:");
		while (!(flag_readyQueue = IsEmptyQueue(readyQueue_head))) //���������в��գ���˵�����н��̿������
		{
			FCFSTransferPCB(&readyQueue_head, &runningQueue_head); //�Ӿ�������ȡ��Ҫ���еĽ��̿飬�ᵽ���ж�����
			FCFSRunningPCB(&runningQueue_head); //���н��̿�
			FCFSFinishRunningPCB(&readyQueue_head, &runningQueue_head); //���̿�������ɣ��ͷŽ��̿��ѽ��̿�Żؾ�������
		}
	}
	else //SPF
	{
		printf("spf order:");
		count_SPF = 0;
		while (!(flag_runningQueue = IsEmptyQueue(readyQueue_head))) //���������в��գ���˵�����н��̿������
		{
			//(��һ��ʹ��SDF)�����������У�����ʱ�����ٵĽ��̿���ȡ���������뵽���ж���
			if (count_SPF == 0) //��һ��ʹ��SDF
			{
				min_time = InitFindMinTime(&readyQueue_head); //��ʼ�����̿���������ʱ��
				r = FindMinTimePCB(&readyQueue_head, min_time); //�ҵ�����ʱ����̵Ľ��̿�
				SPFTransferPCB(r, &readyQueue_head, &runningQueue_head);  //�Ӿ�������ȡ��Ҫ���еĽ��̿飬�ᵽ���ж�����
			}
			else
			{
				SPFTransferPCB(r, &readyQueue_head, &runningQueue_head); //�Ӿ�������ȡ��Ҫ���еĽ��̿飬�ᵽ���ж�����
			}
			//���н��̿�
			SPFRunningPCB(&runningQueue_head); //���н��̿�

			l = r;

			//�����ж����еĽ��̿�û�з��ؾ�������֮ǰ��Ҫ�ھ���������������ѡһ������ʱ����̵�
			if (!(flag_runningQueue = IsEmptyQueue(readyQueue_head))) //�������в�Ϊ�ղ���������ѡ��������̿黹��Ϊr
			{
				min_time = InitFindMinTime(&readyQueue_head); //��ʼ�����̿���������ʱ��
				r = FindMinTimePCB(&readyQueue_head, min_time); //�ҵ�����ʱ����̵Ľ��̿�
			}
			SPFFinishRunningPCB(l, &readyQueue_head, &runningQueue_head); //���̿�������ɣ��ͷŽ��̿��ѽ��̿�Żؾ�������
			count_SPF++;
		}
	}
	return 0;
}

void InitReadyQueue(pPCB readyQueue_head, int num_process) //��ʼ����������
{
	pPCB r = readyQueue_head;
	for (int i = 0; i < num_process; i++) //�������г�ʼ��
	{
		pPCB p = malloc(sizeof(PCB));

		p->pid = i;
		p->state = READY;
		scanf("%d", &p->time); //�����̿鸳ֵ

		r->next = p;
		p->next = NULL;
		r = p;
	}
	return readyQueue_head;
}

void FCFSRunningPCB(pPCB runningQueue_head) //���ж������н��̿�
{
	pPCB r = runningQueue_head->next;
	r->state = RUNNING; //�ý��̿��״̬�ɡ���������Ϊ�����С�
	if (r->time >= 2) //���ý��̿��(ʣ��)����ʱ�����ʱ��Ƭ����(ʣ��)����ʱ��-2
		r->time -= 2;
	else //���ý��̿��(ʣ��)����ʱ��С��ʱ��Ƭ����(ʣ��)����ʱ��ֱ�Ӹ�Ϊ0
		r->time = 0;
	printf("P0%d ", r->pid);
}

void FCFSFinishRunningPCB(pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB r = runningQueue_head->next;
	pPCB p = readyQueue_head;
	if (r->time == 0) //�ý��̿�������ɣ��ͷ�
		free(r);
	else
	{
		r->state = READY; //�ý��̿��״̬�ɡ����С���Ϊ��������

		//���̿���뵽��������ĩβ
		if (readyQueue_head->next == NULL) //��������ֻʣͷ���
			readyQueue_head->next = r;
		else //�������г���ͷ��㻹���������̿�
			for (p; p != NULL; p = p->next)
				if (p->next == NULL)
				{
					p->next = r;
					break;
				}
	}
	runningQueue_head->next = NULL; //���ж���ɾȥ������ɵĽ��̿�
}

void FCFSTransferPCB(pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB r = readyQueue_head->next;
	readyQueue_head->next = r->next; //��������ɾ����һ�����̿�

	runningQueue_head->next = r; //���ý��̿���뵽���ж���
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
	for (p; p != NULL; p = p->next) //�ҳ����������и����̿���̵�����ʱ��
		if (p->time < min_time)
			min_time = p->time;

	for (r = readyQueue_head->next; r != NULL; r = r->next) //����������������ʱ�����ٵĽ��̿���ȡ���������̿�Ϊr
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
	for (q; q != NULL; q = q->next) //����������ɾȥ���̿�r
	{
		if (q == r)
		{
			p->next = q->next;
			break;
		}
		p = p->next;
	}
	runningQueue_head->next = r; //�����̿�r���뵽���ж�����
	r->next = NULL;
}

void SPFRunningPCB(pPCB runningQueue_head)
{
	pPCB r;
	r = runningQueue_head->next;
	r->state = RUNNING; //�ý��̿��״̬�ɡ���������Ϊ�����С�
	if (r->time >= 2) //���ý��̿��(ʣ��)����ʱ�����ʱ��Ƭ����(ʣ��)����ʱ��-2
		r->time -= 2;
	else //���ý��̿��(ʣ��)����ʱ��С��ʱ��Ƭ����(ʣ��)����ʱ��ֱ�Ӹ�Ϊ0
		r->time = 0;
	printf("P0%d ", r->pid);
}

void SPFFinishRunningPCB(pPCB l, pPCB readyQueue_head, pPCB runningQueue_head)
{
	pPCB p;
	p = readyQueue_head;

	if (l->time == 0) //�ý��̿�������ɣ��ͷ�
		free(l);
	else
	{
		l->state = READY; //�ý��̿��״̬�ɡ����С���Ϊ��������

		//���̿���뵽��������ĩβ
		if (readyQueue_head->next == NULL) //��������ֻʣͷ���
			readyQueue_head->next = l;
		else //�������г���ͷ��㻹���������̿�
			for (p; p != NULL; p = p->next)
				if (p->next == NULL)
				{
					p->next = l;
					break;
				}
	}
	runningQueue_head = NULL;
}