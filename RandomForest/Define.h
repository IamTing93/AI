#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <cmath>

#define FILE_PATH "train.csv"

// ���Ե�����
const int ATTRIBUTE_NUM = 617;

// ������Ե����� << ���Ե�����
const int RANDOM_ATTRBUTE_NUM = (int)sqrt(ATTRIBUTE_NUM);

// ѵ������������
const int TRAINING_EXAMPLES_NUM = 6238;

// �������������
const int LABEL_NUM = 26;

// һ�����ݵĳ��� = id + attributes + label
const int COLUMN_LENGTH = ATTRIBUTE_NUM + 2;

// �̵߳�����
const int THREAD_NUM = 100;

#endif