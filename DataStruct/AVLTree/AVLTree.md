# AVL Tree

所谓AVL Tree就是一种自平衡树，可以在结点的插入和删除都能保持根结点的左右子树高度之差小于1。



[TOC]

### 一、TreeNode 和 AVLTree 的定义

```c++
/* 这里TreeNode 和 AVLTree的定义都用类模板！ */

/*
* 这里TreeNode采用struct来定义，虽然可以用类，但是本人喜欢用struct！
*/

template <class T>
struct TreeNode {
    T val;
    int height; // 树高，空结点的树高为0
    TreeNode<T>* left;
    TreeNode<T>* right;
    TreeNode (T x) : val(x), height(1), left(nullptr), right(nullptr) {}
};

/*
* AVLTree 用类来封装
*/

template <class T>
class AVLTree {
private:
    int NodeCount = 0;  // 结点计数
    TreeNode<T>* ROOT = nullptr; // AVLTree的根结点
    int GetHeight(TreeNode<T>* root); // 获取结点的高度
    void UpdataHeight(TreeNode<T>* root); // 更新结点的树高
    TreeNode<T>* RealInsert(TreeNode<T>* root, T val); // 主插入函数
	TreeNode<T>* RealRemove(TreeNode<T>* root, T target); // 主删除函数
    TreeNode<T>* LL_Rotation(TreeNode<T>* root); // 调整不平衡LL型
    TreeNode<T>* RR_Rotation(TreeNode<T>* root); // 调整不平衡RR型
    TreeNode<T>* LR_Rotation(TreeNode<T>* root); // 调整不平衡LR型
    TreeNode<T>* RL_Rotation(TreeNode<T>* root); // 调整不平衡RL型
	void CLEARAVLTree(TreeNode<T>* &root); // 主清空函数
public:
	AVLTree(); // AVLTree 构造函数
	~AVLTree(); // AVLTree 解析函数
    TreeNode<T>* GetRoot(); // 获取 AVLTree 的根结点
    int GetNodeNums(); // 获取 AVLTree的总结点个数
	T MinNodeVal(TreeNode<T>* root); // 获取该root下的最小val
	T MaxNodeVal(TreeNode<T>* root); // 获取该root下的最大val
    void Insert(T val); // 可调用API 插入
    void Remove(T target); // 可调用API 删除
	void Clear(); // 可调用API 清空
    void PrintTree(TreeNode<T>* root); // 打印AVLTree树以方便验证
};

```





### 二、树的不平衡类型以及解决方法

```C++
/*
* LL型 
* 该不平衡的原因是新结点插入当前root结点的左子树的左子树导致的
* 即有 (root->left->height - root->right->height)==2 
*/

/*
* 解决方案
* 令左子树进行右旋操作，即root->left 上位为 root
*/

template <class T>
TreeNode<T>* AVLTree<T>::LL_Rotation(TreeNode<T>* root) {
	TreeNode<T>* temp = root->left;
	root->left = temp->right;
	temp->right = root;
	UpdataHeight(root); // 更新根结点height
	return temp;
}


/*
* RR型 
* 该不平衡的原因是新结点插入当前root结点的右子树的右子树导致的
* 即有 (root->right->height - root->left->height)==2 
*/

/*
* 解决方案
* 令右子树进行左旋操作，即root->right 上位为 root
*/

template <class T>
TreeNode<T>* AVLTree<T>::RR_Rotation(TreeNode<T>* root) {
	TreeNode<T>* temp = root->right;
	root->right = temp->left;
	temp->left = root;
	UpdataHeight(root); 
	return temp;
}

/*
* LR型
* 该不平衡的原因是新结点插入当前root结点左子树的右子树导致
* 即有 (root->left->height - root->right->height)==2 
* (root->left->left->height)<(root->left->right->height)
*/

/*
* 解决方案
* 令左子树的右子树进行左旋操作
* 令左子树进行右旋操作
*/

template <class T>
TreeNode<T>* AVLTree<T>::LR_Rotation(TreeNode<T>* root) {
	root->left = RR_Rotation(root->left); // RR型的左旋
	root = LL_Rotation(root);             // LL型的右旋
	UpdataHeight(root->left);
	return root;
}


/*
* RL型
* 该不平衡的原因是新结点插入当前root结点右子树的左子树导致
* 即有 (root->right->height - root->left->height)==2 
* (root->right->left->height)>(root->right->right->height)
*/

/*
* 解决方案
* 令右子树的左子树进行右旋操作
* 令右子树进行左旋操作
*/

template <class T>
TreeNode<T>* AVLTree<T>::RL_Rotation(TreeNode<T>* root) {
	root->right = LL_Rotation(root->right); // LL型的右旋
	root = RR_Rotation(root);               // RR型的左旋
	UpdataHeight(root->right);
	return root;
}
```





