/*

xml�ļ�����/�༭��
��C++����ʵ��
Ϊ��ʵ�������������ö�ȡ��������

*/
#include "XML.h"
using namespace CXML_space;

int main(char**,char**,int)
{

	CXML_space::CXML MyXml;
	long bRet = 0;

	string xmlFile = "C:\\Users\\Administrator\\Desktop\\test.xml";
	bRet = MyXml.LoadXmlFile(xmlFile);
	MyXml.CloseXmlFile();
	system("pause");
	return 0;
}