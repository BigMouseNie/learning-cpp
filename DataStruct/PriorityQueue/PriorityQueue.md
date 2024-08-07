# Priority Queue

所谓Priority Queue可以理解为带有权的元素形成的队列，即元素的插入存在插队。比如按照数值的大小为权，

每次Pop为最小元素，很容易想到运用数组或链表，但是数组和链表的Insert的时间复杂度为O(N)，虽然循环

数组和链表的Pop的时间复杂度为O(1),也是比较慢的。所以这里引入树，可以将时间复杂度和空间复杂度都降

为O(logn)! 又因为这会形成个满二叉树，所以可以用数组模拟，并且该队列只需要满足Insert和Pop操作，无需

排序，因此左右根结点三者的约束关系不必像搜索二叉树如此，只需要堆的关系即可！

对于数组空间的开辟无需像线段树那样开 4N，我们只需要用多少开多少即可！

**NOTE: 下面的代码是一个元素为int数值，Pop队列中最小元素的Priority Queue！**



[TOC]

### 一、My Priority Queue Class



```c++
#include<iostream>
#include<vector>

using namespace std;
class PriorityQueue {
private:
	vector<int> pQueue;
	int MaxSize; // 最大容量
	int eleCount = 0; // 当前已入队的元素个数
	int cur = 1;  // 待插入索引，索引从"1"开始易实现 NodeFloating!

	void eleSwap(int a, int b) {} // 交换 pQueue 中索引 a和b的值

	int NodeSinking(int idx) {}

	void NodeFloating(int idx) {}

public:
	PriorityQueue(int Size=1000) {
		MaxSize = Size;
		pQueue = vector<int>(Size+1,INT_MAX);
	}
	~PriorityQueue() {}

	bool pQueueInsert(int val) {} // Insert 操作

	bool pQueuePop() {} // Pop 操作

	bool isEmpty() {} 

	bool isFull() {}

	int eleNums() {} // 返回已经入队结点个数

};

```





### 二、Insert



```C++
/*
* 插入操作类似于队列结点的上浮，在带插入点插入新结点，然后结点于根结点相比较
* 若比根结点大则返回，否则于根结点交换，即上浮。
*/

bool pQueueInsert(int val) {
    if (isFull()) { return false; }
    pQueue[cur] = val; // 先将其插入，再令其上浮
    ++eleCount; // updata
    NodeFloating(cur);
    ++cur; // updata
    return true;
}

void NodeFloating(int idx) { // 传入的idx就是待插入索引
    int k = idx;
    while (k > 1) {
        int temp = k >> 1;
        if (pQueue[k] < pQueue[temp]) {
            eleSwap(k, temp);
            k = temp;
        }
        else { return; }
    }
    return;
}


```



### 三、Pop



```c++
/*
* 弹出操作类似于结点的下沉，弹出根结点，即下沉根结点，再使其删除。
* 令根结点同左右结点中最小的结点交换，重复此操作，当到达最后一层时，再于最后一个结点交换即可。
*/

bool pQueuePop() {
    if (isEmpty()) { return false; }
    --eleCount;--cur;
    pQueue[NodeSinking(1)]=pQueue[cur]; // 无需交换，直接赋值即可，因为我们的目的就是Pop
    return true;
}

int NodeSinking(int idx) { // 返回根结点最后一层的位置
    int k = idx;
    while (k<<1 < cur) {
        // 拿到左右结点的最小值的索引
        int temp = pQueue[k << 1] < pQueue[k << 1 | 1] ? k << 1 : k << 1 | 1;
        if (temp >= cur) { return k; }
        pQueue[k] = pQueue[temp]; // 无需交换，直接赋值即可，因为我们的目的就是Pop
        k = temp;
    }
    return k;
}

```



### 四、实现



```c++
#include<iostream>
#include<vector>
using namespace std;

class PriorityQueue {
private:
	vector<int> pQueue;
	int MaxSize;
	int eleCount = 0;
	int cur = 1; 

	void eleSwap(int a, int b) {
		int temp = pQueue[a];
		pQueue[a] = pQueue[b];
		pQueue[b] = temp;
		return;
	}

	int NodeSinking(int idx) {
		int k = idx;
		while (k<<1 < cur) {
			int temp = pQueue[k << 1] < pQueue[k << 1 | 1] ? k << 1 : k << 1 | 1;
			if (temp >= cur) { return k; }
			pQueue[k] = pQueue[temp];
			k = temp;
		}
		return k;
	}

	void NodeFloating(int idx) {
		int k = idx;
		while (k > 1) {
			int temp = k >> 1;
			if (pQueue[k] < pQueue[temp]) {
				eleSwap(k, temp);
				k = temp;
			}
			else { return; }
		}
		return;
	}

public:
	PriorityQueue(int Size=1000) {
		MaxSize = Size;
		pQueue = vector<int>(Size+1,INT_MAX);
	}
	~PriorityQueue() {}

	bool pQueueInsert(int val) {
		if (isFull()) { return false; }
		pQueue[cur] = val;
		++eleCount;
		NodeFloating(cur);
		++cur;
		return true;
	}

	bool pQueuePop() {
		if (isEmpty()) { return false; }
		--eleCount;--cur;
		pQueue[NodeSinking(1)]=pQueue[cur];
		return true;
	}

	bool isEmpty() {
		return eleCount == 0;
	}

	bool isFull() {
		return eleCount == MaxSize;
	}

	int eleNums() {
		return eleCount;
	}

	void PrintHeap() {
		for (int i = 1; i < cur; ++i) {
			cout << pQueue[i] << " " << endl;
		}
	}

};


int main() {
	PriorityQueue PQ(10);
	PQ.pQueueInsert(7);
	PQ.pQueueInsert(2);
	PQ.pQueueInsert(13);
	PQ.pQueueInsert(1);
	PQ.pQueueInsert(4);
	PQ.pQueueInsert(8);
	PQ.pQueueInsert(99);
	PQ.pQueuePop();
	PQ.pQueuePop();
	PQ.PrintHeap();
	return 0;
}

```





























































