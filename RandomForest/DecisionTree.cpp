#include <ctime>
#include <cmath>
#include <iostream>
#include "DecisionTree.h"

// ����ԭʼ�������ݼ�
void DecisionTree::setSamples(vector<vector<int>>* samples) {
	m_samples = samples;
}

// ����зŻص�ѡȡ�Թ�����������
void DecisionTree::setRandomSamplesNum() {
	srand((unsigned)time(0));
	for (int i = 0; i < TRAINING_EXAMPLES_NUM; ++i) {
		m_randomSamplesNum.insert(rand() % TRAINING_EXAMPLES_NUM);
	}
}

// ���ѡȡ���ԣ����зŻ�ѡȡ��
void DecisionTree::setRandomAttributesNum(const int num) {
	srand((unsigned)time(0));
	while (m_randomAttributesNum.size() <= num) {
		m_randomAttributesNum.insert(rand() % ATTRIBUTE_NUM);
	}
}

// �������������õ�����������ֵ��map
void DecisionTree::computeMapFrom2DVector() {
	set<int> values;
	for (set<int>::iterator attribute = m_randomAttributesNum.begin(); attribute != m_randomAttributesNum.end(); ++attribute) {
		// ���������ȡ�����ı�ʶ��Ѱ��ָ��������
		for (set<int>::iterator row = m_randomSamplesNum.begin(); row != m_randomSamplesNum.end(); ++row) {
			values.insert((*m_samples)[*row][*attribute + 1]);
		}

		// �������Ե���Ŵ�value0��ʼ
		map_attibure_values[*attribute] = values;
		values.clear();
	}
}

// ���ݹ�ʽ������
double DecisionTree::computeEntropy(set<int>& remainRandomSamples, int attribute, int values, bool ifparent) {
	vector<int> count(LABEL_NUM + 1, 0); // ��¼����ĸ�����count[0]��Ҫ����count[1]��ʼ
	int column = attribute + 1;

	for (set<int>::iterator row = remainRandomSamples.begin(); row != remainRandomSamples.end(); ++row) {
		if ((!ifparent && ((*m_samples)[*row][column] == values)) || ifparent) { // ifparent��¼�Ƿ���㸸���
			++count[(*m_samples)[*row][COLUMN_LENGTH - 1]]; // ��Ӧ�ķ����1
		}
	}
	
	for (int i = 1; i < LABEL_NUM + 1; ++i) { // ȫ����������ͬһ����
		if (count[i] == remainRandomSamples.size()) {
			return 0;
		}
	}

	// ����ͬһ����ģ�����Ҫ������
	double sum = remainRandomSamples.size();
	double entropy = 0;
	for (int i = 1; i < LABEL_NUM + 1; ++i) {
		if (count[i] != 0) {
			double rate = count[i] / sum;
			entropy -= rate * log2(rate);
		}
	}

	return entropy;
}

// ������Ϣ����
double DecisionTree::computeGain(set<int>& remainRandomSamples, int attribute) {
	// ������������ʱ����
	double parentEntropy = computeEntropy(remainRandomSamples, attribute, blank, true);

	// Ȼ���������ֺ����ֵ����
	double childrenEntropy = 0;
	set<int> values = map_attibure_values[attribute];
	vector<double> rate;
	vector<int> countValues;
	int column = attribute + 1;
	// set������ļ��ϣ��������Ķ�Ӧ��ϵ��һ�������߼���Ӧ��ϵ
	for (set<int>::iterator it = values.begin(); it != values.end(); ++it) {
		int counter = 0;
		for (set<int>::iterator row = remainRandomSamples.begin(); row != remainRandomSamples.end(); ++row) {
			if ((*m_samples)[*row][column] == *it) {
				counter++;
			}
		}
		// ��¼���Զ�Ӧ�ĸ���ֵ����������г��ֵĴ���
		countValues.push_back(counter);
	}

	// �������Եĸ���ֵռԭ�����ı���
	for (int i = 0; i < values.size(); ++i) {
		rate.push_back((double)countValues[i] / (double)remainRandomSamples.size());
	}

	double tmpEntropy = 0;
	set<int>::iterator it = values.begin();
	for (int ptr = 0; ptr < values.size(); ++ptr, ++it) {
		tmpEntropy = computeEntropy(remainRandomSamples, attribute, *it, false);
		childrenEntropy += rate[ptr] * tmpEntropy;
	}

	return (parentEntropy - childrenEntropy);
}

// �ҳ�������ռ�����ķ���
int DecisionTree::mostCommonLabel(set<int>& remainSamples) {
	vector<int> count(LABEL_NUM + 1, 0);
	for (set<int>::iterator row = remainSamples.begin(); row != remainSamples.end(); ++row) {
		++count[(*m_samples)[*row][COLUMN_LENGTH - 1]];
	}
	int mostLabel = 0;
	int largest = 0;
	for (int i = 1; i < LABEL_NUM + 1; ++i) {
		if (count[i] > largest) {
			mostLabel = i;
			largest = count[i];
		}
	}
	return mostLabel;
}

