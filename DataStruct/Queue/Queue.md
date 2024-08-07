# Queue

所谓队列就是一种先进先出的数组结构

其实现方式可以是数组或队列

因为大部分语言都有现成的，因此用C来实现



[TOC]

#### 一、By Array

```c
// 因为queue的特点用数组其实并不适合，因此采用有size限制的循环queue

#include<stdio.h>
#include<stdlib.h>

struct QueueByArray {
	int* arr;
	int front;
	int rear;  // rear索引是不存元素的
	int Len;   // 因为有了 Len 参数所以判断full or empty 更简单
	int MaxSize;
	int factor;  // 取余常量
};

typedef struct QueueByArray* Queue;

Queue InitQueue(int size) {
	Queue qu = (Queue)malloc(sizeof(struct QueueByArray));
	qu->Len = 0;
	qu->MaxSize = size;
	qu->arr = (int*)malloc((size + 1) * sizeof(int));
	qu->front = 0;
	qu->rear = 0;
	qu->factor = size + 1;
	return qu;
}

int IsFull(Queue qu) { return qu->Len == qu->MaxSize; }// (rear+1)%factor==front;
int IsEmpty(Queue qu) {return qu->Len==0;} // rear==front;

void Push(Queue qu,int eml) {
	if (IsFull(qu)) {
		printf("队列已满！\n");
		return;
	}
	qu->arr[qu->rear++] = eml;
	qu->rear %= qu->factor;
	++qu->Len;
	return;
}


void Pop(Queue qu) {
	if (IsEmpty(qu)) {
		printf("队列已空！\n");
		return;
	}
	++qu->front;
	qu->front %= qu->factor;
	--qu->Len;
}

void PrintQueue(Queue qu) {
	int x = qu->front;
	int y = qu->rear;
	int z = qu->factor;
	while (x != y) {
		printf("%d,",qu->arr[x++]);
		x %= z;
	}
	printf("\n");
	return;
}
// 因为测试代码太长并简单索性删去，当然健壮性很弱，这里只不过是提供一种思想

```



#### 二、By Linked List

```c
// 说实话用链表模拟队列太简单了，后期没有测试，编译了一下，反正都是大白话没有一点点的绕！

#include<stdio.h>
#include<stdlib.h>


typedef struct QNode {
	int val;
	struct QNode* next;
}QNode;

struct QueueByArray {
	QNode* head;
	QNode* rear;
	int Len;
	int MaxSize;
};


typedef struct QueueByArray* Queue;

QNode* InitQNode(int eml) {
	QNode* temp = (QNode*)malloc(sizeof(QNode));
	temp->val = eml;
	temp->next = NULL;
	return temp;
}


Queue InitQueue(int size) {
	Queue qu = (Queue)malloc(sizeof(struct QueueByArray));
	qu->rear = InitQNode(-1);
	qu->head = qu->rear;
	qu->MaxSize = size;
	qu->Len = 0;
	return qu;
}

int IsFull(Queue qu) { return qu->MaxSize==qu->Len; }
int IsEmpty(Queue qu) { return qu->Len == 0; }


void Push(Queue qu, int eml) {
	if (IsFull(qu)) {
		printf("队列已满！\n");
		return;
	}
	QNode* newNode = InitQNode(eml);
	qu->rear->next = newNode;
	qu->rear = qu->rear->next;
	++qu->Len;
	return;
}


void Pop(Queue qu) {
	if (IsEmpty(qu)) {
		printf("队列已空！\n");
		return;
	}
	QNode* temp = qu->head;
	qu->head = qu->head->next;
	free(temp);
	--qu->Len;
	return;
}

```



