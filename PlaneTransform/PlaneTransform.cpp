#include "PlaneTransform.h"


//1.��ȡShapeFileͷ�ļ�
	void ReadShp (FILE*   fp_ShpFile , int &ShapeType)
	{
		   //��ȡ�����ļ�ͷ�����ݿ�ʼ
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
		   //��ȡ�����ļ�ͷ�����ݽ���
	}

	//2.��ȡ���ļ���ÿ���ߵ���ɲ�����������Ϣ
	int ReadPointNum(FILE*   fp_ShpFile)
	{
		int shapeType;
		double Box[4];
		int NumParts;
		int NumPoints;
		int Parts;

		fread(&shapeType, sizeof(int), 1, fp_ShpFile);
		//��Box
		for (int i = 0; i < 4; i++)
			fread(Box + i, sizeof(double), 1, fp_ShpFile);

		//��NumParts��NumPoints
		fread(&NumParts, sizeof(int), 1, fp_ShpFile);
		fread(&NumPoints, sizeof(int), 1, fp_ShpFile);
		fread(&Parts, sizeof(int), 1, fp_ShpFile);
		return NumPoints;
	}

	//3.��ȡ�������е������
	void ReadSingleLine(FILE*   fp_ShpFile, Vector3d *pt,int num)
	{
			//��Points
			double x, y;
			for (int j = 0; j < num; j++)
			{
				fread(&x, sizeof(double), 1, fp_ShpFile);
				fread(&y, sizeof(double), 1, fp_ShpFile);
				pt[j] << x, y, 1;
			}
	}

	//4.�Ե��������ƽ��ת��
	void MatrixTransform(Vector3d *pt, int num)
	{
		Matrix3d TM1, TM2;
		TM1 << 2, 0, 0, 0, 2, 0, 0, 0, 1;
		TM2 << cos(M_PI / 3), sin(M_PI / 3), 0, -sin(M_PI / 3), cos(M_PI / 3), 0, 0, 0, 1;
		for (int i = 0; i < num; i++)
			pt[i] =TM2* TM1*pt[i];
		return;
	}

	//5.��ת��������꽨���µ���
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
	{//Big�������ֽڴ洢����ʼλ�ã�Little�������ֽڴ洢����ʼλ��
		char ss[9];
		char temp[8];
		unsigned long val = unsigned long(indata);

		//1. ��ʮ�����Ƶ���(val)ת��һ���ַ���(ss)��
		_ultoa(val, ss, 16); //����ת������--ת��һ���޷��ų�������Ϊ������Ƶ��ַ���,ԭ��Ϊʮ���ƣ�value�����ַ��������ı�����ʽ
		int i;
		int length = strlen(ss);
		if (length != 8)//��ʮ��λ�������£�ul���ֽڳ���Ϊ8
		{//�����0�����ڸ�λ������
			for (i = 0; i < 8 - length; i++)
				temp[i] = '0';
			for (i = 0; i < length; i++)
				temp[i + 8 - length] = ss[i];
			for (i = 0; i < 8; i++)
				ss[i] = temp[i];
		}

		//2. ���д�С��ת��
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

		//3. ������ʮ��������(val)���ַ���(ss)�е�ʮ��������ת��ʮ������
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