#include <ctime>
#include <cmath>
#include <iostream>
#include "DecisionTree.h"

// 设置原始样例数据集
void DecisionTree::setSamples(vector<vector<int>>* samples) {
	m_samples = samples;
}

// 随机有放回地选取以构成子样例集
void DecisionTree::setRandomSamplesNum() {
	srand((unsigned)time(0));
	for (int i = 0; i < TRAINING_EXAMPLES_NUM; ++i) {
		m_randomSamplesNum.insert(rand() % TRAINING_EXAMPLES_NUM);
	}
}

// 随机选取属性（非有放回选取）
void DecisionTree::setRandomAttributesNum(const int num) {
	srand((unsigned)time(0));
	while (m_randomAttributesNum.size() <= num) {
		m_randomAttributesNum.insert(rand() % ATTRIBUTE_NUM);
	}
}

// 根据数据样例得到属性与它的值的map
void DecisionTree::computeMapFrom2DVector() {
	set<int> values;
	for (set<int>::iterator attribute = m_randomAttributesNum.begin(); attribute != m_randomAttributesNum.end(); ++attribute) {
		// 根据随机抽取样例的标识来寻找指定的样例
		for (set<int>::iterator row = m_randomSamplesNum.begin(); row != m_randomSamplesNum.end(); ++row) {
			values.insert((*m_samples)[*row][*attribute + 1]);
		}

		// 这里属性的序号从value0开始
		map_attibure_values[*attribute] = values;
		values.clear();
	}
}

// 根据公式计算熵
double DecisionTree::computeEntropy(set<int>& remainRandomSamples, int attribute, int values, bool ifparent) {
	vector<int> count(LABEL_NUM + 1, 0); // 记录分类的个数，count[0]不要，从count[1]开始
	int column = attribute + 1;

	for (set<int>::iterator row = remainRandomSamples.begin(); row != remainRandomSamples.end(); ++row) {
		if ((!ifparent && ((*m_samples)[*row][column] == values)) || ifparent) { // ifparent记录是否计算父结点
			++count[(*m_samples)[*row][COLUMN_LENGTH - 1]]; // 对应的分类加1
		}
	}
	
	for (int i = 1; i < LABEL_NUM + 1; ++i) { // 全部样例都是同一分类
		if (count[i] == remainRandomSamples.size()) {
			return 0;
		}
	}

	// 不是同一分类的，则需要计算熵
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

// 计算信息增益
double DecisionTree::computeGain(set<int>& remainRandomSamples, int attribute) {
	// 首先求不做划分时的熵
	double parentEntropy = computeEntropy(remainRandomSamples, attribute, blank, true);

	// 然后求做划分后各个值得熵
	double childrenEntropy = 0;
	set<int> values = map_attibure_values[attribute];
	vector<double> rate;
	vector<int> countValues;
	int column = attribute + 1;
	// set是有序的集合，因此这里的对应关系是一种弱的逻辑对应关系
	for (set<int>::iterator it = values.begin(); it != values.end(); ++it) {
		int counter = 0;
		for (set<int>::iterator row = remainRandomSamples.begin(); row != remainRandomSamples.end(); ++row) {
			if ((*m_samples)[*row][column] == *it) {
				counter++;
			}
		}
		// 记录属性对应的各种值在随机样例中出现的次数
		countValues.push_back(counter);
	}

	// 计算属性的各种值占原样例的比例
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

// 找出样例中占多数的分类
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

// 判断样例的分类是否都为label
bool DecisionTree::allTheSameLabel(set<int>& remainSamples, int label) {
	for (set<int>::iterator row = remainSamples.begin(); row != remainSamples.end(); ++row) {
		if ((*m_samples)[*row][COLUMN_LENGTH - 1] != label) {
			return false;
		}
	}
	return true;
}

// 计算信息增益，DFS构建决策树
// current_node为当前的节点
// remainSamples为剩余待分类的样例
// remainAttribute为剩余还没有考虑的属性
Node* DecisionTree::buildDecisionTreeDFS(Node* p, set<int>& remainSamples, set<int>& remainAttribute) {
	// 指针为空
	if (p == NULL) {
		p = new Node();
	}

	// 判断剩余样例的label是否相同，相同则该结点为叶子结点
	for (int label = 1; label <= 26; ++label) {
		if (allTheSameLabel(remainSamples, label)) {
			p->attribute = label;
			return p;
		}
	}

	// 所有属性均已考虑完毕，但还有样例没有分尽
	if (remainAttribute.size() == 0) {
		int mostLabel = mostCommonLabel(remainSamples);
		p->attribute = mostLabel;
		return p;
	}

	// 最大的信息增益
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

	// 下面根据max_it所指向的属性来划分当前样例，更新样例集和属性集
	set<int> newAttributes;
	set<int> newSamples;
	for (set<int>::iterator tmp_it = remainAttribute.begin(); tmp_it != remainAttribute.end(); ++tmp_it) {
		if (*max_it != *tmp_it) {
			newAttributes.insert(*tmp_it);
		}
	}

	// 划分后保存最佳的划分属性
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

		// 若果当前没有这个分支的样例，当前的newNode为叶子结点 
		if (newSamples.size() == 0) {
			newNode->attribute = mostCommonLabel(remainSamples);
		} else {
			buildDecisionTreeDFS(newNode, newSamples, newAttributes);
		}

		//递归函数返回时即回溯时需要将新结点加入父节点孩子容器，并且清除newSamples容器
		p->children.push_back(newNode);
		newSamples.clear();
	}

	return p;
}

void DecisionTree::printTree(Node* p) {
	if (p->children.size() == 0) {
		cout << "叶子结点: ";
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
		// 如果p不是叶子结点
		while (p->children.size() != 0) {
			int value = (*testSamples)[row][p->attribute + 1];
			// 寻找到达值与样例相同的子结点
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


// 删除决策树
void DecisionTree::freeTree(Node* p) {
	if (p != NULL) {
		for (vector<Node*>::iterator it = p->children.begin(); it != p->children.end(); ++it) {
			freeTree(*it);
		}
		delete p;
	}
}

// 返回随机抽取的样例集
set<int>& DecisionTree::getRandomSamplesNum() {
	setRandomSamplesNum();
	return m_randomSamplesNum;
}

// 返回随机选取的属性集
set<int>& DecisionTree::getRandomAttributesNum(const int num) {
	setRandomAttributesNum(num);
	return m_randomAttributesNum;
}