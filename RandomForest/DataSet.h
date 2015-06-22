/* DataSet�ฺ�������� */
#ifndef _DATA_SET_H_
#define _DATA_SET_H_

#include <set>
#include <string>
#include <vector>
#include "Define.h"

using namespace std;

class DataSet {
public:
	static DataSet* getInstance();
	static void freeInstance();
	bool readFile(const string& path, int choice);
	void writeFile(const string& path, const vector<int>& decision);
	void printData();
	vector<vector<int>>* getTrainData();
	vector<vector<int>>* getTestData();


private:
	DataSet();
	static DataSet* instance;
	set<int> m_trainingExamples;
	vector<vector<int>> m_trainData; // ѵ�������Ķ�ά����
	vector<vector<int>> m_testData; // ѵ�������Ķ�ά����
};

#endif