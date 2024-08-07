# Binary Tree



[TOC]



#### 一、<u>Binary Tree的创建</u>

##### 1、C  创建树节点

```c
// By C

typedef struct TreeNode
{
	int Val;   
	struct TreeNode* left; 
	struct TreeNode* right;
}TreeNode;

```



##### 2、C++ 创建树节点

```c++
// By C++

// By struct
 struct TreeNode {
     int val;
     TreeNode* left;
     TreeNode* right;
     TreeNode() : val(0), left(nullptr), right(nullptr) {}
     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
     TreeNode(int x, TreeNode* left, TreeNode* right) : 
             val(x), left(left), right(right) {}
};

// By class
class TreeNode{
public:
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(){right=NULL;left=NULL;}
};

int main(){
    TreeNode* root=new TreeNode(); // 创建指针时用 new();
    TreeNode root;                 // Node实例无需用 new();
}


```



##### 3、Python 创建树节点

```python
# By Python

class TreeNode():
    def __init__(self,val,left=None,right=None):
        self.val=val	
        self.left=lchild		
        self.right=rchild		
        vector<int>


```



#### 二、<u>Binary Tree的遍历</u>

​		

| 遍历方式 | 时间复杂度 | 空间复杂度 |
| :------- | ---------- | ---------- |
| 递归     | O(n)       | O(n)       |
| 迭代     | O(n)       | O(n)       |
| 层序     | O(n)       | O(n)       |
| Morris   | O(2n)      | O(1)       |

##### 1、递归（By C++）

```c++
/*
*所谓递归遍历，即 前、中、后序 三种，递归实现的本质也是系统帮我们建立了栈结构，
*而系统栈需要记住每个节点的值，所以空间复杂度仍为O(N)
*/
void Order(TreeNode* root){
    if(root){
        // 前序操作位
        Order(root->left);
        // 中序操作位
        Order(root->right);
        // 后序操作位
    }
}

```



##### 2、迭代（By C++）

```c++
// 所谓迭代遍历即 利用栈数据结构进行二叉树遍历
void OrderByItr(TreeNode* root) {     // 迭代实现 前序和中序 遍历
    stack<TreeNode*> stk;
    vector<int> result;
    while (root != nullptr || !stk.empty()) {
        while (root != nullptr) {
            // 前序遍历节点可操作位置
            stk.push(root);
            root = root->left;
        }
        root = stk.top(); stk.pop();
        // 后序遍历节点可操作位置
        root = root->right;
    }
}

//后序遍历有些许不同，可以认为反着的前序遍历，即 顺序为 根 右 左; 最后反转数组

vector<int> postOrderByItr(TreeNode* root) {  
    stack<TreeNode*> stk;
    vector<int> result;
    while (root != nullptr || !stk.empty()) {
        while (root != nullptr) {
            stk.push(root);
            result.push_back(root->val);  // 操作为同前序相同
            root = root->right;    // 先遍历右      
        }
        root = stk.top(); stk.pop();
        root = root->left;         // 后遍历左
    }
    reverse(result.begin(), result.end());  // 反转遍历数组
    return result;
}


```



##### 3、层序遍历（By C++）

```c++
// 所谓层序遍历，是利用数组结构 数组来进行 一层一层的遍历

vector<vector<int>> levelOrder(TreeNode* root) { // 返回值为二维vector,索引i为第i+1层
    queue<TreeNode*> que;
    if (root != NULL) que.push(root);
    vector<vector<int>> result;
    while (!que.empty()) {
        int size = que.size();
        vector<int> vec;
        // 这里一定要使用固定大小size，不要使用que.size()，因为que.size是不断变化的
        for (int i = 0; i < size; i++) {
            TreeNode* node = que.front();
            que.pop();
            vec.push_back(node->val);
            if (node->left) que.push(node->left);
            if (node->right) que.push(node->right);
        }
        result.push_back(vec);
    }
    return result;
}

```



##### 4、Morris 遍历（By C++）

```c++
/*
*所谓Morris 遍历其特点就是 空间复杂度: O(n)，但是时间复杂度: O(2n);即 该二叉树将遍历 两 次
*其精髓就是 找到root节点的左节点的最右节点，将该节点(叶子节点)的右节点指向root,下次遍历将
*直接到达该节点，并且下次遍历进行判断来断开该节点指向，但是牺牲了时间,节省了空间
*/

// Note:前序有两个操作位缺一不可
// 没有后序，后序其实和迭代的后序一样，根 右 左 ，在反转
vector<int> MorrisInOrder(TreeNode* root) {
	vector<int> result;
	TreeNode* cur = root;
	TreeNode* mostright = nullptr;
	while (cur) {
		mostright = cur->left;
		if (mostright) {
			while (mostright->right && mostright->right != cur) {
				mostright = mostright->right;
			}
			if (!mostright->right) {
				result.push_back(cur->val);  // 节点可操作位置  <前序>
				mostright->right = cur;
				cur = cur->left;
				continue;
			}
			else {
				mostright->right = nullptr;
			}
		}
		else { result.push_back(cur->val); } // 节点可操作位置  <前序>
		//result.push_back(cur->val);        // 节点可操作位置  <中序>
		cur = cur->right;
	}
	return result;
}

```































