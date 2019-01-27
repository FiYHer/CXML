#pragma once

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

namespace CXML_space
{
#define XMLREAD  (ios::in)           //����
#define XMLWRITE (ios::out | ios::trunc)//д��


	class CXML
	{
	private:
		//XML�ļ�
		fstream m_XmlFile;
		//xml�ļ�������
		string m_XmlBuf;
		//xml�ļ�·��
		string m_XmlFilePath;
		//��ģʽ
		int m_nMode;
		//д����ڵ�Ŀ�ʼλ��
		long m_lReadRootBegin;
	private:
		//��ȡxml�ļ����ڴ�����
		void ReadXmlFileToBuf();
		//���ڴ��е��޸ı��浽xml�ļ�
		void WriteBufToXmlFile();
		//�л��ļ�ģʽ
		bool SwitchFileMode();
	public:
		//���һ�����캯��
		CXML();
		~CXML();

		CXML(const CXML&) = delete;
		CXML(CXML&&) = delete;
		CXML& operator=(const CXML&) = delete;
	public:
		//����xml�ļ�
		bool LoadXmlFile(const string xmlFilePath = NULL,
			ios_base::openmode xmlOpenMode = XMLREAD);

		//����xml�ļ�
		void CloseXmlFile();

		//��ȡ���ڵ�
		//#strRootName:���ڵ������
		bool ReadRootNodeName(string& strRootName);

		//��ȡ���ڵ����Ե�ֵ
		/*
		#strAttribute:���ڵ������
		#strValue:���ڵ��ֵ
		*/
		bool ReadRootNodeValue(string strAttribute, string& strValue);

		//��ȡ���ӽڵ�
		/*
		#strChildName:���ӽڵ������
		#strChildAttribute�����ӽڵ������
		#strValue�����ӽڵ��ֵ
		#lBegin����ʼ��ȡ��λ��
		*/
		long ReadChildNodeValue(string strChildName, string strChildAttribute, string& strValue, long lBegin = 0);

		//��ȡȫ�����ӽڵ������
		//#strChildNodeName:���պ��ӽڵ������
		//#lBegin:��ʼ�����Ŀ�ʼλ��
		long ReadChildNodeAllName(string& strChildNodeName, long lBegin = 0);

		//��ȡ���ӽڵ��ȫ������
		//#strChildNodeName:���ӽڵ������
		//#strChildAttributeList:���ӽڵ������б�
		//#lBegin:��ʼ�����Ŀ�ʼλ��
		bool ReadChildNodeAllAttribute(string strChildNodeName,string& strChildAttributeList,long lBegin=0);

		//д��xml�ļ��汾��Ϣ
		/*
		#strVersion:�汾��Ϣ
		#strEncoding:������Ϣ
		*/
		void WriteEdtion(string strVersion="1.0",string strEncoding="UTF-8");

		//д��xml�ļ���������
		//#strRootName:���ڵ������
		void WriteRootNodeName(string strRootName);
		
		//д���������
		/*
		#strRootName:���ڵ������
		#strRootAttribute:���ڵ������
		#strRootValue:���ڵ����Ե�ֵ
		*/
		void WriteRootNodeAttribute(string strRootName, string strRootAttribute, string strRootValue);


		//д�뺢�ӽڵ�,ָ����һ���ڵ�����
		/*
		#strChildNodeName:���ӽڵ������
		#strLastNodeName:��һ���ڵ������
		#lBegin:��ʼ������λ��
		*/
		void WriteChildNodeName(string strChildNodeName, string strLastNodeName, long lBegin = 0);


		//д�뺢�ӽڵ�����
		/*
		#strChildNodeName:���ӽڵ�����
		#strChildNodeAttribute:���ӽڵ�����
		#strChildValue:���ӽڵ����Ե�ֵ
		#lBegin����ʼ��������ʼλ��
		*/
		void WriteChildNodeAttribute(string strChildNodeName, string strChildNodeAttribute,
			string strChildValue, long lBegin=0);

	};


}


