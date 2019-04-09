#pragma once
#ifndef  QUADTREE_H
#define QUADTREE_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <vector>
#include <string>
#define NUM_OF_PIXEL 100
using namespace std;

//һ��ʹ�õĽ����
//1.�����洢������Ϣ��Ԫ�ؽ��
class myNode {
public:
	myNode() {}
	myNode(int m, int v) :morton(m), value(v) {}
	void Value(int v) { value = v; }
	void Morton(int m) { morton = m; }
	int value;
	long morton;
};
//2.�����洢�����Ĳ�����Ϣ�Ľ��
class LQTNode {
public:
	LQTNode(long m, int d, int v) :Morton(m), depth(d), value(v)
	{}
	LQTNode(int d, int v) :depth(d), value(v)
	{}
	const void Print() {
		printf("M Code: %3ld,\t Depth: %d,\tValue: %3d\n", Morton, depth, value);
	}
	long Morton;
	int depth;
	int value;
};

//����ʹ�õĺ���
void ReadArray(vector<myNode> &array, ifstream &file);			//��ȡ���󲢴��ڶ�̬������
long four_code(int col, int row);								//ͨ�����к�����M��
void four_decode(long Morton, int &col, int &row);				//ͨ��M��õ����к�
void four_mSequence(vector<myNode> &marr, const vector<myNode> &array, const int order);	//��M��˳����֯����
void four_vSequence(vector<myNode> &array, const vector<myNode> &matr, const int order);	//��ԭʼ����˳����֯����
void four_LQT(vector<LQTNode> &list, const vector<myNode> &matrix, const int layer);		//�����Ĳ���ѹ��
void four_LQTprint(vector<LQTNode> &list, int order);			//��LQT�л�ԭ����
int D2Binary(int d);				//ʮ����ת������
int B2Decimal(int b);				//������תʮ����

#endif // ! QUADTREE_H