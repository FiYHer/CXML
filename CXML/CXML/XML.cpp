#include "XML.h"


//��ȡxml�ļ����ڴ�����
void CXML_space::CXML::ReadXmlFileToBuf()
{
	//��ȡһ��xml�ļ��Ĵ�С
	m_XmlFile.seekg(0, m_XmlFile.end);
	long srcSize = (long)m_XmlFile.tellg();

	//����дָ�붼����
	m_XmlFile.seekg(0, ios::beg);
	m_XmlFile.seekp(0, ios::beg);

	if (srcSize == NULL)
		srcSize = 1000;

	//���ļ�������ȫ�������ڴ�
	char *pBuf = new char[srcSize];
	memset(pBuf,0,srcSize);
	m_XmlFile.read(pBuf, srcSize);

	//ʹ��string����������
	m_XmlBuf = pBuf;
	delete[] pBuf;
}

//���ڴ��е��޸ı��浽xml�ļ�
void CXML_space::CXML::WriteBufToXmlFile()
{
	assert(m_XmlFile.is_open());

	//�����дģʽ�Ļ�
	if (m_nMode == XMLWRITE)
	{
		//��ԭ�����ļ����
		m_XmlFile.clear();

		//�������ǲ��ǿ�
		if (!m_XmlBuf.empty())
			m_XmlFile << m_XmlBuf.c_str() << endl;
	}

	if (m_XmlFile.is_open())
		m_XmlFile.close();

	if (!m_XmlBuf.empty())
		m_XmlBuf.clear();
}

