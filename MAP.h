#ifndef MAP
#define MAP
#include "write_xml\tinystr.h"
#include "write_xml\tinyxml.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#define INT 0
#define CHAR 1
using namespace std;

struct tt//symbol list
{
	string name;//name in c
	string symbol;// name in X86
}maplist[255];

int numSYM = 0;
int flagFALSE[255] = { 0 };//flag of FALSE 2 - use without appearance 1 - appearance and use 0 - no use
int flagDONE[255] = { 0 };
int flagBEGIN[255] = { 0 };
int flagIF =  0 ;//flag of IF 
int flagWHILE = 0 ;//flag of WHILE
int flagELSE = 0;
int flagCONDITION = 0;
int numFALSE = 0;
int numDONE = 0;
int flagASS_STMT = 0;//true when ASS_STMT is used; false not used or quit from
int flagDECLARE = 0;
int flagEXPR = 0;
int flagINT = 0;//区分开辟内存的大小
int flagCHAR = 0;
int flagFirst = 0;//区分表达式中的内容
int flagSecond = 0;
int flag_tmp[10];
int tmp_count;

int numBEGIN = 0;
char tagFALSE[20] = "FALSE";
char tagDONE[20] = "DONE";
char tagBEGIN[20] = "BEGIN";
char mc[255];
char x86Symbol[20] = "Sym";
char x86Tmp[20] = "temp";

