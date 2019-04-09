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

//一、使用的结点类
//1.用来存储矩阵信息的元素结点
class myNode {
public:
	myNode() {}
	myNode(int m, int v) :morton(m), value(v) {}
	void Value(int v) { value = v; }
	void Morton(int m) { morton = m; }
	int value;
	long morton;
};
//2.用来存储线性四叉树信息的结点
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

//二、使用的函数
void ReadArray(vector<myNode> &array, ifstream &file);			//读取矩阵并存在动态数组内
long four_code(int col, int row);								//通过行列号生成M码
void four_decode(long Morton, int &col, int &row);				//通过M码得到行列号
void four_mSequence(vector<myNode> &marr, const vector<myNode> &array, const int order);	//按M码顺序组织数组
void four_vSequence(vector<myNode> &array, const vector<myNode> &matr, const int order);	//按原始行列顺序组织数组
void four_LQT(vector<LQTNode> &list, const vector<myNode> &matrix, const int layer);		//线性四叉树压缩
void four_LQTprint(vector<LQTNode> &list, int order);			//从LQT中还原数据
int D2Binary(int d);				//十进制转二进制
int B2Decimal(int b);				//二进制转十进制

#endif // ! QUADTREE_H