### 三、AVLTree 结点的插入

```c++
/*
* AVLTree 的结点插入方式是以递归的形式进行，开始通搜索二叉树的Insert方法相同
* 不同的是，在返回时需要检测左右子树的高度差是否为2，来进行调整！
*/

// 这里提供一个无返回值的共有函数，来调用一个拥有真正的Insert函数主体的私有函数
template <class T>
void AVLTree<T>::Insert(T val) {
	this->NodeCount++;
	if (!this->ROOT) {
		this->ROOT = new TreeNode<T>(val);
	}
	else { this->ROOT = RealInsert(this->ROOT, val); }
}

template <class T>
TreeNode<T>* AVLTree<T>::RealInsert(TreeNode<T>* root, T val) {
	if (!root) {
		root = new TreeNode<T>(val);
	}
	else if (root->val > val) { // 插入左结点
		root->left = RealInsert(root->left, val);
		if (abs(GetHeight(root->left) - GetHeight(root->right)) == 2) {// 需调整
			// 匹配调整类型
			if (GetHeight(root->left->left) > GetHeight(root->left->right)) {
				root = LL_Rotation(root);
			}
			else { root = LR_Rotation(root); }
		}
	}
	else if (root->val < val) { // 插入右结点
		root->right = RealInsert(root->right, val);
		if (abs(GetHeight(root->left) - GetHeight(root->right)) == 2) {// 需调整
			// 匹配调整类型
			if (GetHeight(root->right->right) > GetHeight(root->right->left)) {
				root = RR_Rotation(root);
			}
			else { root = RL_Rotation(root); }
		}
	}
	else { // 禁止插入相同的val的结点
		this->NodeCount--;
	}
	UpdataHeight(root);// updata height
	return root;
}
```





### 四、AVLTree 结点的删除

```c++
/*
* AVLTree 的结点删除方式是以递归的形式进行，先寻找需要删除的值
* 找到值时，对该结点的左右子树是否为NULL来进行不同的操作
*/

/* 
* 若左右子树都不为NULL: 判断左右子树的高
*   若左子树高: 寻找左子树的最大值同当前root值交换
*   若右子树高: 寻找右子树的最小值同当前root值交换   
* 若左右子树至少一个为空:
*   若左子树为空: 将右子树root结点上位到当前root
*   否则: 将左子树root结点上位到当前root
*/

// 提供无返回值的共有函数，来调用一个拥有真正的Remove函数主体的私有函数
template <class T>
void AVLTree<T>::Remove(T target) {
	if (!this->ROOT) { return; }
	this->ROOT = RealRemove(this->ROOT, target);
	return;
}

template <class T>
TreeNode<T>* AVLTree<T>::RealRemove(TreeNode<T>* root, T target) {
	if (!root) { return nullptr; }
	else if (root->val > target) { // 在root的左子树
		root->left = RealRemove(root->left, target);
		if (abs(GetHeight(root->right) - GetHeight(root->right)) == 2) {
			if (GetHeight(root->right->left) > GetHeight(root->right->right)) {
				root = RL_Rotation(root);
			}
			else { root = RR_Rotation(root); }
		}
	}
	else if (root->val < target) { // 在root的右子树
		root->right = RealRemove(root->right, target);
		if (abs(GetHeight(root->right) - GetHeight(root->right)) == 2) {
			if (GetHeight(root->left->left) > GetHeight(root->left->right)) {
				root = LL_Rotation(root);
			}
			else { root = LR_Rotation(root); }
		}
	}
	else { // 在当前结点
		if (root->left && root->right) { // 左右都不为NULL
			if (GetHeight(root->left) > GetHeight(root->right)) {
				T LMaxVal = MaxNodeVal(root->left); // 寻找左子树的最大值
				root->val = LMaxVal;
				root->left = RealRemove(root->left, LMaxVal);
			}
			else {
				T RMinVal = MinNodeVal(root->right); // 寻找右子树的最小值
				root->val = RMinVal;				
				root->right = RealRemove(root->right, RMinVal);
			}
		}
		else { // 左右子树至少一个为空
			TreeNode<T>* temp = root;
			root = (root->left) ? root->left : root->right;
			--this->NodeCount;
			delete temp;
		}
	}
	UpdataHeight(root);
	return root;
}

```





### 五、代码展示与测试案例

#### AVLTree.h

