/*

xml文件解析/编辑类
纯C++代码实现
为了实现软件界面库配置读取而开发的

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