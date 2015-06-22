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
	// ����ԭʼ�������ݼ�
	void setSamples(vector<vector<int>>* samples);

	// �����ȡ����
	void setRandomSamplesNum();

	// ���ѡȡ����
	void setRandomAttributesNum(const int num);

	// �������������õ�����������ֵ��map
	void computeMapFrom2DVector();

	// ������Ϣ���棬DFS����������
	Node* buildDecisionTreeDFS(Node* p, set<int>& remainSamples, set<int>& remainAttribute);

	// ��ӡ������
	void printTree(Node* p);

	// ɾ��������
	static void freeTree(Node* p);

	// ���������ȡ��������
	set<int>& getRandomSamplesNum();

	// �������ѡȡ���Լ�
	set<int>& getRandomAttributesNum(const int num);

	// ��������
	void makeDecision(Node* root, vector<vector<int>>* testSamples, vector<int>& decision);

private:

	// ���ݾ������Ժ�ֵ��������
	double computeEntropy(set<int>& remainRandomSamples, int attribute, int values, bool ifparent);

	// ��������attribute���ֵĵ�ǰʣ��������������Ϣ����
	double computeGain(set<int>& remainRandomSamples, int attribute);

	// �ҳ�������ռ�����ķ���
	int mostCommonLabel(set<int>& remainSamples);

	// �ж������ķ����Ƿ�Ϊlabel
	bool allTheSameLabel(set<int>& m_randomSamplesNum, int label);

	vector<vector<int>>* m_samples; // ������
	vector<int> m_sampleRow; // ��Ӧһ�е���������
	set<int> m_randomSamplesNum; // �зŻصس�ȡ��������,���ѡȡ����Щ��������
	map<int, set<int>> map_attibure_values; // �洢���Զ�Ӧ�����е�ֵ
	set<int> m_randomAttributesNum; // ���ѡȡ����

};

#endif