// �ж������ķ����Ƿ�Ϊlabel
bool DecisionTree::allTheSameLabel(set<int>& remainSamples, int label) {
	for (set<int>::iterator row = remainSamples.begin(); row != remainSamples.end(); ++row) {
		if ((*m_samples)[*row][COLUMN_LENGTH - 1] != label) {
			return false;
		}
	}
	return true;
}

// ������Ϣ���棬DFS����������
// current_nodeΪ��ǰ�Ľڵ�
// remainSamplesΪʣ������������
// remainAttributeΪʣ�໹û�п��ǵ�����
Node* DecisionTree::buildDecisionTreeDFS(Node* p, set<int>& remainSamples, set<int>& remainAttribute) {
	// ָ��Ϊ��
	if (p == NULL) {
		p = new Node();
	}

	// �ж�ʣ��������label�Ƿ���ͬ����ͬ��ý��ΪҶ�ӽ��
	for (int label = 1; label <= 26; ++label) {
		if (allTheSameLabel(remainSamples, label)) {
			p->attribute = label;
			return p;
		}
	}

	// �������Ծ��ѿ�����ϣ�����������û�з־�
	if (remainAttribute.size() == 0) {
		int mostLabel = mostCommonLabel(remainSamples);
		p->attribute = mostLabel;
		return p;
	}

	// ������Ϣ����
	double maxGain = 0;
	double tmpGain;

	set<int>::iterator max_it = remainAttribute.begin();
	for (set<int>::iterator tmp_it = remainAttribute.begin(); tmp_it != remainAttribute.end(); ++tmp_it) {
		tmpGain = computeGain(remainSamples, *tmp_it);
		if (tmpGain > maxGain){
			maxGain = tmpGain;
			max_it = tmp_it;
		}
	}

	// �������max_it��ָ������������ֵ�ǰ���������������������Լ�
	set<int> newAttributes;
	set<int> newSamples;
	for (set<int>::iterator tmp_it = remainAttribute.begin(); tmp_it != remainAttribute.end(); ++tmp_it) {
		if (*max_it != *tmp_it) {
			newAttributes.insert(*tmp_it);
		}
	}

	// ���ֺ󱣴���ѵĻ�������
	p->attribute = *max_it;
	
	int column = *max_it + 1;
	set<int> values = map_attibure_values[*max_it];

	for (set<int>::iterator it = values.begin(); it != values.end(); ++it) {
		for (set<int>::iterator row = remainSamples.begin(); row != remainSamples.end(); ++row) {
			if ((*m_samples)[*row][column] == *it) {
				newSamples.insert(*row);
			}
		}

		Node* newNode = new Node();
		newNode->arrivedValue = *it;

		// ������ǰû�������֧����������ǰ��newNodeΪҶ�ӽ�� 
		if (newSamples.size() == 0) {
			newNode->attribute = mostCommonLabel(remainSamples);
		} else {
			buildDecisionTreeDFS(newNode, newSamples, newAttributes);
		}

		//�ݹ麯������ʱ������ʱ��Ҫ���½����븸�ڵ㺢���������������newSamples����
		p->children.push_back(newNode);
		newSamples.clear();
	}

	return p;
}

void DecisionTree::printTree(Node* p) {
	if (p->children.size() == 0) {
		cout << "Ҷ�ӽ��: ";
	}

	cout << p->attribute << endl;

	for (vector<Node*>::iterator it = p->children.begin(); it != p->children.end(); ++it) {
		printTree(*it);
	}
}

void DecisionTree::makeDecision(Node* root, vector<vector<int>>* testSamples, vector<int>& decision) {
	Node* p = NULL;
	for (int row = 0; row < (*testSamples).size(); ++row) {
		p = root;
		// ���p����Ҷ�ӽ��
		while (p->children.size() != 0) {
			int value = (*testSamples)[row][p->attribute + 1];
			// Ѱ�ҵ���ֵ��������ͬ���ӽ��
			for (int ptr = 0; ptr < p->children.size(); ++ptr) {
				if (p->children[ptr]->arrivedValue == value) {
					p = p->children[ptr];
					break;
				}
			}
		}
		decision.push_back(p->attribute);
	}
}


// ɾ��������
void DecisionTree::freeTree(Node* p) {
	if (p != NULL) {
		for (vector<Node*>::iterator it = p->children.begin(); it != p->children.end(); ++it) {
			freeTree(*it);
		}
		delete p;
	}
}

// ���������ȡ��������
set<int>& DecisionTree::getRandomSamplesNum() {
	setRandomSamplesNum();
	return m_randomSamplesNum;
}

// �������ѡȡ�����Լ�
set<int>& DecisionTree::getRandomAttributesNum(const int num) {
	setRandomAttributesNum(num);
	return m_randomAttributesNum;
}