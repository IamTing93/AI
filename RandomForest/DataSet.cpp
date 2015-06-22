#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#include "DataSet.h"

// ���캯��
DataSet::DataSet() {}

// ��������
DataSet* DataSet::instance = NULL;

DataSet* DataSet::getInstance() {
	if (instance == NULL) {
		instance = new DataSet();
	}
	return instance;
}

bool DataSet::readFile(const string& path, int choice) {
	string line, number;
	ifstream f(path, ifstream::_Nocreate);
	if (f.fail()) {
		cout << "Cannot open file" << endl;
		return false;
	}
	
	// �ѿ�ͷ�������У�����value0, value2, ..., label�����˵�
	getline(f, line);
	
	vector<vector<int>>* vecPtr = &m_trainData;
	if (choice == 1) {
		vecPtr = &m_testData;
	}
	
	// �������ݣ����ұ��浽������
	while (getline(f, line)) {
		istringstream is(line);
		vector<int> data_row;
		while (getline(is, number, ',')) {
			data_row.push_back(atoi(number.c_str()));
		}
		(*vecPtr).push_back(data_row);
	}

	f.close();
	return true;
}

// �Ѿ��ߵĽ��������ļ�
void DataSet::writeFile(const string& path, const vector<int>& decision) {
	FILE* outputFile;
	// ��Ҫ������ļ�
	fopen_s(&outputFile, path.c_str(), "wb");
	fprintf(outputFile, "id,label\n");

	// ��������
	int size_of_buff = 8;
	char* str = new char[size_of_buff];

	// ������ߵĽ��
	for (int row = 0; row < decision.size(); ++row) {
		// ������id
		sprintf_s(str, size_of_buff, "%d,", row);
		fprintf(outputFile, str);

		// ���߽��
		sprintf_s(str, size_of_buff, "%d\n", decision[row]);
		fprintf(outputFile, str);
	}

	fclose(outputFile);
}

// ��ӡ���������Ч����̫��
void DataSet::printData() {
	if (m_trainData.size() != 0) {
		for (int i = 0; i < m_trainData.size(); ++i) {
			for (vector<int>::iterator it = m_trainData[i].begin(); it != m_trainData[i].end(); ++it) {
				cout << *it << ' ';
			}
			cout << endl;
		}
	}
}

vector<vector<int>>* DataSet::getTrainData() {
	return &m_trainData;
}

vector<vector<int>>* DataSet::getTestData() {
	return &m_testData;
}

void DataSet::freeInstance() {
	delete instance;
	instance = 0;
}