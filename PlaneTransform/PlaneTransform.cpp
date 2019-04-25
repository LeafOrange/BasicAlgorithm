#include "PlaneTransform.h"


//1.读取ShapeFile头文件
	void ReadShp (FILE*   fp_ShpFile , int &ShapeType)
	{
		   //读取坐标文件头的内容开始
		   int FileCode;
		   int Unused;
		   int FileLength;
		   int Version;
		   double Xmin,Ymin,Xmax,Ymax,Zmin,Zmax,Mmin,Mmax;

		   fread(&FileCode,     sizeof(int),   1,fp_ShpFile);
		   FileCode = EndianBig2Little(FileCode);
		   for (int i = 0; i < 5; i++)
				  fread(&Unused,sizeof(int),   1,fp_ShpFile);
		   fread(&FileLength,   sizeof(int),   1,fp_ShpFile);
		   FileLength = EndianBig2Little(FileLength);

		   fread(&Version,          sizeof(int),   1,fp_ShpFile);
		   fread(&ShapeType,    sizeof(int),   1,fp_ShpFile);
		   fread(&Xmin,         sizeof(double),1,fp_ShpFile);
		   fread(&Ymin,         sizeof(double),1,fp_ShpFile);
		   fread(&Xmax,         sizeof(double),1,fp_ShpFile);
		   fread(&Ymax,         sizeof(double),1,fp_ShpFile);
		   fread(&Zmin,         sizeof(double),1,fp_ShpFile);
		   fread(&Zmax,        sizeof(double),1,fp_ShpFile);
		   fread(&Mmin,         sizeof(double),1,fp_ShpFile);
		   fread(&Mmax,         sizeof(double),1,fp_ShpFile);
		   //读取坐标文件头的内容结束
	}

	//2.读取线文件中每条线的组成部分与点个数信息
	int ReadPointNum(FILE*   fp_ShpFile)
	{
		int shapeType;
		double Box[4];
		int NumParts;
		int NumPoints;
		int Parts;

		fread(&shapeType, sizeof(int), 1, fp_ShpFile);
		//读Box
		for (int i = 0; i < 4; i++)
			fread(Box + i, sizeof(double), 1, fp_ShpFile);

		//读NumParts和NumPoints
		fread(&NumParts, sizeof(int), 1, fp_ShpFile);
		fread(&NumPoints, sizeof(int), 1, fp_ShpFile);
		fread(&Parts, sizeof(int), 1, fp_ShpFile);
		return NumPoints;
	}

	//3.读取单条线中点的坐标
	void ReadSingleLine(FILE*   fp_ShpFile, Vector3d *pt,int num)
	{
			//读Points
			double x, y;
			for (int j = 0; j < num; j++)
			{
				fread(&x, sizeof(double), 1, fp_ShpFile);
				fread(&y, sizeof(double), 1, fp_ShpFile);
				pt[j] << x, y, 1;
			}
	}

	//4.对点坐标进行平面转换
	void MatrixTransform(Vector3d *pt, int num)
	{
		Matrix3d TM1, TM2;
		TM1 << 2, 0, 0, 0, 2, 0, 0, 0, 1;
		TM2 << cos(M_PI / 3), sin(M_PI / 3), 0, -sin(M_PI / 3), cos(M_PI / 3), 0, 0, 0, 1;
		for (int i = 0; i < num; i++)
			pt[i] =TM2* TM1*pt[i];
		return;
	}

	//5.用转换后的坐标建立新的线
	void WriteSingleLine(OGRLayer *poLayer, Vector3d *pt, int num)
	{
		OGRFeature *poFeature;
		poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

		OGRLineString *line = new OGRLineString();
		for (int i = 0; i < num; i++)
			line->addPoint(pt[i][0], pt[i][1]);
		poFeature->SetGeometry(line);
		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
		{
			printf("Failed to create feature in shapefile.\n");
			exit(1);
		}
		OGRFeature::DestroyFeature(poFeature);
	}

	OGRSpatialReference* GetProjInfoFromFile(const char * FileName)
	{
		GDALDataset *poDS;
		poDS = (GDALDataset*)GDALOpenEx(FileName, GDAL_OF_VECTOR, NULL, NULL, NULL);
		OGRLayer *poLayer;
		poLayer = poDS->GetLayer(0);
		OGRSpatialReference *prj;
		prj = poLayer->GetSpatialRef();
		if (prj != NULL)
			return prj;
	}

	unsigned long EndianBig2Little(int indata)
	{//Big将高序字节存储在起始位置，Little将低序字节存储在起始位置
		char ss[9];
		char temp[8];
		unsigned long val = unsigned long(indata);

		//1. 将十六进制的数(val)转到一个字符串(ss)中
		_ultoa(val, ss, 16); //进制转换函数--转换一个无符号长整型数为任意进制的字符串,原数为十进制，value代表字符串中数的表现形式
		int i;
		int length = strlen(ss);
		if (length != 8)//六十四位编译器下，ul的字节长度为8
		{//低序的0出现在高位被忽略
			for (i = 0; i < 8 - length; i++)
				temp[i] = '0';
			for (i = 0; i < length; i++)
				temp[i + 8 - length] = ss[i];
			for (i = 0; i < 8; i++)
				ss[i] = temp[i];
		}

		//2. 进行大小端转换
		int t;
		t = ss[0];
		ss[0] = ss[6];
		ss[6] = t;

		t = ss[1];
		ss[1] = ss[7];
		ss[7] = t;

		t = ss[2];
		ss[2] = ss[4];
		ss[4] = t;

		t = ss[3];
		ss[3] = ss[5];
		ss[5] = t;

		//3. 将存有十六进制数(val)的字符串(ss)中的十六进制数转成十进制数
		int value = 0;
		for (i = 0; i < 8; i++)
		{
			int k;
			char mass;
			mass = ss[i];
			if (ss[i] == 'a' ||
				ss[i] == 'b' ||
				ss[i] == 'c' ||
				ss[i] == 'd' ||
				ss[i] == 'e' ||
				ss[i] == 'f')
				k = 10 + ss[i] - 'a';
			else
				sscanf(&mass, "%d", &k);
			value = value + int(k*pow(16, 7 - i));
		}
		return (value);
	}