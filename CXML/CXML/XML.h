#pragma once

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

namespace CXML_space
{
#define XMLREAD  (ios::in)           //读入
#define XMLWRITE (ios::out | ios::trunc)//写入


	class CXML
	{
	private:
		//XML文件
		fstream m_XmlFile;
		//xml文件的数据
		string m_XmlBuf;
		//xml文件路径
		string m_XmlFilePath;
		//打开模式
		int m_nMode;
		//写入根节点的开始位置
		long m_lReadRootBegin;
	private:
		//读取xml文件到内存中来
		void ReadXmlFileToBuf();
		//把内存中的修改保存到xml文件
		void WriteBufToXmlFile();
		//切换文件模式
		bool SwitchFileMode();
	public:
		//添加一个构造函数
		CXML();
		~CXML();

		CXML(const CXML&) = delete;
		CXML(CXML&&) = delete;
		CXML& operator=(const CXML&) = delete;
	public:
		//加载xml文件
		bool LoadXmlFile(const string xmlFilePath = NULL,
			ios_base::openmode xmlOpenMode = XMLREAD);

		//保存xml文件
		void CloseXmlFile();

		//读取根节点
		//#strRootName:根节点的名字
		bool ReadRootNodeName(string& strRootName);

		//读取根节点属性的值
		/*
		#strAttribute:根节点的属性
		#strValue:根节点的值
		*/
		bool ReadRootNodeValue(string strAttribute, string& strValue);

		//读取孩子节点
		/*
		#strChildName:孩子节点的名字
		#strChildAttribute：孩子节点的属性
		#strValue：孩子节点的值
		#lBegin：开始读取的位置
		*/
		long ReadChildNodeValue(string strChildName, string strChildAttribute, string& strValue, long lBegin = 0);

		//读取全部孩子节点的名字
		//#strChildNodeName:接收孩子节点的名字
		//#lBegin:开始搜索的开始位置
		long ReadChildNodeAllName(string& strChildNodeName, long lBegin = 0);

		//读取孩子节点的全部属性
		//#strChildNodeName:孩子节点的名字
		//#strChildAttributeList:孩子节点属性列表
		//#lBegin:开始搜索的开始位置
		bool ReadChildNodeAllAttribute(string strChildNodeName,string& strChildAttributeList,long lBegin=0);

		//写入xml文件版本信息
		/*
		#strVersion:版本信息
		#strEncoding:编码信息
		*/
		void WriteEdtion(string strVersion="1.0",string strEncoding="UTF-8");

		//写入xml文件根的名字
		//#strRootName:根节点的名字
		void WriteRootNodeName(string strRootName);
		
		//写入根的属性
		/*
		#strRootName:根节点的名字
		#strRootAttribute:根节点的属性
		#strRootValue:根节点属性的值
		*/
		void WriteRootNodeAttribute(string strRootName, string strRootAttribute, string strRootValue);


		//写入孩子节点,指定上一个节点名字
		/*
		#strChildNodeName:孩子节点的名字
		#strLastNodeName:上一个节点的名字
		#lBegin:开始搜索的位置
		*/
		void WriteChildNodeName(string strChildNodeName, string strLastNodeName, long lBegin = 0);


		//写入孩子节点属性
		/*
		#strChildNodeName:孩子节点名字
		#strChildNodeAttribute:孩子节点属性
		#strChildValue:孩子节点属性的值
		#lBegin：开始搜索的起始位置
		*/
		void WriteChildNodeAttribute(string strChildNodeName, string strChildNodeAttribute,
			string strChildValue, long lBegin=0);

	};


}


