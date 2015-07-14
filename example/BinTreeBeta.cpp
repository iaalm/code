/*****************************************
�ļ�����BinTreeBeta.cpp
���ܣ���������BinTree�Ĳ���ʾ��
�汾��1.0
���ߣ�Simon
����ʱ�䣺2012/06/06 10:25:42 
����ʱ�䣺
��ע��
******************************************/

#include<iostream>
#include"BinTree.h"

using namespace std;

class intBinTree : public BinTree		//�򵥵ļ̳г�һ��ֻ��һ�����ͱ����Ķ��������������
{
	private:
		int num;
	public:
		intBinTree(int n = 0,intBinTree* up = NULL,intBinTree* left = NULL,intBinTree* right = NULL):BinTree(up,left,right)
	{
		num = n;
	}
		ostream& print(ostream &o);
};

ostream& intBinTree::print(ostream &o)
{
	return o << num;
}

int main()
{
	intBinTree* leaf = new intBinTree(21);
	intBinTree* lleaf = new intBinTree(2,NULL,leaf),*rleaf = new intBinTree(3);
	intBinTree* root = new intBinTree(1,NULL,lleaf,rleaf);
	root -> displayTree(cout);
	return 0;
}
