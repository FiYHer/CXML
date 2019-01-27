#include "XML.h"


//读取xml文件到内存中来
void CXML_space::CXML::ReadXmlFileToBuf()
{
	//获取一下xml文件的大小
	m_XmlFile.seekg(0, m_XmlFile.end);
	long srcSize = (long)m_XmlFile.tellg();

	//将读写指针都清零
	m_XmlFile.seekg(0, ios::beg);
	m_XmlFile.seekp(0, ios::beg);

	if (srcSize == NULL)
		srcSize = 1000;

	//将文件的数据全部读进内存
	char *pBuf = new char[srcSize];
	memset(pBuf,0,srcSize);
	m_XmlFile.read(pBuf, srcSize);

	//使用string类来处理它
	m_XmlBuf = pBuf;
	delete[] pBuf;
}

//把内存中的修改保存到xml文件
void CXML_space::CXML::WriteBufToXmlFile()
{
	assert(m_XmlFile.is_open());

	//如果是写模式的话
	if (m_nMode == XMLWRITE)
	{
		//把原来的文件清空
		m_XmlFile.clear();

		//缓冲区是不是空
		if (!m_XmlBuf.empty())
			m_XmlFile << m_XmlBuf.c_str() << endl;
	}

	if (m_XmlFile.is_open())
		m_XmlFile.close();

	if (!m_XmlBuf.empty())
		m_XmlBuf.clear();
}

//切换文件模式
bool CXML_space::CXML::SwitchFileMode()
{
	assert(!m_XmlFilePath.empty());

	//由写模式切换到读模式的话，就要先保存一下数据啊
	//读模式变成写模式的时候，缓冲区不能清空！！！！！

	//切换模式
	if (m_nMode == XMLWRITE)
	{
		WriteBufToXmlFile();
		m_XmlFile.open(m_XmlFilePath.c_str(), XMLREAD);
		if (m_XmlFile.is_open())
			ReadXmlFileToBuf();
		m_nMode = XMLREAD;
	}
	else
	{
		//由读模式或者其它模式切换到写模式的话
		//缓冲区不能清空
		m_XmlFile.close();
		m_XmlFile.open(m_XmlFilePath.c_str(), XMLWRITE);
		m_nMode = XMLWRITE;
	}

	if (m_XmlFile.is_open())
		return true;
	else
	    return false;
}


CXML_space::CXML::CXML()
{
	m_lReadRootBegin = NULL;
	m_nMode = NULL;
}

CXML_space::CXML::~CXML()
{
	if (m_XmlFile.is_open())
		m_XmlFile.close();

	if (!m_XmlBuf.empty())
		m_XmlBuf.clear();
}

//加载xml文件
bool CXML_space::CXML::LoadXmlFile(const string xmlFilePath /*= NULL*/,
	ios_base::openmode xmlOpenMode /*= DefaultOpenMode*/)
{
	//文件已经打开
	if (m_XmlFile.is_open())
	{
		m_XmlFile.close();
	}

	if (!m_XmlBuf.empty())
	{
		m_XmlBuf.clear();
	}

	if (xmlFilePath.empty())
		return false;

	m_XmlFilePath = xmlFilePath;

	//读模式不会创建文件
	m_XmlFile.open(xmlFilePath.c_str(),xmlOpenMode);
	if (m_XmlFile.is_open())
	{
		//读取模式//或许不是默认参数
		ReadXmlFileToBuf();
		m_nMode = xmlOpenMode;
		return true;
	}
	//写模式打开、、会创建文件
	m_XmlFile.open(xmlFilePath.c_str(), XMLWRITE);
	if (m_XmlFile.is_open())
	{
		//写入模式
		m_nMode = XMLWRITE;
		return true;
	}
	return false;
}

//保存xml文件
void CXML_space::CXML::CloseXmlFile()
{
	WriteBufToXmlFile();
}

