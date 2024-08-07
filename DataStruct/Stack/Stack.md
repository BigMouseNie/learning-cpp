# Stack

所谓栈，就是一种先近后出的数据结构

欲模拟栈的储存方式，可利用数组和链表

因为大部分语言都有现成的实现，因此将用 C 模拟  



[TOC]



#### 一、By Array

```c
/*
* 这里代码是没有任何健壮性可言的，但是做一个简单的参考是灰常不错的
* Note: 刚开始没用指针，C当成局部处理了
*/

#include<stdio.h>
#include<stdlib.h>

typedef struct{
	int top;       // 索引
	int* arr;
	int MaxSize;   // 最大尺寸
}Stack;

Stack* InitStack(int Size) {
	Stack* stk=(Stack*)malloc(sizeof(Stack));
	stk->top = 0;
	stk->MaxSize = Size;
	stk->arr = (int*)malloc(Size * sizeof(int));
	return stk;
}

int IsFull(Stack* stk) {
	return stk->top == stk->MaxSize;
}

int IsEmpty(Stack* stk) {
	return stk->top == 0;
}

void Back_Push(Stack* stk, int eml) {
	if (IsFull(stk)) { 
		printf("入栈失败！");
		return; 
	}
	stk->arr[stk->top] = eml;
	++stk->top;
	return;
}

void Pop(Stack stk) {
	if (stk.top != 0) { --stk.top; }
}

void PrintStack(Stack* stk) {
	for (int i = 0; i < stk->top; ++i) {
		printf("%d,", stk->arr[i]);
	}
	printf("\n");
	return;
}

int main() {
	Stack* MyStack=InitStack(5);
	
	Back_Push(MyStack, 1);
	Back_Push(MyStack, 2);
	Back_Push(MyStack, 123213);
	PrintStack(MyStack);
	return 0;
}

```



#### 二、By Linked List

```c
//  NULL <- head <- val1 <- val2 <- .... <- top
//  上面的连接看清楚了，是核心
#include<stdio.h>
#include<stdlib.h>

typedef struct xy{
	int val;
	struct xy* next;
}SNode;

typedef struct {
	SNode* head;
	SNode* top;
	int Len;       // 当前栈的长度
	int MaxSize;
}stack;

typedef stack* Stack;  //  这里把 stack* 命名为 Stack

SNode* InitSNode(int eml){
	SNode* node = (SNode*)malloc(sizeof(SNode));
	node->val = eml;
	node->next = NULL;
	return node;
}

Stack InitStack(int size) {
	Stack stk = (Stack)malloc(sizeof(stack));
	stk->head = InitSNode(-1);
	stk->top = stk->head;
	stk->Len = 0;
	stk->MaxSize = size;
	return stk;
}

int IsFull(Stack stk) { return stk->Len == stk->MaxSize; }

int IsEmpty(Stack stk) { return stk->Len == 0; }

void back_push(Stack stk, int eml) {
	if (IsFull(stk)) {
		printf("该栈已满！");
		return;
	}
	SNode* newNode = InitSNode(eml);
	newNode->next = stk->top;
	stk->top = newNode;
	++stk->Len;
	return;
}

void pop(Stack stk) {
	if (IsEmpty) { 
		printf("该栈已空！");
		return; 
	}
	SNode* temp = stk->top;
	stk->top = stk->top->next;
	free(temp);
	--stk->Len;
}

int main() {
	Stack stk = InitStack(5);
	back_push(stk, 1);
	back_push(stk, 1);
	back_push(stk, 1);
	back_push(stk, 1);
	back_push(stk, 1);
	back_push(stk, 1);
	back_push(stk, 1);
	return 1;
}

```









































