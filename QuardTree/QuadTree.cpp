#include "QuadTree.h"

int main(void)
{
	//①声明文件流，矩阵的阶，记录矩阵的数组
	ifstream matrix("matrix.txt");
	int order =8;
	vector<myNode> myArr;
	myArr.reserve(order*order);

	//②读取矩阵文件，在屏幕中输出，并生成每个元素的M码
	ReadArray(myArr, matrix);
	matrix.close();
	cout << "The original matrix:" << endl;
	for (int i = 0; i < order; i++) {
		for (int j = 0; j < order; j++) {
			myArr[order*i + j].Morton(four_code(j,i));
			cout << myArr[order*i + j].value << "\t";
		}
		cout << "\n";
	}
	cout << endl;

	//③按照M码的大小对结点重新排序
	vector<myNode> myMatr;
	myMatr.reserve(order*order);
	four_mSequence(myMatr, myArr, order);

	//④自下而上合并区域的M码，在数组中存储为线性四叉树形式，并在屏幕上输出
	vector<LQTNode> LQT;
	four_LQT(LQT, myMatr, log2(order));
	cout << "LQT Node List:" << endl;
	for (int i = 0; i < LQT.size(); i++) {
		printf("Node %2d  ", i + 1);
		LQT[i].Print();
	}
	cout << endl << endl;
	four_LQTprint(LQT, order);
	getchar();
	return 0;
}

//1.读取文件函数
void ReadArray(vector<myNode> &array, ifstream &file)
{//从文件中读取矩阵并存入一个vector容器中
	string line;
	stringstream sstr;
	int n;
	myNode node;
	while (getline(file, line, ',')) {//以逗号为分隔符，读取字符串流file中的字符到line字符串
		sstr << line;
		sstr >> n;
		sstr.clear();
		node.Value(n);
		array.push_back(node);
	}
	return;
}

//2.四进制Morton码的生成与解码
long four_code(int col, int row)
{//由行列号生成对应的M码
	row = D2Binary(row);
	col = D2Binary(col);
	return ( 2 * row + col);
}
void four_decode(long Morton, int &col, int &row) 
{//由M码找到所属区域
	int figure = 0;
	for (col = 0, row = 0; Morton != 0; figure++, Morton /= 10) {
		col += pow(10, figure)*(Morton % 10 % 2);
		row += pow(10, figure)*(Morton % 10 / 2);
	}
	col = B2Decimal(col);
	row = B2Decimal(row);
}

//3.自下而上的线性四叉树的合并生成
void four_LQT(vector<LQTNode> &list, const vector<myNode> &matrix, const int layer)
{//生成压缩的线性四叉树
	int num = pow(pow(2, layer), 2);	//点的总数
	int gap;						//每层合并时相邻区域编码之间的位置差，可能的值为1，4，16，……
	vector<int> depth;				//每个区域所处的深度
	depth.reserve(num);
	for (int i = 0; i < num; i++) {
		depth.push_back(layer);				//默认深度最大
	}
	for (int n = 0; n < layer; n++) {
		gap = pow(4, n);
		for (int i = 0; i < num; i += gap * 4)
		{
			//cout<< "depth[i]= "<< depth[i]<<""
			if (depth[i] == depth[i + gap] && depth[i + gap * 2] && depth[i + gap * 3] == layer - n	//判断四个子区域是否内部均一，其中layer-n恒大于0
				&& matrix[i].value == matrix[i + gap].value
				&& matrix[i + gap].value == matrix[i + gap * 2].value
				&& matrix[i + gap * 2].value == matrix[i + gap * 3].value)
			{
				depth[i]--;
			}
		}
	}
	int col, row;
	for (int i = 0; i < num; i += gap * gap) {
		gap = pow(2, layer - depth[i]);
		four_decode(matrix[i].morton, col, row);
		col /= gap, row /= gap;
		list.push_back(LQTNode(four_code(col, row), depth[i], matrix[i].value));
	}
}

//4.屏幕输出函数
void four_LQTprint(vector<LQTNode> &list, int order)
{
	vector<myNode> matrix;
	int col, row, gap;
	int num = order * order;
	int layer = log2(order);
	for (int i = 0; i < num; i++) {
		matrix.push_back(myNode(0, 0));
	}
	for (int i = 0;i<list.size(); i++) {
		four_decode(list[i].Morton, col, row);
		gap = pow(2, layer - list[i].depth);
		col *= gap, row *= gap;
		for (int j = 0; j < gap; j++) {
			for (int k = 0; k < gap; k++) {
				matrix[(row + j)*order + (col + k)].Value(list[i].value);
				matrix[(row + j)*order + (col + k)].Morton(list[i].Morton);
			}
		}
	}
	cout << "Morton Code Result:" << endl;
	for (int i = 0; i < order; i++) {
		for (int j = 0; j < order; j++) {
			cout << matrix[i*order + j].morton << "\t";
		}
		cout << "\n";
	}
	cout << endl;
	return;
}

//5.排序函数
void four_mSequence(vector<myNode> &matr, const vector<myNode> &array, const int order)
{//通过行列位置，以M码顺序排列后的顺序为主序，最坏的时间复杂度为num*3*2;
	int layer = log2(order);
	int num = order * order;
	int n = 0, col = 0, row = 0, gap = 0, temp = 0;
	for (int pos = 0; pos < num; pos++) {
		col = row = 0, temp = pos;
		for (n = 1; n <= layer; n++) {
			gap = (int)pow(2, layer - n);
			switch (temp / (gap*gap))
			{
			case 1:col += gap;  break;
			case 2:row += gap; break;
			case 3:row += gap, col += gap;  break;
			}
			temp %= gap * gap;
		}
		matr.push_back(array[row*order + col]);
	}
}
void four_vSequence(vector<myNode> &array, const vector<myNode> &matr, const int order)
{//将按照M码大小进行排序的数组还原到原行列位置。
	int layer = log2(order);
	int num = order * order;
	int n = 0, col = 0, row = 0, gap = 0, temp = 0;
	for (int i = 0; i < num; i++) {
		col = row = 0, temp = i;
		for (n = 1; n <= layer; n++) {
			gap = (int)pow(2, layer - n);
			switch (temp / (gap*gap))
			{
			case 1:col += gap;  break;
			case 2:row += gap; break;
			case 3:row += gap, col += gap;  break;
			}
			temp %= gap * gap;
		}
		array[row*order + col] = matr[i];
	}
}
//进制转换函数（辅助函数）
int D2Binary(int d)
{//将十进制数用二进制的形式表示（仍按十进制存储）
	int digit, byte;
	long b = 0;
	if (d == 0)
		return b;
	for (digit = 0; (d / 2) != 0; digit++) {
		byte = d % 2;
		d /= 2;
		b += byte * pow(10, digit);
	}
	b += pow(10, digit);
	return b;
}

int B2Decimal(int b)
{
	int d = 0, p = 0;
	for (; b / 10 != 0;) {
		d += pow(2, p++)*(b % 10);
		b /= 10;
	}
	d += pow(2, p)*(b % 10);
	return d;
}

//void PrintMatrixMorton(const vector<myNode> &matrix, const int &order)
//{
//	for (int i = 0; i < order; i++) {
//		for (int j = 0; j < order; j++) {
//			cout << matrix[order*i + j].morton << "\t";
//		}
//		cout << "\n";
//	}
//	cout << endl;
//}