# Union-findDisjointSets





```C++
// 大致一个模板，可以看看，说实话我对此了解太少


#include<iostream>
#include<vector>
using namespace std;

const int MAX = 4;
 
void Initialise(vector<int>& parent) {
	int i;
	for (int i = 0; i < MAX; ++i) {
		parent[i] = i;
	}
}

int FindRoot(int x, vector<int>& parent) {
	int x_root = x;
	while (parent[x_root] != x_root) {
		x_root = parent[x_root];
	}
	return x_root;
}

bool UnionVertices(int x,int y,vector<int>& parent,vector<int>& rank) {
	int x_root = FindRoot(x, parent);
	int y_root = FindRoot(y, parent);
	if (x_root == y_root) { return false; }
	if (rank[x_root] > rank[y_root]) { parent[y_root] = x_root; }
	else if(rank[x_root] < rank[y_root]) { parent[x_root] = y_root; }
	else { parent[x_root] = y_root; ++rank[y_root]; }
	return true;
}



int main() {
	vector<int> parent(MAX, 0);
	vector<int> rank(MAX, 0);
	Initialise(parent);
	vector<vector<int>> arr{
		{0,2},{0,1},{3,0}
	};
	for (int i = 0;i<3; ++i) {
		int x = arr[i][0];
		int y = arr[i][1];
		if (!UnionVertices(x, y, parent,rank)) {
			cout << "YES" << endl;	
			return 0;
		}
	}
	for (int i : parent) { cout << i << " " << endl; }
	cout << "ON" << endl;
	return 0;
}


```

