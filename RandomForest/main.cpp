/************************************************************************/
/*								  ���ɭ��                              */
/************************************************************************/
/* 1.�зŻصش�ѵ�������г�ȡ�ȴ�С����ѵ������ */
/* 2.����ش����Լ�A����ѡ���ԣ������µ������Լ�a����С��Ҫ����a << A */
/* 3.���ɾ����� */
/* 4.��������������Խ������ͶƱ */


#include <stdlib.h>
#include <iostream>
#include <set>
#include <thread>
#include <memory.h>
#include <mutex>
#include "DataSet.h"
#include "DecisionTree.h"

using namespace std;

std::mutex mutex1, mutex2;

void getDecisionTrees(vector<Node*>& treeRoots, DataSet* dataSet, int attributesNum) {
	set<int> remainSamples;
	set<int> remainAttributes;
	Node* root = NULL;
	DecisionTree treeManager;

	// �����������
	remainSamples = treeManager.getRandomSamplesNum();
	// �����������
	remainAttributes = treeManager.getRandomAttributesNum(attributesNum);

	// ����ԭʼ������
	treeManager.setSamples(dataSet->getTrainData());

	treeManager.computeMapFrom2DVector();
	// �����̣߳����ɾ�����
	
	root = treeManager.buildDecisionTreeDFS(root, remainSamples, remainAttributes);

	// �����ٽ���
	mutex1.lock();
	// ��������
	if (root != NULL) {
		treeRoots.push_back(root);
	}
	else {
		cout << "�̵߳ĸ����Ϊ��" << endl;
	}
	// �뿪
	mutex1.unlock();
	
}

void vote(Node* root, vector<vector<int>>* testSamples, vector<vector<int>>& decisionSet) {
	DecisionTree treeManager;
	vector<int> decision;
	treeManager.makeDecision(root, testSamples, decision);
	// �����ٽ���
	mutex2.lock();
	decisionSet.push_back(decision);
	// �뿪
	mutex2.unlock();
}

// �õ�ͶƱ������label 
int getMostLabel(int* arr) {
	int mostLabel = -1;
	int counter = -1;
	for (int i = 1; i < LABEL_NUM + 1; ++i) {
		if (arr[i] > counter) {
			counter = arr[i];
			mostLabel = i;
		}
	}
	return mostLabel;
}

// �Ӿ��߼��еõ�ÿ������������label
void getResult(vector<vector<int>>& decisionSet, vector<int>& result) {
	int countArr[LABEL_NUM + 1];
	for (int column = 0; column < decisionSet[0].size(); ++column) {
		memset(countArr, 0, sizeof(countArr));
		for (int row = 0; row < decisionSet.size(); ++row) {
			++countArr[decisionSet[row][column]];
		}
		result.push_back(getMostLabel(countArr));
	}
}


int main() {
	DataSet* dataSet = DataSet::getInstance();

	// �ļ�������	
	string trainPath , testPath, outputFilePath;
	cout << "Input the whole name of training file: ";
	cin >> trainPath;
	cout << endl;
	cout << "Input the whole name of testing file: ";
	cin >> testPath;
	cout << endl;
	cout << "Input the whole name of output file: ";
	cin >> outputFilePath;
	cout << endl;

	// �߳���
	int threadsNum = 0;
	do {
		cout << "How many threads do you want?" << endl;
		cin >> threadsNum;
		cout << endl;
		if (threadsNum <= 0) {
			cout << "Are you kidding me?\n" << endl;
		}
	} while (threadsNum <= 0);

	// ������Ե�����
	int randomAttributeNum = 0;
	do {
		cout << "How many attributes to build the decision trees?" << endl;
		cin >> randomAttributeNum;
		cout << endl;
		if (randomAttributeNum <= 0 || randomAttributeNum > ATTRIBUTE_NUM) {
			cout << "Wrong number" << endl;
		}
	} while (randomAttributeNum <= 0 || randomAttributeNum > ATTRIBUTE_NUM);

	/*
	string trainPath = "trainingData.csv";
	string testPath = "testingData.csv";
	string outputFilePath = "decision.csv";
*/


	vector<Node*> treeRoots;
	vector<thread> trainingThreads;

	// ����ѵ�����ݲ��Ҷ��߳�ѵ��������
	cout << "Reading training data!" << endl;
	if (dataSet->readFile(trainPath, 0)) {
		cout << "Training the decision trees!" << endl;
		for (int i = 0; i < threadsNum; ++i) {
			trainingThreads.push_back(thread(getDecisionTrees, std::ref(treeRoots), dataSet, randomAttributeNum));
		}

		for (auto& thread : trainingThreads){
			thread.join();
		}
	}

	vector<vector<int>> decisionSet;
	vector<thread> testingThreads;

	// ����������ݲ��Ҷ��߳���������
	cout << "Reading testing data!" << endl;
	if (dataSet->readFile(testPath, 1)) {
		cout << "Make decision!" << endl;
		for (auto root : treeRoots) {
			testingThreads.push_back(thread(vote, root, dataSet->getTestData(), std::ref(decisionSet)));
		}

		for (auto& thread : testingThreads){
			thread.join();
		}
	}

	vector<int> result;
	// ��ȡ���
	cout << "Voting!" << endl;
	if (decisionSet.size() > 0) {
		getResult(decisionSet, result);
	}

	// ������
	cout << "Writing!" << endl;
	dataSet->writeFile(outputFilePath, result);

	cout << "Over!" << endl;
	system("pause");

	// ������������ٶ���
	for (auto root : treeRoots) {
		DecisionTree::freeTree(root);
	}
	DataSet::freeInstance();
	return 0;
}