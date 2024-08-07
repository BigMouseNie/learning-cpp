# 最近公共祖先(LCA)

最近公共祖先是树图中的问题，即两个树节点的最近公共祖先。解决方案为进行一次数据的预处理，

对提供的两个树节点利用预处理好的数据可以快速的寻找到最近公共祖先节点。



## 一、数据的预处理

一般所给予的数据为一个edges数组，且数组中元素为一个连通边，例如：{{0, 1}, {1, 2}, {3,5}, {3,4}}, 每一个数代

表一个节点，注意edges数组所表示的是一个树图，那么edges中每个数字最多出现三次，即两个子节点和父节点。

首先n个节点表示为 0~n-1(如果实际的节点不是这样需要使用哈希映射)，那我们设节点0为根节点，建立一个二

维数组g，g[x]表示与节点x相连通的节点(父节点和子节点)。

````C++
/* n表示节点数，edges为树图的邻接表，节点为0~n-1 */
vector<vector<int>> preprocessData(int n,vector<vector<int>>& edges){
    vector<vector<int>> g(n);
    for(auto& edge : edges){
        g[edge[0]].push_back(edge[1]);
        g[edge[1]].push_back(edge[0]);
    }
    return g;
}

````

根据数组g我们可以建立一个parent数组，parent[x]表示x的父节点(设parent[0]=-1)，并且在建立parent数组的同

时我们还可以建立一个depth数组，depth[x]表示x节点以节点0为根节点的深度(depth[0]=0)。

````C++
/*
* 只需执行dfs(0, -1);即可预处理完成parent和depth
*/
void dfs(vector<int>& parent, vector<int>& depth,vector<vector<int>>& g, int x, int father){
    parent[x] = father;
    for(auto y : g[x]){
        if(y!=father){ // 排除x的父节点
            depth[y] = depth[x]+1;
            dfs(parent,depth,g,y,x);
        }
    }
}
````

根据parent和depth我们可以找到节点x和节点y的LCA，首先令x和y置为同一高度，然后一起向上找祖先节点，直到

x==y的时候结束，此时x就是最近公共祖先，但是这样寻找其实是比较慢的，假如x和y位于同一高度，若它们的LCA需要向上找13次，13的二进制为1101，可以按照这种跨度顺序查找(反之也可以) 8 -> 4 -> 1，但是如果还是使用parent数组，我们要寻找跨度8次的祖先节点还是需要向上遍历8次，因此需要新的parent数组。

我们将parent初始化为二维数组parent[x] [i]表示节点x的2^i个祖先节点(parent[x] [0]为节点x的父亲节点，

parent[x] [1]为节点x的爷爷节点，parnet[x] [2]为节点x的爷爷的爷爷节点)。

那么parent的大小为多少，易知行数为n(n个节点)，列数用极限法即可，假如所有的节点都位于一条线上，取最后

一个节点，求到它向上寻找2^i个节点中i的最大有意义值即可，即n转化为二进制时最前面的1所在位置(从右到左)

索引即是parent的列数

````C++
/*
* int m = 32 - __builtin_clz(n); // __builtin_clz函数获取前导零的个数
* vector<vector<int>> parent(n, vector<int>(m,-1)); // 不存在的置为-1
* vector<int> depth(n,0);
*/


/* 先预处理所有节点第2^0的父节点 */
void dfs(vector<vector<int>>& parent, vector<int>& depth,vector<vector<int>>& g, int x, int father){
    parent[x][0] = father;
    for(auto y : g[x]){
        if(y!=father){ // 排除x的父节点
            depth[y] = depth[x]+1;
            dfs(parent,depth,g,y,x);
        }
    }
}


/* 
* 利用动态规划处理parent的剩余部分(例：当前节点的爷爷节点等于它父节点的父节点)
* parent[x][i] = parent[parent[x][i-1]][i-1] 
*/
void fillParent(vector<vector<int>>& parent, int n, int m){
    for(int i=1;i<m;++i){
        for(int x=0;x<n;++x){
            int t = parent[x][i-1];
            if(t!=-1){
                parent[x][i] = parent[t][i-1];
            }
        }
    }
}

````





## 二、利用预处理好的数据获取LCA

如果要获取x和y的最近公共祖先节点，需要令x和y置于同一高度，然后同步上升



````c++

/* 获取节点x和节点y的最近公共祖先 */
int getLCA(vector<vector<int>>& parent, vector<int>& depth, int x, int y){
    // 令y为更深的节点
    if(depth[x]>depth[y]){swap(x,y);}

    // 令x和y在同一深度
    int height_diff = depth[y] - depth[x];
    int k = 32 - __builtin_clz(height_diff);
    for(int i=0;i<k;++i){
        if(k&(1<<i)){
            y = parent[y][i];
        }
    }
    /*
    // 令x和y在同一深度的另一种方法
    // k&(k-1)是将k与k-1进行按位与操作，这会将k中最右侧的1所在的位置设为0，而保留其他位的值
    for(int k=depth[y] - depth[x]; k; k&=k-1){
        int i = __builtin_ctz(k); // __builtin_ctz函数获取后导零个数
        y = pa[y][i];
    }
    */

    if(y!=x){ //如果 x==y，说明x是y的祖节点，所以x的父节点即是LCA
     /*
      * 设parent的列数为 9 列
      * 假如x和y距离LCA一步一步跳的步数转化为二进制为 0 1 1 0 0 1 1 1 0(9位)
      * 我们只需要遍历 9 次即可确定，先跳2^8步数，发现parent[x][8]==parent[y][8]                             * 则其值为-1或者祖先节点，那么就继续遍历，跳2^7步，会发现 parent[x][7]!=parent[y][7]
      * 就令x=parent[x][7],y=parent[y][7],然后继续循环...当循环到 2^1 时就会碰到LCA，
      * 但是parent[x][1]==parent[y][1]，所以跳过，然后循环到 2^0时parent[x][0]!=parent[y][0]
      * 就令x=parent[x][0],y=parent[y][0]，x和y会成为LCA的子节点,所以x的父节点即是LCA
      */
        for(int i = parent[0].size()-1;i>=0;--i){
            int px = parent[x][i],py=parent[y][i];
            if(px != py){
                x=px;y=py;
            }
        }
        x = parent[x][0]; // x的父节点即是LCA
    }
    int LCA = x;
    return LCA;
}
````



