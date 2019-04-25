#pragma once
#ifndef  PLANE_TRANSFORM_H
#include <iostream>
#include <Eigen/Dense>
#include <math.h>
#include "ogrsf_frmts.h"
using namespace std;
using namespace Eigen;

void ReadShp(FILE*   fp_ShpFile, int &ShapeType);						//1.��ȡShapeFileͷ�ļ�
int ReadPointNum(FILE*   fp_ShpFile);									//2.��ȡ���ļ���ÿ���ߵ���ɲ�����������Ϣ
void ReadSingleLine(FILE*   fp_ShpFile, Vector3d *pt, int num);			//3.��ȡ�������е������
void MatrixTransform(Vector3d *pt, int num);							//4.�Ե��������ƽ��ת��
void WriteSingleLine(OGRLayer *poLayer, Vector3d *pt, int num);			//5.��ת��������꽨���µ���

OGRSpatialReference* GetProjInfoFromFile(const char * FileName);		//���Shapefile�ļ��Ŀռ�ο���Ϣ
unsigned long EndianBig2Little(int indata);								//�����մ�˴洢������ת��Ϊ����С�˴洢
#define PLANE_TRANSFORM_H
#endif // ! PLANE TRANSFORM_H