//读取根节点
//#strRootName:根节点的名字
bool CXML_space::CXML::ReadRootNodeName(string& strRootName)
{
	assert(m_XmlFile.is_open());

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return false;

	///根节点永远都是在最后面的，所以我们后面入手
	int nLastRootBegin = m_XmlBuf.rfind("<");
	int nLastRootEnd = m_XmlBuf.rfind(">");
	if (nLastRootBegin == string::npos || nLastRootEnd == string::npos)
		return false;

	//这个就是最后的根节点名字
	strRootName = m_XmlBuf.substr(nLastRootBegin + 2, nLastRootEnd - nLastRootBegin - 2);

	//查找开始第一个根节点名字
	int nRootBengin = m_XmlBuf.find(strRootName);
	if (nRootBengin == nLastRootBegin || nRootBengin == string::npos)
		return false;

	//前后一致就是真正的根节点了
	string strRoot = m_XmlBuf.substr(nRootBengin, strRootName.length());
	if (strRoot == strRootName)
	{
		m_lReadRootBegin = nRootBengin;
		return true;
	}

	return false;
}

//读取根节点属性的值
/*
#strAttribute:根节点的属性
#strValue:根节点的值
*/
bool CXML_space::CXML::ReadRootNodeValue(string strAttribute,string& strValue)
{
	assert(!strAttribute.empty());

	assert(!strValue.empty());

	assert(!m_XmlBuf.empty());

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return false;

	//找到根节点的起始位置
	string strRootName;
	if (!ReadRootNodeName(strRootName))
		return false;

	//根节点一行的结束位置
	int nRootLineEnd = m_XmlBuf.find(">", m_lReadRootBegin);
	if (nRootLineEnd == string::npos)
		return false;

	//找到要查询的属性的开始位置
	int nAttributeBegin = m_XmlBuf.find(strAttribute, m_lReadRootBegin);
	if (nAttributeBegin > nRootLineEnd || nAttributeBegin == string::npos)
		return false;

	//查找属性的值得结束位置
	int nValueEnd = m_XmlBuf.find("\"", nAttributeBegin + strAttribute.length() + 2);
	if (nValueEnd == string::npos || nValueEnd > nRootLineEnd)
		return false;

	//把值复制出来
	int nPos = nAttributeBegin + strAttribute.length() + 2;
	strValue = m_XmlBuf.substr(nPos, nValueEnd - nPos);
	return true;
}


//读取孩子节点
/*
#strChildName:孩子节点的名字
#strChildAttribute：孩子节点的属性
#strValue：孩子节点的值
#lBegin：开始读取的位置
*/
long CXML_space::CXML::ReadChildNodeValue(string strChildName, string strChildAttribute, string& strValue, long lBegin/*=0*/)
{
	assert(m_XmlFile.is_open());

	if (strChildName.empty())
		return 0;

	if (strChildAttribute.empty())
		return 0;

	if (lBegin == -1)
		return -1;

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return 0;

	strValue = "Can't Find";

	//从指定位置开始查找孩子节点名字
	int nPos = m_XmlBuf.find(strChildName,lBegin);
	while (nPos != string::npos)
	{
		//如果找到的位置的前面没有<的话，那它就不是一个孩子节点，
		//很可能只是一个属性或者值
		//所以我们要继续查找，直到文件结尾
		if (m_XmlBuf[nPos-1]=='<')
			break;
		nPos = m_XmlBuf.find(strChildName, nPos+1);
	}

	//孩子节点一行的结束
	int nChilidEnd = m_XmlBuf.find("<", nPos);
	if (nChilidEnd == string::npos)
		return -1;

	//查找属性，位置不能超过孩子节点一行的长度
	int nAttribute = m_XmlBuf.find(strChildAttribute,nPos);
	if (nAttribute == string::npos || nAttribute >= nChilidEnd)
		return -1;

	//值开始的地方=属性开始的地方+属性的长度+2【一个等号加一个双引号就是2】
	int nSubBegin = nAttribute + strChildAttribute.length() + 2;

	//值结束的地址后面都有一个引号
	int nValueEnd = m_XmlBuf.find("\"", nSubBegin);
	if (nValueEnd == string::npos)
		return -1;

	//把值给复制出来
	strValue = m_XmlBuf.substr(nSubBegin, nValueEnd - nSubBegin);

	//返回当前值结束的地方加1，方便后面函数继续调用查找下一个
	return nValueEnd+1;
}

