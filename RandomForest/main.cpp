/************************************************************************/
/*								  随机森林                              */
/************************************************************************/
/* 1.有放回地从训练样例中抽取等大小的新训练样例 */
/* 2.随机地从属性集A中挑选属性，构成新的子属性集a，大小的要求是a << A */
/* 3.生成决策树 */
/* 4.输入测试样例，对结果进行投票 */


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

	// 生成随机样例
	remainSamples = treeManager.getRandomSamplesNum();
	// 生成随机属性
	remainAttributes = treeManager.getRandomAttributesNum(attributesNum);

	// 设置原始样例集
	treeManager.setSamples(dataSet->getTrainData());

	treeManager.computeMapFrom2DVector();
	// 创建线程，生成决策树
	
	root = treeManager.buildDecisionTreeDFS(root, remainSamples, remainAttributes);

	// 进入临界区
	mutex1.lock();
	// 保存根结点
	if (root != NULL) {
		treeRoots.push_back(root);
	}
	else {
		cout << "线程的根结点为空" << endl;
	}
	// 离开
	mutex1.unlock();
	
}

void vote(Node* root, vector<vector<int>>* testSamples, vector<vector<int>>& decisionSet) {
	DecisionTree treeManager;
	vector<int> decision;
	treeManager.makeDecision(root, testSamples, decision);
	// 进入临界区
	mutex2.lock();
	decisionSet.push_back(decision);
	// 离开
	mutex2.unlock();
}

// 得到投票数最多的label 
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

// 从决策集中得到每个测试样例的label
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

	// 文件名输入	
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

	// 线程数
	int threadsNum = 0;
	do {
		cout << "How many threads do you want?" << endl;
		cin >> threadsNum;
		cout << endl;
		if (threadsNum <= 0) {
			cout << "Are you kidding me?\n" << endl;
		}
	} while (threadsNum <= 0);

	// 随机属性的数量
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

	// 读入训练数据并且多线程训练决策树
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

	// 读入测试数据并且多线程做出决策
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
	// 获取结果
	cout << "Voting!" << endl;
	if (decisionSet.size() > 0) {
		getResult(decisionSet, result);
	}

	// 输出结果
	cout << "Writing!" << endl;
	dataSet->writeFile(outputFilePath, result);

	cout << "Over!" << endl;
	system("pause");

	// 程序结束，销毁对象
	for (auto root : treeRoots) {
		DecisionTree::freeTree(root);
	}
	DataSet::freeInstance();
	return 0;
}