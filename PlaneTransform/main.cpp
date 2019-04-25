#include "PlaneTransform.h"
//#define ShpFileName "../Customize/AlbersChina.shp"
//#define ShpResultName "../Customize/TransformChina2.shp"
#define ShpFileName "../DefaultParameter/AlbersChina.shp"
#define ShpResultName "../DefaultParameter/TransformChina.shp"

int main()
{
	//1. 打开Shapfile文件
	FILE*   fp_ShpFile;
	if ((fp_ShpFile = fopen(ShpFileName, "rb")) == NULL)
	{
		cout << "Fail to open file!" << endl;
		exit(EXIT_FAILURE);
	}
	int ShpType=0;
	ReadShp(fp_ShpFile, ShpType);
	//***********************************************************************
	//***********************************************************************
	//2. 创建Shapfile文件
	//2.1 注册驱动
	OGRRegisterAll();
	const char *pszDriverName = "ESRI Shapefile";
	//在GDAL 2.0+的C/C++版本中移除了对于OGRDatasource及OGRSFDriver的支持。 
	//分别用GDALDataset、GDALDriver代替，类下的方法不变
	//OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);
	if (poDriver == NULL)
	{
		cout << pszDriverName << " driver not available" << endl;
		exit(1);
	}
	//2.2 创建文件
	GDALDataset *poDS;
	poDS = poDriver->Create(ShpResultName, 0, 0, 0, GDT_Unknown, NULL);
	if (poDS == NULL)
	{
		printf("Creation of output file failed.\n");
		exit(1);
	}
	//2.3 创建图层并建立投影
	OGRLayer *poLayer;
	OGRSpatialReference *prj = GetProjInfoFromFile(ShpFileName);
	poLayer = poDS->CreateLayer("chinaTransform", prj, wkbLineString, NULL);
	if (poLayer == NULL)
	{
		printf("Layer creation failed.\n");
		exit(1);
	}
	//***********************************************************************
	//***********************************************************************
	//3. 读取线状目标的实体信息，进行平面转换并进行存储
	int RecordNumber;
	int ContentLength;
	while ((fread(&RecordNumber, sizeof(int), 1, fp_ShpFile) != 0))	
	{  //循环至所有线全部读取转换完毕
		fread(&ContentLength, sizeof(int), 1, fp_ShpFile);
		//实体信息负责记录坐标信息，它以记录段为基本单位，
		//每一个记录段记录一个地理实体目标的坐标信息，每个记录段分为记录头和记录内容两部分。
		RecordNumber = EndianBig2Little(RecordNumber);
		ContentLength = EndianBig2Little(ContentLength);

		Vector3d *point=NULL;
		int num;
		num = ReadPointNum(fp_ShpFile);
		point = new Vector3d[num];

		ReadSingleLine(fp_ShpFile,point,num);		//读取点坐标
		MatrixTransform(point, num);				//转换点坐标
		WriteSingleLine(poLayer, point, num);		//坐标写入线
		
		delete[] point;
	}
	cout << "Plane Transformation finish!" << endl;
	GDALClose(poDS);

	getchar();
	return 0;
}