//读取全部孩子节点的名字
//#strChildNodeName:接收孩子节点的名字
//#lBegin:开始搜索的开始位置
long CXML_space::CXML::ReadChildNodeAllName(string& strChildNodeName, long lBegin)
{
	assert(m_XmlFile.is_open());

	if (lBegin == -1)
		return -1;

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return 0;

	strChildNodeName = "Can't Find";

	int nPos = m_XmlBuf.find("<", lBegin);
	while (nPos != string::npos)
	{
		if (m_XmlBuf[nPos+1]!='/')
		{
			//找到这一行的结束位置
			int nChildLineEnd = m_XmlBuf.find(">",nPos);
			if (nChildLineEnd == string::npos)
				return -1;

			//找到孩子节点名字的结束位置
			int nChildNodeNameEnd = m_XmlBuf.find(" ",nPos);
			if (nChildNodeNameEnd == string::npos || nChildNodeNameEnd>=nChildLineEnd)
				return -1;

			strChildNodeName = m_XmlBuf.substr(nPos+1,nChildNodeNameEnd-nPos-1);
			return nPos + 1;
		}
		nPos = m_XmlBuf.find("<", nPos + 1);
	}
	return -1;
}

//读取孩子节点的全部属性
//#strChildNodeName:孩子节点的名字
//#strChildAttributeList:孩子节点属性列表
//#lBegin:开始搜索的开始位置
bool CXML_space::CXML::ReadChildNodeAllAttribute(string strChildNodeName, string& strChildAttributeList, long lBegin/*=0*/)
{
	assert(m_XmlFile.is_open());

	if (strChildNodeName.empty())
		return false;

	if (lBegin == -1)
		return false;

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return false;

	strChildAttributeList = "Can't Find";

	//先找到孩子节点，要区分是节点还是属性或者是值
	int nChildNodeBegin = m_XmlBuf.find(strChildNodeName,lBegin);
	while (nChildNodeBegin != string::npos)
	{
		if (m_XmlBuf[nChildNodeBegin-1]=='<')
		{
			//现在开始找这一行的结束位置
			int nChildLineEnd = m_XmlBuf.find(">",nChildNodeBegin);
			if (nChildLineEnd == string::npos)
				return false;

			//字符串复制出来
			strChildAttributeList = m_XmlBuf.substr(nChildNodeBegin,nChildLineEnd-nChildNodeBegin);
			return true;
		}
		//继续查找
		nChildNodeBegin = m_XmlBuf.find(strChildNodeName,nChildNodeBegin+1);
	}
	return false;
}

//写入xml文件版本信息
/*
#strVersion:版本信息
#strEncoding:编码信息
*/
void CXML_space::CXML::WriteEdtion(string strVersion, string strEncoding)
{
	assert(m_XmlFile.is_open());

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//版本和编码两个字符查看一下有没有,在不在指定位置上
	int nVersionPos = m_XmlBuf.find("version");
	int nEncodingPos = m_XmlBuf.find("encoding");
	if (nVersionPos == string::npos || nEncodingPos==string::npos || nVersionPos>38 || nEncodingPos>38)
	{
		//格式化再进行写入
		char szFormat[1024];
		sprintf(szFormat, "<?xml version=\"%s\" encoding=\"%s\"?>\n", strVersion.c_str(), strEncoding.c_str());
		//插入到文件的开始的地方
		m_XmlBuf.insert(0, szFormat);
	}
}


//写入xml文件名字
//#strRootName:根节点的名字
void CXML_space::CXML::WriteRootNodeName(string strRootName)
{
	assert(m_XmlFile.is_open());

	if (strRootName.empty())
		return;

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//写入一下版本信息
	WriteEdtion();

	//版本信息的结尾就是根节点的开头
 	int nRootBegin = m_XmlBuf.find(">");
	if (nRootBegin == string::npos)
		return;

	char szRoot[1024] = {0};
	sprintf(szRoot,"\n<%s>",strRootName.c_str());
	m_XmlBuf.insert(nRootBegin + 1, szRoot);

	//从后面查找第一个>就是文件的结尾
	//我们把根节点结束添加到结尾就OK
	int nEnd = m_XmlBuf.rfind(">");
	if (nEnd == string::npos)
		return;

	char szFormat[1024];
	sprintf(szFormat, "\n</%s>", strRootName.c_str());
	m_XmlBuf.insert(nEnd + 1, szFormat);
}


