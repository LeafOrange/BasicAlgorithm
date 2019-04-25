#pragma once
#ifndef  PLANE_TRANSFORM_H
#include <iostream>
#include <Eigen/Dense>
#include <math.h>
#include "ogrsf_frmts.h"
using namespace std;
using namespace Eigen;

void ReadShp(FILE*   fp_ShpFile, int &ShapeType);						//1.读取ShapeFile头文件
int ReadPointNum(FILE*   fp_ShpFile);									//2.读取线文件中每条线的组成部分与点个数信息
void ReadSingleLine(FILE*   fp_ShpFile, Vector3d *pt, int num);			//3.读取单条线中点的坐标
void MatrixTransform(Vector3d *pt, int num);							//4.对点坐标进行平面转换
void WriteSingleLine(OGRLayer *poLayer, Vector3d *pt, int num);			//5.用转换后的坐标建立新的线

OGRSpatialReference* GetProjInfoFromFile(const char * FileName);		//获得Shapefile文件的空间参考信息
unsigned long EndianBig2Little(int indata);								//将按照大端存储的数据转换为按照小端存储
#define PLANE_TRANSFORM_H
#endif // ! PLANE TRANSFORM_H
