# Trie

所谓字典树又称单词查找树，典型应用是用于统计，排序和保存（但不仅限于字符串），所以经常被搜索引擎系统用于文本词频统计。它的优点是：利用字符串的公共前缀来减少查询时间，最大限度地减少无谓的字符串比较，查询效率比哈希树高。



[TOC]



### 一、创建



```c++
/*
* 非常简单只需要一个struct即可
* 数组长度要看你所保存的字符串各种字符的种类，这里默认都是小写
* 根节点默认不存储值
*/

struct TrieNode {
	bool IsWord;  // 判断是否是某个单词的结尾
	vector<TrieNode*> next;
	TrieNode() :IsWord(false), next(vector<TrieNode*>(26, nullptr)) {}
};

```



### 二、存储



```C++
void Insert(string& word,TrieNode* root) { // 参数需要提供 root 节点
	TrieNode* location = root;
	for (int i = 0; i < word.size(); ++i) {
        // 在储存中初始化节点,若存在公共前缀就无需创造节点
		if (location->next[word[i] - 'a'] == nullptr) {
			TrieNode* temp = new TrieNode();
			location->next[word[i] - 'a'] = temp;
		}
		location = location->next[word[i] - 'a'];
	}
	location->IsWord = true; // 标记单词结尾
}

```



### 三、查找



```C++
bool Search(string& word, TrieNode* root) {
	TrieNode* location = root;
	for (int i = 0; i < word.size() && location; ++i) {
		location = location->next[word[i] - 'a'];
	}
	return location && location->IsWord;
}

```



### 四、Free

```c++
void DeleteTrie(TrieNode* root) {
	for (int i = 0; i < 26; ++i) {
		if (root->next[i] != nullptr) {
			DeleteTrie(root->next[i]);
		}
	}
	delete root;
}

```