```c++
/*
* NOTE:
* 因为该函数都是用类模板写的，所以并不能将函数实行放在单独的cpp文件内编译,因此都写在了头文件
*/

#pragma once
#ifndef AVLTree_H_
#define AVLTree_H_
#include<iostream>
#include<queue>
using namespace std;

/*************树节点定义*************/
template <class T>
struct TreeNode {
    T val;
    int height;
    TreeNode<T>* left;
    TreeNode<T>* right;
    TreeNode (T x) : val(x), height(1), left(nullptr), right(nullptr) {}
};

/*************AVLTree类的定义*************/
template <class T>
class AVLTree {
private:
    int NodeCount = 0;
    TreeNode<T>* ROOT = nullptr;
    int GetHeight(TreeNode<T>* root);
    void UpdataHeight(TreeNode<T>* root);
    TreeNode<T>* RealInsert(TreeNode<T>* root, T val);
	TreeNode<T>* RealRemove(TreeNode<T>* root, T target);
    TreeNode<T>* LL_Rotation(TreeNode<T>* root);
    TreeNode<T>* RR_Rotation(TreeNode<T>* root);
    TreeNode<T>* LR_Rotation(TreeNode<T>* root);
    TreeNode<T>* RL_Rotation(TreeNode<T>* root);
	void CLEARAVLTree(TreeNode<T>* &root);
public:
	AVLTree();
	~AVLTree();
    TreeNode<T>* GetRoot();
    int GetNodeNums();
	T MinNodeVal(TreeNode<T>* root);
	T MaxNodeVal(TreeNode<T>* root);
    void Insert(T val);
    void Remove(T target);
	void Clear();
    void PrintTree(TreeNode<T>* root);
};


/*************AVLTree类中函数的实现*************/
template <class T>
AVLTree<T>::AVLTree() {}

template <class T>
AVLTree<T>::~AVLTree() {
	if (this->ROOT) { Clear(); }
}

template <class T>
int AVLTree<T>::GetHeight(TreeNode<T>* root) {
	if (!root) { return 0; }
	return root->height;
}

template <class T>
void AVLTree<T>::UpdataHeight(TreeNode<T>* root) {
	if (root) {
		root->height = max(GetHeight(root->left), GetHeight(root->right)) + 1;
	}
}

template <class T>
TreeNode<T>* AVLTree<T>::GetRoot() {
	if (!this->ROOT) { return nullptr; }
	return this->ROOT;
}

template <class T>
int AVLTree<T>::GetNodeNums() {
	return this->NodeCount;
}

template <class T>
T AVLTree<T>::MinNodeVal(TreeNode<T>* root) {
	if (!root) { cout << "错误的NULL值!" << endl; }
	while (root->left) {
		root = root->left;
	}
	return root->val;
}

template <class T>
T AVLTree<T>::MaxNodeVal(TreeNode<T>* root) {
	if (!root) { cout << "错误的NULL值!" << endl; }
	while (root->right) {
		root = root->right;
	}
	return root->val;
}

template <class T>
void AVLTree<T>::Insert(T val) {
	this->NodeCount++;
	if (!this->ROOT) {
		this->ROOT = new TreeNode<T>(val);
	}
	else { this->ROOT = RealInsert(this->ROOT, val); }
}

template <class T>
TreeNode<T>* AVLTree<T>::RealInsert(TreeNode<T>* root, T val) {
	if (!root) {
		root = new TreeNode<T>(val);
	}
	else if (root->val > val) { // 插入左结点
		root->left = RealInsert(root->left, val);
		if (abs(GetHeight(root->left) - GetHeight(root->right)) == 2) {// 需调整
			// 匹配调整类型
			if (GetHeight(root->left->left) > GetHeight(root->left->right)) {
				root = LL_Rotation(root);
			}
			else { root = LR_Rotation(root); }
		}
	}
	else if (root->val < val) { // 插入右结点
		root->right = RealInsert(root->right, val);
		if (abs(GetHeight(root->left) - GetHeight(root->right)) == 2) {// 需调整
			// 匹配调整类型
			if (GetHeight(root->right->right) > GetHeight(root->right->left)) {
				root = RR_Rotation(root);
			}
			else { root = RL_Rotation(root); }
		}
	}
	else { // 禁止插入相同的val的结点
		this->NodeCount--;
	}
	UpdataHeight(root);// updata height
	return root;
}

template <class T>
void AVLTree<T>::Remove(T target) {
	if (!this->ROOT) { return; }
	this->ROOT = RealRemove(this->ROOT, target);
	return;
}

template <class T>
TreeNode<T>* AVLTree<T>::RealRemove(TreeNode<T>* root, T target) {
	if (!root) { return nullptr; }
	else if (root->val > target) { // 在root的左子树
		root->left = RealRemove(root->left, target);
		if (abs(GetHeight(root->right) - GetHeight(root->right)) == 2) {
			if (GetHeight(root->right->left) > GetHeight(root->right->right)) {
				root = RL_Rotation(root);
			}
			else { root = RR_Rotation(root); }
		}
	}
	else if (root->val < target) { // 在root的右子树
		root->right = RealRemove(root->right, target);
		if (abs(GetHeight(root->right) - GetHeight(root->right)) == 2) {
			if (GetHeight(root->left->left) > GetHeight(root->left->right)) {
				root = LL_Rotation(root);
			}
			else { root = LR_Rotation(root); }
		}
	}
	else { // 在当前结点
		if (root->left && root->right) { // 左右都不为NULL
			if (GetHeight(root->left) > GetHeight(root->right)) {
				T LMaxVal = MaxNodeVal(root->left);
				root->val = LMaxVal;
				root->left = RealRemove(root->left, LMaxVal);
			}
			else {
				T RMinVal = MinNodeVal(root->right);
				root->val = RMinVal;
				root->right = RealRemove(root->right, RMinVal);
			}
		}
		else {
			TreeNode<T>* temp = root;
			root = (root->left) ? root->left : root->right;
			--this->NodeCount;
			delete temp;
		}
	}
	UpdataHeight(root);
	return root;
}

template <class T>
TreeNode<T>* AVLTree<T>::LL_Rotation(TreeNode<T>* root) {
	TreeNode<T>* temp = root->left;
	root->left = temp->right;
	temp->right = root;
	UpdataHeight(root);
	return temp;
}

template <class T>
TreeNode<T>* AVLTree<T>::RR_Rotation(TreeNode<T>* root) {
	TreeNode<T>* temp = root->right;
	root->right = temp->left;
	temp->left = root;
	UpdataHeight(root); 
	return temp;
}

template <class T>
TreeNode<T>* AVLTree<T>::LR_Rotation(TreeNode<T>* root) {
	root->left = RR_Rotation(root->left);
	root = LL_Rotation(root);
	UpdataHeight(root->left);
	return root;
}

template <class T>
TreeNode<T>* AVLTree<T>::RL_Rotation(TreeNode<T>* root) {
	root->right = LL_Rotation(root->right);
	root = RR_Rotation(root);
	UpdataHeight(root->right);
	return root;
}

template <class T>
void AVLTree<T>::Clear() {
	CLEARAVLTree(this->ROOT);
	this->ROOT = nullptr;
}

template <class T>
void AVLTree<T>::CLEARAVLTree(TreeNode<T>* &root) {
	if (root) {
		CLEARAVLTree(root->left);
		CLEARAVLTree(root->right);
		delete(root);
	}
}

template <class T>
void AVLTree<T>::PrintTree(TreeNode<T>* root) {
	if (!root) { return; }
	queue<TreeNode<T>*> printQu;
	printQu.push(root);
	while (!printQu.empty()) {
		int pLen = printQu.size();
		for (int i = 0; i < pLen; ++i) {
			TreeNode<T>* temp = printQu.front();
			printQu.pop();
			if (temp->left) { printQu.push(temp->left); }
			if (temp->right) { printQu.push(temp->right); }
			cout << "   " << temp->val << "(" << temp->height << ")   ";
		}
		cout << endl;
	}
}


#endif // !AVLTree_H_

```