int tmpB;
int buffer_count = 0;
int c_buffer_count = 0;
int ass_buffer_count = 0;
int con_buffer_count = 0;
string const_buffer[2];
string buffer[2];
string opbuffer;
string con_buffer[2];
string ass_buffer[2];
string sizebuffer;
string tmpRelation;
ofstream ofile;
void readxml(TiXmlElement *node)                     //读取xml文件
{
	while (node)
	{
		string tagValue;//get the tag and see what it is 
		string tagText;
		//take the content
		tagValue = node->Value();
		if (node->GetText() != NULL)
		{
			//cout << "=" << node->GetText() << endl;
			tagText = node->GetText();
		}
		else tagText = "\0";
		//根据实际的情况进行选择
		if (tagValue == "ASSIGN_STMT") {
			flagASS_STMT = 1;
			//递归
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
			flagASS_STMT = 0;
			if (flagDECLARE == 0)
			{
				ofile << "mov " << ass_buffer[0] << "," << ass_buffer[1] << endl;
				ass_buffer_count = 0;
			}
		}
		else if (tagValue == "DECLARE") {
			flagDECLARE = 1;//获取标记，向后续的操作表示，当前句子正在申请变量
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
			flagDECLARE = 0;
		}
		else if (tagValue == "IF") {
			flagIF = flagWHILE + 1;
			//递归
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}

			flagIF = 0;
			//写入完成语句
			 
			for (int i = numDONE; i >= 0; i--)
			{
				if (flagDONE[i] == 2)//找到最后一个压入的 写出序号
				{
					ofile << tagDONE << i << ":" << endl;
					flagDONE[i] = 1;
				}
			}
		}
		else if (tagValue == "WHILE") {
			flagWHILE = flagIF + 1;
			 
			ofile << tagBEGIN << numBEGIN << ":" << endl;
			tmpB = numBEGIN++;
			//递归
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}

			flagWHILE = 0;
			 
			for (int i = numFALSE; i >= 0; i--)
			{
				if (flagFALSE[i] == 2)//找到最后一个压入的 写出序号
				{
					ofile << tagFALSE << i << ":" << endl;
					flagFALSE[i] = 1;
					break;
				}
			}
		}
		else if (tagValue == "EXPR") 
		{
			flagEXPR = 1;
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
			//基于不同的情况进行处理
			if (buffer_count == 2)
			{
				ofile << "mov eax," << buffer[0] << endl;
				ofile << opbuffer << buffer[1] << endl;
				buffer_count = 0;
				if (opbuffer == "div")//除除法的结果位置不同
				{
					ofile << "mov " << x86Tmp << tmp_count << ",ax" << endl;
				}
				else
				{
					ofile << "mov " << x86Tmp << tmp_count <<",eax"<< endl;
				}
				char tmp2[255];
				sprintf(tmp2, "%s%d", x86Tmp, tmp_count++);
				if (flagASS_STMT == 1)
				{
					ass_buffer[ass_buffer_count] = tmp2;
					ass_buffer_count++;
				}
				else if (flagCONDITION == 1)
				{
					con_buffer[con_buffer_count] = tmp2;
					con_buffer_count++;
				}
			}
			else if (buffer_count == 1)
			{
				if (c_buffer_count == 1)
				{
					ofile << "mov eax," << buffer[0] << endl;
					ofile << opbuffer << const_buffer[0] << endl;
					buffer_count = 0;
					c_buffer_count = 0;
					if (opbuffer == "div")//除除法的结果位置不同
					{
						ofile << "mov " << x86Tmp << tmp_count << ",ax" << endl;
					}
					else
					{
						ofile << "mov " << x86Tmp << tmp_count << ",eax" << endl;
					}
					char tmp2[255];
					sprintf(tmp2, "%s%d", x86Tmp, tmp_count++);
					if (flagASS_STMT == 1)
					{
						ass_buffer[ass_buffer_count] = tmp2;
						ass_buffer_count++;
					}
					else if (flagCONDITION == 1)
					{
						con_buffer[con_buffer_count] = tmp2;
						con_buffer_count++;
					}
				}
				else
				{
					//此时只有一个标识符 说明此时可以直接用符号进行表示
					buffer_count = 0;
					
					if (flagASS_STMT == 1)
					{
						ass_buffer[ass_buffer_count] = buffer[0];
						ass_buffer_count++;
					}
					else if (flagCONDITION == 1)
					{
						con_buffer[con_buffer_count] = buffer[0];
						con_buffer_count++;
					}
				}
			}
			else if (c_buffer_count == 2)
			{
				ofile << "mov eax," << const_buffer[0] << endl;
				ofile << opbuffer << const_buffer[1] << endl;
				//buffer_count = 0;
				c_buffer_count = 0;
				if (opbuffer == "div")//除除法的结果位置不同
				{
					ofile << "mov " << x86Tmp << tmp_count << ",ax" << endl;
				}
				else
				{
					ofile << "mov " << x86Tmp << tmp_count << ",eax" << endl;
				}
				//tmp_count++;
				c_buffer_count = 0;
				char tmp2[255];
				sprintf(tmp2, "%s%d", x86Tmp, tmp_count++);
				if (flagASS_STMT == 1)
				{
					ass_buffer[ass_buffer_count] = tmp2;
					ass_buffer_count++;
				}
				else if (flagCONDITION == 1)
				{
					con_buffer[con_buffer_count] = tmp2;
					con_buffer_count++;
				}
			}
			else if (c_buffer_count == 1)
			{
				c_buffer_count = 0;
				
				if (flagASS_STMT == 1)
				{
					ass_buffer[ass_buffer_count] = const_buffer[0];
					ass_buffer_count++;
				}
				else if (flagCONDITION == 1)
				{
					con_buffer[con_buffer_count] = const_buffer[0];
					con_buffer_count++;
				}
				c_buffer_count = 0;
			}
			flagEXPR = 0;
		}//in the EXPR, make sure the operator and do it
		else if (tagValue == "ELSEIF")
		{
			flagELSE = 1;
			for (int i = numFALSE; i >= 0; i--)
			{
				if (flagFALSE[i] == 2)
				{
					ofile << tagFALSE << i << ":" << endl;
					flagFALSE[i] = 1;
					break;//出现，但未定义
				}
			}
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
			flagELSE = 0;
		}
		else if (tagValue == "CODE_BLOCK") 
		{ //FALSE 写在 WHILE 结束处 和IF 结束处(内部CODEBLOCK的尾部)，DONE 写在WHILE和IF的出口位置
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
			if (flagIF >= 1) //书写跳转标记 此时在if后接的CODEBLOCK 出口
			{ //跳向done
				if (flagELSE == 0)
				{
					char tmp4[255];
					flagDONE[numDONE] = 2;
					sprintf(tmp4, "%s%d", tagDONE, numDONE++);
					ofile << "jmp " << tmp4 << endl;
				}
				else
				{
					char tmp4[255];
					//flagDONE[numDONE] = 2;
			
					sprintf(tmp4, "%s%d", tagDONE, numDONE - 1);
					ofile << "jmp " << tmp4 << endl;
				}
			}
			if (flagWHILE >= 1 && flagELSE == 0 && flagIF == 0) //无论是什么，都是在CODEBLOCK的出口写FALSE，除了ELSE之外
			{
				ofile << "jmp " << tagBEGIN << tmpB << endl;
				for (int i = numFALSE; i >= 0; i--)
				{
					if (flagFALSE[i] == 2)
					{
						ofile << tagFALSE << i << ":" << endl;
						flagFALSE[i] = 1;
						break;//出现，但未定义
					}
				}					
			}
		}
		else if (tagValue == "identifer") 
		{//递归
			if (tagText == "main")
			{
				ofile << ".code" << endl << endl;
				ofile << "start :" << endl <<endl;
			}
			if (flagDECLARE == 1)//在声明当中
			{
				//此时一定不在符号表里
				maplist[numSYM].name = tagText;
				maplist[numSYM].symbol = x86Symbol;
				char tmp[2];
				sprintf(tmp, "%d", numSYM);
				maplist[numSYM].symbol += tmp;
				numSYM++;

				ofile << maplist[numSYM - 1].symbol << " "<<sizebuffer;
			}
			else if(flagEXPR == 1)//当不为0时则将内容取到eax当中备用
			{
				for (int i = 0; i < numSYM; i++)
				{
					if (tagText == maplist[i].name)
					{
						buffer[buffer_count] = maplist[i].symbol;
						buffer_count++;
						break;
					}
				}
			}
			else if (flagASS_STMT == 1)//说明是被赋值的符号 
			{
				for (int i = 0; i < numSYM; i++)
				{
					if (tagText == maplist[i].name)
					{
						ass_buffer[ass_buffer_count] = maplist[i].symbol;
						ass_buffer_count++;
						break;
					}
				}
			}
		}
		else if (tagValue == "const_i")//本次实验中只考虑常数
		{
			if (flagDECLARE == 1)//说明在定义语句内部
			{

				ofile << " " << tagText << endl;
			}
			else if (flagEXPR == 1)//不再定义语句，则应该进行运算，常数此时操作与变量类似
			{
				const_buffer[c_buffer_count] = tagText;
				c_buffer_count++;
			}
		}
		else if (tagValue == "seperator")
		{
			if (tagText == ";")//部分标记清零 类型标记，表达式标记
			{
				flagINT = 0;
				flagCHAR = 0;
			}
		//， 和 （）,{}在本次代码中没有实际意义	
		}
		else if (tagValue == "operator")
		{
			//四则运算符
			if (tagText == "+") { opbuffer =  "add eax,"; }
			else if (tagText == "-") { opbuffer = "sub eax,"; }
			else if (tagText == "*") { opbuffer = "mul "; }
			else if (tagText == "//") { opbuffer ="div"; }
			//比较操作符
			else if (tagText == "<") { tmpRelation = "jge "; }
			else if (tagText == ">") { tmpRelation = "jbe "; }
			else if (tagText == ">=") { tmpRelation = "jb "; }
			else if (tagText == "<=") { tmpRelation = "jg "; }
			else if (tagText == "==") { tmpRelation = "jnz "; }
			else if (tagText == "!=") { tmpRelation = "jz "; }
		}
		else if (tagValue == "CONDITION")
		{
			flagCONDITION = 1;
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
			ofile << "cmp " << con_buffer[0] << "," << con_buffer[1] << endl;
			ofile << tmpRelation;
			flagFALSE[numFALSE] = 2;
			char tmp3[255];
			sprintf(tmp3, "%s%d", tagFALSE, numFALSE++);
			ofile << tmp3 << endl;
			//考虑在此处添加向错误的跳转
			flagCONDITION = 0;
			con_buffer_count = 0;
		}
		else if (tagValue == "keyword")//由语法分析时的设计决定 必定在最底层，因此不用读取
		{
			if (tagText == "int") { sizebuffer = "dword"; }
			else if (tagText == "char") { sizebuffer = "byte"; }
		}
		else
		{//没有明显语义时 应当继续递归
			if (node->FirstChildElement() != NULL)
			{
				readxml(node->FirstChildElement());    //若还有子节点，则进行递归调用
			}
		}
		//遍历
		node = node->NextSiblingElement();
		if (!node)
		{
			break;
		}
	}
}
void build(TiXmlElement *node)
{
	//get the project name
	char projectname[255];
	TiXmlAttribute *attri = node->FirstAttribute();   //获取节点的第一个属性，可以用LastAttribute()获取最后一个属性节点
	while (attri != NULL)          //若属性不为空则输出
	{
		strcpy(projectname,attri->Value());
		attri = attri->Next();        //获取下一个属性节点
	}
	for (int i = 0; projectname[i] != '\0'; i++)
	{
		if (projectname[i] == '.') {
			projectname[i] = '\0'; break;
		}
	}
	
	sprintf(mc, "C:\\Build-MC\\Build-MC\\%s.asm",projectname);
	ofile.open(mc);
	ofile << ".386\n.model flat, stdcall\noption casemap : none" << endl<<endl;
	ofile << "includelib user32.lib\nincludelib kernel32.lib\nincludelib msvcrt.lib" << endl<<endl;
	//定义临时变量
	ofile << ".data\n"
		  <<"temp0 dword ?\n"
		  <<"temp1 dword ?\n"
		  <<"temp2 dword ?\n"
		  <<"temp3 dword ?\n"
		  <<"temp4 dword ?\n"
		  <<"temp5 dword ?\n"
		  <<"temp6 dword ?\n"
		  <<"temp7 dword ?\n"
          <<"temp8 dword ?\n"
		  <<"temp9 dword ?\n";
	readxml(node);
	ofile << "end start" << endl;
}
#endif