//�л��ļ�ģʽ
bool CXML_space::CXML::SwitchFileMode()
{
	assert(!m_XmlFilePath.empty());

	//��дģʽ�л�����ģʽ�Ļ�����Ҫ�ȱ���һ�����ݰ�
	//��ģʽ���дģʽ��ʱ�򣬻�����������գ���������

	//�л�ģʽ
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
		//�ɶ�ģʽ��������ģʽ�л���дģʽ�Ļ�
		//�������������
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

//����xml�ļ�
bool CXML_space::CXML::LoadXmlFile(const string xmlFilePath /*= NULL*/,
	ios_base::openmode xmlOpenMode /*= DefaultOpenMode*/)
{
	//�ļ��Ѿ���
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

	//��ģʽ���ᴴ���ļ�
	m_XmlFile.open(xmlFilePath.c_str(),xmlOpenMode);
	if (m_XmlFile.is_open())
	{
		//��ȡģʽ//������Ĭ�ϲ���
		ReadXmlFileToBuf();
		m_nMode = xmlOpenMode;
		return true;
	}
	//дģʽ�򿪡����ᴴ���ļ�
	m_XmlFile.open(xmlFilePath.c_str(), XMLWRITE);
	if (m_XmlFile.is_open())
	{
		//д��ģʽ
		m_nMode = XMLWRITE;
		return true;
	}
	return false;
}

//����xml�ļ�
void CXML_space::CXML::CloseXmlFile()
{
	WriteBufToXmlFile();
}

//��ȡ���ڵ�
//#strRootName:���ڵ������
bool CXML_space::CXML::ReadRootNodeName(string& strRootName)
{
	assert(m_XmlFile.is_open());

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return false;

	///���ڵ���Զ�����������ģ��������Ǻ�������
	int nLastRootBegin = m_XmlBuf.rfind("<");
	int nLastRootEnd = m_XmlBuf.rfind(">");
	if (nLastRootBegin == string::npos || nLastRootEnd == string::npos)
		return false;

	//����������ĸ��ڵ�����
	strRootName = m_XmlBuf.substr(nLastRootBegin + 2, nLastRootEnd - nLastRootBegin - 2);

	//���ҿ�ʼ��һ�����ڵ�����
	int nRootBengin = m_XmlBuf.find(strRootName);
	if (nRootBengin == nLastRootBegin || nRootBengin == string::npos)
		return false;

	//ǰ��һ�¾��������ĸ��ڵ���
	string strRoot = m_XmlBuf.substr(nRootBengin, strRootName.length());
	if (strRoot == strRootName)
	{
		m_lReadRootBegin = nRootBengin;
		return true;
	}

	return false;
}

//��ȡ���ڵ����Ե�ֵ
/*
#strAttribute:���ڵ������
#strValue:���ڵ��ֵ
*/
bool CXML_space::CXML::ReadRootNodeValue(string strAttribute,string& strValue)
{
	assert(!strAttribute.empty());

	assert(!strValue.empty());

	assert(!m_XmlBuf.empty());

	if (m_nMode == XMLWRITE)
		if (!SwitchFileMode())
			return false;

	//�ҵ����ڵ����ʼλ��
	string strRootName;
	if (!ReadRootNodeName(strRootName))
		return false;

	//���ڵ�һ�еĽ���λ��
	int nRootLineEnd = m_XmlBuf.find(">", m_lReadRootBegin);
	if (nRootLineEnd == string::npos)
		return false;

	//�ҵ�Ҫ��ѯ�����ԵĿ�ʼλ��
	int nAttributeBegin = m_XmlBuf.find(strAttribute, m_lReadRootBegin);
	if (nAttributeBegin > nRootLineEnd || nAttributeBegin == string::npos)
		return false;

	//�������Ե�ֵ�ý���λ��
	int nValueEnd = m_XmlBuf.find("\"", nAttributeBegin + strAttribute.length() + 2);
	if (nValueEnd == string::npos || nValueEnd > nRootLineEnd)
		return false;

	//��ֵ���Ƴ���
	int nPos = nAttributeBegin + strAttribute.length() + 2;
	strValue = m_XmlBuf.substr(nPos, nValueEnd - nPos);
	return true;
}


//��ȡ���ӽڵ�
/*
#strChildName:���ӽڵ������
#strChildAttribute�����ӽڵ������
#strValue�����ӽڵ��ֵ
#lBegin����ʼ��ȡ��λ��
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

	//��ָ��λ�ÿ�ʼ���Һ��ӽڵ�����
	int nPos = m_XmlBuf.find(strChildName,lBegin);
	while (nPos != string::npos)
	{
		//����ҵ���λ�õ�ǰ��û��<�Ļ��������Ͳ���һ�����ӽڵ㣬
		//�ܿ���ֻ��һ�����Ի���ֵ
		//��������Ҫ�������ң�ֱ���ļ���β
		if (m_XmlBuf[nPos-1]=='<')
			break;
		nPos = m_XmlBuf.find(strChildName, nPos+1);
	}

	//���ӽڵ�һ�еĽ���
	int nChilidEnd = m_XmlBuf.find("<", nPos);
	if (nChilidEnd == string::npos)
		return -1;

	//�������ԣ�λ�ò��ܳ������ӽڵ�һ�еĳ���
	int nAttribute = m_XmlBuf.find(strChildAttribute,nPos);
	if (nAttribute == string::npos || nAttribute >= nChilidEnd)
		return -1;

	//ֵ��ʼ�ĵط�=���Կ�ʼ�ĵط�+���Եĳ���+2��һ���Ⱥż�һ��˫���ž���2��
	int nSubBegin = nAttribute + strChildAttribute.length() + 2;

	//ֵ�����ĵ�ַ���涼��һ������
	int nValueEnd = m_XmlBuf.find("\"", nSubBegin);
	if (nValueEnd == string::npos)
		return -1;

	//��ֵ�����Ƴ���
	strValue = m_XmlBuf.substr(nSubBegin, nValueEnd - nSubBegin);

	//���ص�ǰֵ�����ĵط���1��������溯���������ò�����һ��
	return nValueEnd+1;
}

//��ȡȫ�����ӽڵ������
//#strChildNodeName:���պ��ӽڵ������
//#lBegin:��ʼ�����Ŀ�ʼλ��
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
			//�ҵ���һ�еĽ���λ��
			int nChildLineEnd = m_XmlBuf.find(">",nPos);
			if (nChildLineEnd == string::npos)
				return -1;

			//�ҵ����ӽڵ����ֵĽ���λ��
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

//��ȡ���ӽڵ��ȫ������
//#strChildNodeName:���ӽڵ������
//#strChildAttributeList:���ӽڵ������б�
//#lBegin:��ʼ�����Ŀ�ʼλ��
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

	//���ҵ����ӽڵ㣬Ҫ�����ǽڵ㻹�����Ի�����ֵ
	int nChildNodeBegin = m_XmlBuf.find(strChildNodeName,lBegin);
	while (nChildNodeBegin != string::npos)
	{
		if (m_XmlBuf[nChildNodeBegin-1]=='<')
		{
			//���ڿ�ʼ����һ�еĽ���λ��
			int nChildLineEnd = m_XmlBuf.find(">",nChildNodeBegin);
			if (nChildLineEnd == string::npos)
				return false;

			//�ַ������Ƴ���
			strChildAttributeList = m_XmlBuf.substr(nChildNodeBegin,nChildLineEnd-nChildNodeBegin);
			return true;
		}
		//��������
		nChildNodeBegin = m_XmlBuf.find(strChildNodeName,nChildNodeBegin+1);
	}
	return false;
}

//д��xml�ļ��汾��Ϣ
/*
#strVersion:�汾��Ϣ
#strEncoding:������Ϣ
*/
void CXML_space::CXML::WriteEdtion(string strVersion, string strEncoding)
{
	assert(m_XmlFile.is_open());

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//�汾�ͱ��������ַ��鿴һ����û��,�ڲ���ָ��λ����
	int nVersionPos = m_XmlBuf.find("version");
	int nEncodingPos = m_XmlBuf.find("encoding");
	if (nVersionPos == string::npos || nEncodingPos==string::npos || nVersionPos>38 || nEncodingPos>38)
	{
		//��ʽ���ٽ���д��
		char szFormat[1024];
		sprintf(szFormat, "<?xml version=\"%s\" encoding=\"%s\"?>\n", strVersion.c_str(), strEncoding.c_str());
		//���뵽�ļ��Ŀ�ʼ�ĵط�
		m_XmlBuf.insert(0, szFormat);
	}
}


//д��xml�ļ�����
//#strRootName:���ڵ������
void CXML_space::CXML::WriteRootNodeName(string strRootName)
{
	assert(m_XmlFile.is_open());

	if (strRootName.empty())
		return;

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//д��һ�°汾��Ϣ
	WriteEdtion();

	//�汾��Ϣ�Ľ�β���Ǹ��ڵ�Ŀ�ͷ
 	int nRootBegin = m_XmlBuf.find(">");
	if (nRootBegin == string::npos)
		return;

	char szRoot[1024] = {0};
	sprintf(szRoot,"\n<%s>",strRootName.c_str());
	m_XmlBuf.insert(nRootBegin + 1, szRoot);

	//�Ӻ�����ҵ�һ��>�����ļ��Ľ�β
	//���ǰѸ��ڵ������ӵ���β��OK
	int nEnd = m_XmlBuf.rfind(">");
	if (nEnd == string::npos)
		return;

	char szFormat[1024];
	sprintf(szFormat, "\n</%s>", strRootName.c_str());
	m_XmlBuf.insert(nEnd + 1, szFormat);
}


//д���������
/*
#strRootName:���ڵ������
#strRootAttribute:���ڵ������
#strRootValue:���ڵ����Ե�ֵ
*/
void CXML_space::CXML::WriteRootNodeAttribute(string strRootName,string strRootAttribute, string strRootValue)
{
	assert(m_XmlFile.is_open());

	if (strRootName.empty() || strRootAttribute.empty() || strRootValue.empty())
		return;

	if (m_nMode == XMLREAD)
		if (!SwitchFileMode())
			return;

	//���ҵ����ڵ�����
	int nRootNameBegin = m_XmlBuf.find(strRootName);
	if (nRootNameBegin == string::npos)
		return;

	char szFormat[1024] = {0};
	sprintf(szFormat," %s=\"%s\"",strRootAttribute.c_str(),strRootValue.c_str());
	m_XmlBuf.insert(nRootNameBegin + strRootName.length(), szFormat);
	return;
}

//д�뺢�ӽڵ�,ָ����һ���ڵ�����
/*
#strChildNodeName:���ӽڵ������
#strLastNodeName:��һ���ڵ������
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

	//�Ȳ�����һ���ڵ�Ľ���
	char szFormat[1024] = { 0 };
	sprintf(szFormat,"</%s>",strLastNodeName.c_str());
	int nLastNodeEnd = m_XmlBuf.find(szFormat,lBegin);
	if (nLastNodeEnd == string::npos)
	{
		//�������ǵ�һ������������ڵ�Ľڵ㰡
		//���ҵ���һ���ڵ�Ŀ�ʼλ��,�Ҳ����ͷ���
		int nLastNodeBegin = m_XmlBuf.find(strLastNodeName.c_str());
		if (nLastNodeBegin == string::npos)
			return;

		//�����ҵ���һ���ڵ����һ�еĽ���λ��
		int nLastNodeLineEnd = m_XmlBuf.find(">",nLastNodeBegin);
		if (nLastNodeLineEnd == string::npos)
			return;

		//�������ǵĽڵ㣬
		sprintf(szFormat,"\n\t<%s />",strChildNodeName.c_str());
		m_XmlBuf.insert(nLastNodeLineEnd + 1, szFormat);

		//������һ���ڵ�Ľ���
		int nNodeLen = strlen(szFormat);
		sprintf(szFormat,"\n\t</%s>",strLastNodeName.c_str());
		m_XmlBuf.insert(nNodeLen+nLastNodeLineEnd+1,szFormat);

		//�����Ȱ���һ�к����/ɾ����
		if (m_XmlBuf[nLastNodeLineEnd - 1] == '/')
			m_XmlBuf.erase(nLastNodeLineEnd - 1, 1);
	}
	else
	{
		//���ǲ��ǵ�һ������������ڵ�Ľڵ㰡

		//���Ǻ��ӽڵ㿪ʼ�ĵط�
		int nChildNodeBegin = nLastNodeEnd-1;

		char szFormat[1024] = {0};
		sprintf(szFormat,"\n\t<%s />",strChildNodeName.c_str());
		m_XmlBuf.insert(nChildNodeBegin, szFormat);
	}
}

//д�뺢�ӽڵ�����
/*
#strChildNodeName:���ӽڵ�����
#strChildNodeAttribute:���ӽڵ�����
#strChildValue:���ӽڵ����Ե�ֵ
#lBegin����ʼ��������ʼλ��
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

	//���Һ��ӽڵ�����
	int nChildNodeName = m_XmlBuf.find(strChildNodeName.c_str(),lBegin);
	if (nChildNodeName == string::npos)
		return;

	//����һ�еĽ���
	int nChildNodeLineEnd = m_XmlBuf.find(">", nChildNodeName);
	if (nChildNodeLineEnd == string::npos)
		return;

	//д��λ��
	int nPos = nChildNodeLineEnd;
	if (m_XmlBuf[nPos-1] == '/')
		nPos--;

	char szFormat[1024] = {0};
	sprintf(szFormat," %s=\"%s\"",strChildNodeAttribute.c_str(),strChildValue.c_str());
	m_XmlBuf.insert(nPos, szFormat);

}

