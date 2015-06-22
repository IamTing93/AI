#ifndef _DECISION_TREE_H_
#define _DECISION_TREE_H_

#include <vector>
#include <string>
#include <map>
#include <set>
#include "Define.h"

using namespace std;

const int blank = 10000;

struct Node {
	int attribute;
	int arrivedValue;
	vector<Node*> children;
	Node() {
		attribute = blank;
		arrivedValue = blank;
	}
};

class DecisionTree {
public:
	// 设置原始样例数据集
	void setSamples(vector<vector<int>>* samples);

	// 随机抽取样例
	void setRandomSamplesNum();

	// 随机选取属性
	void setRandomAttributesNum(const int num);

	// 根据数据样例得到属性与它的值的map
	void computeMapFrom2DVector();

	// 计算信息增益，DFS构建决策树
	Node* buildDecisionTreeDFS(Node* p, set<int>& remainSamples, set<int>& remainAttribute);

	// 打印决策树
	void printTree(Node* p);

	// 删除决策树
	static void freeTree(Node* p);

	// 返回随机抽取的样例集
	set<int>& getRandomSamplesNum();

	// 返回随机选取属性集
	set<int>& getRandomAttributesNum(const int num);

	// 做出决策
	void makeDecision(Node* root, vector<vector<int>>* testSamples, vector<int>& decision);

private:

	// 根据具体属性和值来计算熵
	double computeEntropy(set<int>& remainRandomSamples, int attribute, int values, bool ifparent);

	// 按照属性attribute划分的当前剩余样例来计算信息增益
	double computeGain(set<int>& remainRandomSamples, int attribute);

	// 找出样例中占多数的分类
	int mostCommonLabel(set<int>& remainSamples);

	// 判断样例的分类是否都为label
	bool allTheSameLabel(set<int>& m_randomSamplesNum, int label);

	vector<vector<int>>* m_samples; // 样例集
	vector<int> m_sampleRow; // 对应一行的样例数据
	set<int> m_randomSamplesNum; // 有放回地抽取样例数据,标记选取了哪些样例数据
	map<int, set<int>> map_attibure_values; // 存储属性对应的所有的值
	set<int> m_randomAttributesNum; // 随机选取属性

};

#endif