#### Main.cpp

```c++
/*
* 多种不平衡类型测试
*/


#include"AVLTree.h"

int main() {
	AVLTree<int> x;
	// LL(0)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(16);*/

	// LL(1)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(47);
	x.Insert(16);
	x.Insert(28);
	x.Insert(9);*/

	// RR(0)
	/*x.Insert(31);
	x.Insert(47);
	x.Insert(69);*/

	// RR(1)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(47);
	x.Insert(40);
	x.Insert(69);
	x.Insert(76);*/

	// LR(0)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(28);*/

	// LR(L)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(47);
	x.Insert(16);
	x.Insert(28);
	x.Insert(26);*/

	// LR(R)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(47);
	x.Insert(16);
	x.Insert(28);
	x.Insert(30);*/

	// RL(0)
	/*x.Insert(31);
	x.Insert(47);
	x.Insert(40);*/

	// RL(L)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(47);
	x.Insert(40);
	x.Insert(69);
	x.Insert(36);*/

	// RL(R)
	/*x.Insert(31);
	x.Insert(25);
	x.Insert(47);
	x.Insert(40);
	x.Insert(69);
	x.Insert(43);*/

	x.Insert(4);
	x.Insert(2);
	x.Insert(8);
	x.Insert(1);
	x.Insert(3);
	x.Insert(6);
	x.Insert(10);
	x.Insert(5);
	x.Insert(7);
	x.Insert(9);
	x.Insert(12);
	x.Insert(13);
	x.Remove(10);
	x.Remove(8);
	x.PrintTree(x.GetRoot());
	return 0;
}




```



