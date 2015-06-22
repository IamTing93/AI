#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <cmath>

#define FILE_PATH "train.csv"

// 属性的数量
const int ATTRIBUTE_NUM = 617;

// 随机属性的数量 << 属性的数量
const int RANDOM_ATTRBUTE_NUM = (int)sqrt(ATTRIBUTE_NUM);

// 训练样例的数量
const int TRAINING_EXAMPLES_NUM = 6238;

// 分类种类的数量
const int LABEL_NUM = 26;

// 一行数据的长度 = id + attributes + label
const int COLUMN_LENGTH = ATTRIBUTE_NUM + 2;

// 线程的数量
const int THREAD_NUM = 100;

#endif