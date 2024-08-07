# Segment Tree

将数组的 区间和查询&数值更新 经Segment Tree操作复杂度都平均为O(log n)级别，就是线段树经典操作。

因为数组的树是一个完全二叉树，故优化为数组表示(类似Heap Sort)。

Note: 将由 C++ 操作，索引由1开始，树节点访问将用位运算优化。



[TOC]



### 一、递归实现

```C++
/*  
* 该树的索引从 1 开始，便于利用位运算优化
* 我将其包装成一个类
* function: 单点 & 区间 的修改、 区间的和查询
* Updata 函数被重载为 单点 & 区间 两种方式
*/




#include<iostream>
#include<vector>
using namespace std;

class SegmentTree {
private:
	int Judge = false;   // 判断该树是否已经创建
	vector<int> TreeArr;
	vector<int> Lazy;
                                      // 树索引    arr区间开始  区间结束
	void M_BulidTree(vector<int>& arr, int node, int start, int end) { 
		if (start == end) {  // 该区间只有一个值的时候结束递归   // start & end 为arr的索引区间
			TreeArr[node] = arr[start];
			return;
		}
		int mid = (start + end) >> 1;  // 位运算优化
		int left_node = node << 1;
		int right_node = node << 1 | 1;
		M_BulidTree(arr, left_node, start, mid);
		M_BulidTree(arr, right_node, mid + 1, end);
		TreeArr[node] = TreeArr[left_node] + TreeArr[right_node];
		return;
	}
                                                  // idx为需修改值的索引，val是更新值
	void M_UpdataTree(vector<int>& arr, int node, int start, int end, int idx, int val) {
		if (start == end) {                                        
			arr[idx] = val;
			TreeArr[node] = val;
			return;
		}
		int mid = (start + end) >> 1;
		int left_node = node << 1;
		int right_node = node << 1 | 1;
		if (idx >= start && idx <= mid) {
			M_UpdataTree(arr, left_node, start, mid, idx, val);
		}
		else { M_UpdataTree(arr, right_node, mid + 1, end, idx, val); }
		TreeArr[node] = TreeArr[left_node] + TreeArr[right_node];
		return;
	}

	void PushDown(int node,int Lnum,int Rnum) { // node 为数索引，Lnum为node左子树的节点数
		if (Lazy[node]) { // 如果为零就不需要下推标记
			Lazy[node << 1] += Lazy[node];    // 下推标记
			Lazy[node >> 1 | 1] += Lazy[node];
			TreeArr[node << 1] += Lazy[node] * Lnum;  // 更新值
			TreeArr[node << 1 | 1] += Lazy[node] * Rnum;
			Lazy[node] = 0;
		}
		return;
	}

	void M2_UpdataTree(int node, int start, int end, int L, int R, int tadd) {
		if (start >= L && end <= R) {
			TreeArr[node] += tadd * (end - start + 1);
			Lazy[node] += tadd;
			return;
		}
		int mid = (start + end) >> 1;
		PushDown(node, mid - start + 1, end - mid);
		if (L <= mid) { M2_UpdataTree(node << 1, start, mid, L, R, tadd); }
		if (R >= mid+1) { M2_UpdataTree(node << 1 | 1, mid + 1, end, L, R, tadd); }
		TreeArr[node] = TreeArr[node << 1] + TreeArr[node << 1 | 1];
	}

	int M_QueryTree(vector<int>& arr, int node, int start, int end, int queryL, int queryR) {
        // 在所求区间的范围内直接返回
		if (start >= queryL && end <= queryR) { return TreeArr[node]; } 
		int mid = (start + end) >> 1;
		int left_node = node << 1;
		int right_node = node << 1 | 1;
		PushDown(node, mid - start + 1, end - mid);
return (mid >= queryL ? M_QueryTree(arr, left_node, start, mid, queryL, queryR) : 0)
+ (mid + 1 <= queryR ? M_QueryTree(arr, right_node, mid + 1, end, queryL, queryR) : 0);
	}

public:
	SegmentTree() {}

	SegmentTree(vector<int>& arr) {
		BulidTree(arr);
	}

	void BulidTree(vector<int>& arr) {
		if (Judge) { return; }
		TreeArr = vector<int>(arr.size() << 2);
		Lazy = vector<int>(arr.size() << 2, 0);
		Judge = true;
		M_BulidTree(arr, 1, 0, arr.size() - 1);
	}
	// 单点修改，idx为需修改值的索引，val是更新值
	void UpdataTree(vector<int>& arr, int idx, int val) { 
		if (!Judge) { return; }
		M_UpdataTree(arr, 1, 0, arr.size() - 1, idx, val);
	}

	void UpdataTree(vector<int>& arr, int tadd, int L, int R) {
		if (!Judge) { return; }
		for (int i = L; i <= R; ++i) {
			arr[i]+=tadd;
		}
		M2_UpdataTree(1, 0, arr.size() - 1, L, R, tadd);
	}

	int QueryTree(vector<int>& arr, int queryL, int queryR) {
		if (!Judge) { return 0; }
		return M_QueryTree(arr, 1, 0, arr.size() - 1, queryL, queryR);
	}
};

```





### 二、非递归实现

```c++
/*  待实现中........  */





```