//写入根的属性
/*
#strRootName:根节点的名字
#strRootAttribute:根节点的属性
#strRootValue:根节点属性的值
*/
void CXML_space::CXML::WriteRootNodeAttribute(string strRootName,string strRootAttribute, string strRootValue)
{
	assert(m_XmlFile.is_open());

	if (strRootName.empty() || strRootAttribute.empty() || strRootValue.empty())
		return;

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//先找到根节点名字
	int nRootNameBegin = m_XmlBuf.find(strRootName);
	if (nRootNameBegin == string::npos)
		return;

	char szFormat[1024] = {0};
	sprintf(szFormat," %s=\"%s\"",strRootAttribute.c_str(),strRootValue.c_str());
	m_XmlBuf.insert(nRootNameBegin + strRootName.length(), szFormat);
	return;
}

//写入孩子节点,指定上一个节点名字
/*
#strChildNodeName:孩子节点的名字
#strLastNodeName:上一个节点的名字
*/
void CXML_space::CXML::WriteChildNodeName(string strChildNodeName,string strLastNodeName,long lBegin)
{
	assert(m_XmlFile.is_open());

	if (strChildNodeName.empty())
		return;

	if (lBegin == -1)
		return;

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//先查找上一个节点的结束
	char szFormat[1024] = { 0 };
	sprintf(szFormat,"</%s>",strLastNodeName.c_str());
	int nLastNodeEnd = m_XmlBuf.find(szFormat,lBegin);
	if (nLastNodeEnd == string::npos)
	{
		//我们这是第一个依附于这个节点的节点啊
		//先找到上一个节点的开始位置,找不到就返回
		int nLastNodeBegin = m_XmlBuf.find(strLastNodeName.c_str());
		if (nLastNodeBegin == string::npos)
			return;

		//我们找到上一个节点的那一行的结束位置
		int nLastNodeLineEnd = m_XmlBuf.find(">",nLastNodeBegin);
		if (nLastNodeLineEnd == string::npos)
			return;

		//插入我们的节点，
		sprintf(szFormat,"\n\t<%s />",strChildNodeName.c_str());
		m_XmlBuf.insert(nLastNodeLineEnd + 1, szFormat);

		//插入上一个节点的结束
		int nNodeLen = strlen(szFormat);
		sprintf(szFormat,"\n\t</%s>",strLastNodeName.c_str());
		m_XmlBuf.insert(nNodeLen+nLastNodeLineEnd+1,szFormat);

		//我们先把这一行后面的/删除掉
		if (m_XmlBuf[nLastNodeLineEnd - 1] == '/')
			m_XmlBuf.erase(nLastNodeLineEnd - 1, 1);
	}
	else
	{
		//我们不是第一个依附于这个节点的节点啊

		//我们孩子节点开始的地方
		int nChildNodeBegin = nLastNodeEnd-1;

		char szFormat[1024] = {0};
		sprintf(szFormat,"\n\t<%s />",strChildNodeName.c_str());
		m_XmlBuf.insert(nChildNodeBegin, szFormat);
	}
}

//写入孩子节点属性
/*
#strChildNodeName:孩子节点名字
#strChildNodeAttribute:孩子节点属性
#strChildValue:孩子节点属性的值
#lBegin：开始搜索的起始位置
*/
void CXML_space::CXML::WriteChildNodeAttribute(string strChildNodeName, string strChildNodeAttribute, 
	string strChildValue, long lBegin/*=0*/)
{
	assert(m_XmlFile.is_open());

	if (strChildNodeName.empty() || strChildNodeAttribute.empty() || strChildValue.empty())
		return;

	if (lBegin == -1)
		return;

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//查找孩子节点名字
	int nChildNodeName = m_XmlBuf.find(strChildNodeName.c_str(),lBegin);
	if (nChildNodeName == string::npos)
		return;

	//查找一行的结束
	int nChildNodeLineEnd = m_XmlBuf.find(">", nChildNodeName);
	if (nChildNodeLineEnd == string::npos)
		return;

	//写入位置
	int nPos = nChildNodeLineEnd;
	if (m_XmlBuf[nPos-1] == '/')
		nPos--;

	char szFormat[1024] = {0};
	sprintf(szFormat," %s=\"%s\"",strChildNodeAttribute.c_str(),strChildValue.c_str());
	m_XmlBuf.insert(nPos, szFormat);

}

