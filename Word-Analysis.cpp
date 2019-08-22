#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "MyJudge.h"
#include "write_xml\tinystr.h"
#include "write_xml\tinyxml.h"
#define MAX_FILE 100000
using namespace std;
//创建xml文件
char output[_MAX_PATH]; //路径名称定义在全局
int * bit = NULL;

struct word
{
	char valid[10];//是否正确
	int row;//第几行
	char type[260];//单词类型
	char value[260];//单词取值
	char tips[10];
};
//向目标文件中写入信息
void WriteIn(struct word now, TiXmlElement *Root)
{
	TiXmlElement *tokenElement = new TiXmlElement("token");//Stu  
														 //设置属性  
	Root->LinkEndChild(tokenElement);//父节点写入文档  
  
	TiXmlElement *valueElement = new TiXmlElement("value");
	tokenElement->LinkEndChild(valueElement);

	TiXmlText *valueContent = new TiXmlText(now.value);
	valueElement->LinkEndChild(valueContent);

	TiXmlElement *typeElement = new TiXmlElement("type");
	tokenElement->LinkEndChild(typeElement);

	TiXmlText *typeContent = new TiXmlText(now.type);
	typeElement->LinkEndChild(typeContent);
  
	TiXmlElement *lineElement = new TiXmlElement("line");
	tokenElement->LinkEndChild(lineElement);

	char line[10];
	sprintf(line, "%d", now.row);
	TiXmlText *lineContent = new TiXmlText(line);
	lineElement->LinkEndChild(lineContent);

	TiXmlElement *validElement = new TiXmlElement("valid");
	tokenElement->LinkEndChild(validElement);

	TiXmlText *validContent = new TiXmlText(now.valid);
	validElement->LinkEndChild(validContent);

//TiXmlElement *tipsElement = new TiXmlElement("tips");
//tokenElement->LinkEndChild(tipsElement);
//
//TiXmlText *tipsContent = new TiXmlText(now.tips);
//tipsElement->LinkEndChild(tipsContent);
}

//分类
//返回一个单词以及其全部属性
//输入一个单词 判断其类型及正误
struct word Classify(char word[], int line, char tips[])
{
	int  type;
	struct word current;
	word[strlen(word)] = '\0';
	current.row = line;//写入不因情况改变的词
	strcpy(current.value, word);//如果是字符 或字符串会包含两侧的引号
	strcpy(current.tips, tips);
	//调用判定函数 - 得到类型 1 - 6 共6种
	type = JudgeWord(word);
	if (type == ERROR)
	{
		cout << "in line " << line << " " << word << " is invalid" << endl;
		strcpy(current.type, "unknown");
		strcpy(current.valid, "false");
	}
	else
	{
		switch (type)
		{
		case KEY: strcpy(current.type, "keyword"); break;
		case OP: strcpy(current.type, "operator"); break;
		case BORDER: strcpy(current.type, "seperator"); break;
		case NAME: strcpy(current.type, "identifer"); break;
		case CONST_I: strcpy(current.type, "const_i"); break;
		case CONST_F: strcpy(current.type, "const_f"); break;
		case CONST_S: strcpy(current.type, "stringLiteral"); break;
		case CONST_C: strcpy(current.type, "characterConstant"); break;
		}
		strcpy(current.valid, "true");
	}
	return current;
}
//输入 一个文件和当前读写指针的位置
//处理 进行分词和词的处理- 没有进行 \换行功能的处理
void CreateWord( char word[],int length, TiXmlElement *Root)
{
	int point = 0;     //整体扫描标记	
	int line = 1;      //行数
	int flag_str = 0;  //字符串标记
	int flag_conv = 0; //符号转义标志 
	int flag_three = 0;//三目运算符标记
	int flag_num = 0;//数字读入标记
	int end = 0;       //读取指针
	char tmp[260] = { 0 };
	while (point < length)
	{
		char now = word[point];//取出方便处理
		if (isspace(now))
		{	if(now == '\n')line++;//行数+1
			if (end != 0 )
			{
				if (flag_str == 0)//当记录串不空 且不是变量内部的空格和回车等
				{
					tmp[end] = '\0';//输入结束标记
					WriteIn(Classify(tmp, line,""),Root);					
					end = 0;//结尾标记清零
					memset(tmp, 0, sizeof(tmp));
				}
				else
				{
					tmp[end] = now; end++;//传入值，指针后移
				}
			}
			//如果为空 那么当前没有被处理的值
		}//此时读完了一个单词 或在字符串中 - 情况1
		 //以下对词法分析没有影响 //直接传入处理 不需要其他操作 - 小数在读取时分成三个部分
		else if (now == ';' || now == '.' ||now == '{' || now == '}' ||	now == '(' || now == ')'||now ==']'||now=='[' || now == ',')
		{
			if (now == '.' &&flag_num == 1)
			{
				tmp[end++] = now;//说明是小数点直接读入
			}
			else if (end != 0)
			{
				if (flag_str == 0)//当记录串不空 且不是变量内部的空格和回车等
				{
					tmp[end] = '\0';//输入结束标记
					WriteIn(Classify(tmp, line,""),Root);
					end = 0;//结尾标记清零
					tmp[0] = now; tmp[1] = '\0';//处理当前字符
					WriteIn(Classify(tmp, line,""), Root);
					flag_num = 0;//其他符号会导致数字的结束
					memset(tmp, 0, sizeof(tmp));//清零
				}
				else
				{
					tmp[end] = now; end++;//传入值，指针后移
				}
			}
			else//tmp 为空时直接读入和处理该符号
			{
				tmp[0] = now; tmp[1] = '\0';//处理当前字符
				WriteIn(Classify(tmp, line,""), Root);
				memset(tmp, 0, sizeof(tmp));//清零}//如果为空 那么当前没有被处理的值
			}
		}//读出一个符号 该符号必定独立的词-  ; . { } ( ) [ ] , 则前面的词结束 - 处理 并处理该词 - 
		 //特判. 出现在数字中时 - 体现为后面跟数字 说明不是界限符而是小数点
		else if (now == '\'' || now == '\"')
		{
			int type = 	JudgeType(flag_str, flag_conv);
			if (type == CONV)
			{
				tmp[end] = now; end++;//贝转义 是普通的字符
				flag_conv = 0;//被转义的符号已经处理
			}
			else if (type == STR)
			{
				flag_str = 0;//标志该字符串或字符压入结束
				tmp[end] = now; end++;
				tmp[end] = '\0';
				WriteIn(Classify(tmp, line,""), Root);
				end = 0;
				memset(tmp, 0, sizeof(tmp));
			}
			else
			{   //此时tmp应该必然为空，不存在tmp不为空 且后接字符串变量的情况 - 只是为了保险
				//处理串中的内容，
				if (end != 0)//修改 使满足前缀u l的情况
				{
					tmp[end] = '\0';
					if (strcmp(tmp, "u8") == 0 || strcmp(tmp, "U") == 0|| strcmp(tmp,"u") == 0||  strcmp(tmp, "L") == 0 )
					{
						//匹配一个则说明串中是目前的前缀
						flag_str = 1;
						tmp[end] = now;
						end++; point++; continue;
					}
					else if (flag_str == 0)
					{
						tmp[end] = '\0';//输入结束标记
						WriteIn(Classify(tmp, line,""), Root);
						end = 0;//结尾标记清零
						tmp[0] = now; tmp[1] = '\0';//处理当前字符
						WriteIn(Classify(tmp, line,""), Root);
						memset(tmp, 0, sizeof(tmp));//清零
					}
				}
				flag_str = 1;//标志一个字符串或者字符的开始 处理tmp当中的内容
				memset(tmp, 0, sizeof(tmp)); end = 0;
				tmp[end] = now; end++;	
			}
		}//只能包括一个字符 使用位图标记，当没有标记的时候需要向后扫描加入标记 并将符号压入表示后面的值不是标识符
		//可以包括一个句子 当有标记的时候，不需要处理 直接识别。
		else if (now == '\\')//无论怎样不会导致进入处理
		{
			int type = JudgeType(flag_str, flag_conv);//查看是普通字符还是转义处理
			if (type == CONV){
				//说明是句中的字符串中符号
				tmp[end] = now; end++;
				flag_conv = 0;
			}
			else{
				flag_conv = 1;//说明是转义符号 - 所以不读入且指针后移
			}
		}
		else if(isalpha(now))//字母
		{
			//字符
			if (flag_conv == 1){
				char a[10];
				sprintf(a, "\\%c", now);//只考虑转义厚街字母的情况
				strcat(tmp, a);
				point += strlen(tmp)-1-end;//指针移到末尾
				end = strlen(tmp);//获取该词
				flag_conv = 0;
				continue;
			}
			//直接拼接
			else	tmp[end] = now; end++;
		}
		else if (isdigit(now))
		{
			if( end == 0 )//当前为空
				flag_num = 1;//打开数字标记
			tmp[end++] = now;
		}
		else//剩余符号的情况 首先处理tmp当中的内容
		{   //处理type
			if (flag_str == 1) {
				tmp[end++] = now; point++; continue;
			}
			char next = word[point + 1];//取出后一个字符
			if (now == '+' || now == '-' || now == '&' || now == '|') //两个自身 或接等号
			{
				if (flag_num == 1 && (now == '+' || now == '-')&&(word[point-1] == 'e'||word[point-1] == 'E'))
				{
					tmp[end++] = now; point++; continue;
				}//科学计数法内部的正负号
				else if (next == now || next == '=')
				{
					char a[10];
					sprintf(a, "%c%c\0", now, next);
					WriteIn(Classify(a, line,""), Root);
					point++;//字符串额外的向后移动
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
						memset(tmp, 0, sizeof(tmp)); end = 0;
					}
				}
				else
				{
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
					}
					tmp[0] = now; tmp[1] = '\0';
					WriteIn(Classify(tmp, line,""), Root);
					memset(tmp, 0, sizeof(tmp)); end = 0;
				}
			}
			else if (now == '<' || now == '>')//两个自身 接等号 或两个自身 且接等号
			{   
				char a[10];
				if (next == now)
				{
					char last = word[point + 2];
					if (last == '=')
					{
						sprintf(a, "%c%c%c\0", now, next, last);
						point += 2;
					}
					else
					{
						sprintf(a, "%c%c\0", now, next);
						point += 1;
					}
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
						memset(tmp, 0, sizeof(tmp)); end = 0;
					}
					WriteIn(Classify(a, line,""), Root);
				}
				else if (next == '=')
				{
					sprintf(a, "%c%c\0", now, next);
					WriteIn(Classify(a, line,""), Root);
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
						memset(tmp, 0, sizeof(tmp)); end = 0;
					}
					point += 1;
				}
				else
				{//是单独的符号
					if (end != 0)
					{
						WriteIn(Classify(tmp, line, ""), Root);
						memset(tmp, 0, sizeof(tmp)); end = 0;
					}
					tmp[0] = now; tmp[1] = '\0';
					WriteIn(Classify(tmp, line, ""), Root);
					memset(tmp, 0, sizeof(tmp)); end = 0;
				}
			}
			else if (now == '*' || now == '/' || now == '!' || now == '~'||now =='='||now == '%') //接等号
			{
				if (next == '=')
				{
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
						memset(tmp, 0, sizeof(tmp)); end = 0;
					}
					char a[10];
					sprintf(a, "%c%c\0", now, next);
					WriteIn(Classify(a, line,""), Root);
					point++;//字符串额外的向后移动
				}
				else
				{
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
					}
					tmp[0] = now; tmp[1] = '\0';
					WriteIn(Classify(tmp, line,""), Root);
					memset(tmp, 0, sizeof(tmp)); end = 0;
				}
			}
			else if (now == '?')//三目运算符 添加三目运算符标记
			{
				flag_three++;
				if (end != 0){
					WriteIn(Classify(tmp, line, ""), Root);
				}
				tmp[0] = now; tmp[1] = '\0';
				WriteIn(Classify(tmp, line,""), Root);
				memset(tmp, 0, sizeof(tmp)); end = 0;
			}
			else if( now == ':')//三目运算符 - 接自身
			{
				char next = word[point + 1];
				if (flag_three > 1)//说明是三目运算符
				{
					flag_three -= 1;
					if (end != 0){
						WriteIn(Classify(tmp, line, ""), Root);
					}
					tmp[0] = now; tmp[1] = '\0';
					WriteIn(Classify(tmp, line,"three"), Root);
					memset(tmp, 0, sizeof(tmp)); end = 0;
				}
				else//向后查探一位
				{
					if (next == now)//说明是：：运算符
					{
						if (end != 0){
							WriteIn(Classify(tmp, line, ""), Root);
							memset(tmp, 0, sizeof(tmp)); end = 0;
						}
						char a[10];
						sprintf(a, "%c%c\0", now, next);
						WriteIn(Classify(a, line, ""), Root);
						point++;//字符串额外的向后移动
					}
					else
					{
						if (end != 0){
							WriteIn(Classify(tmp, line, ""), Root);
						}
						tmp[0] = now; tmp[1] = '\0';
						WriteIn(Classify(tmp, line, ""), Root);
						memset(tmp, 0, sizeof(tmp)); end = 0;
					}
				}
			}
			flag_num = 0;//运算符的出现会导致结束
		}
		point++; 	    //后移指针
	}
	struct word endd;
	endd.row = -1;
	strcpy(endd.value,"#");
	strcpy(endd.type, "#");
	strcpy(endd.valid, "true");
	WriteIn(endd, Root);
}

int main(int argc, char * argv[])
{
	//读入文件到缓冲区
	char textname[_MAX_PATH];
	scanf("%s", textname);
	//一次性读入全部的文件 到buffer
	ifstream file;//打开文件
	file.open(textname);//读出长度
	file.seekg(0, ios::end);
	int length = file.tellg();
	file.seekg(0, ios::beg);

	char buffer[MAX_FILE];//申请内存空间
	
	memset(buffer, 0, sizeof(buffer));//清空将要使用的内存

	file.read(buffer, length);
	file.close();
	int p = 0;
	while (textname[p] != '.')
	{		p++; 	}
	textname[p] = '\0';//生成目标串名称
	//开始操作
	sprintf(output,"%s.token.xml",textname);
	//写入xml文件的根节点
	TiXmlDocument *writeDoc = new TiXmlDocument; //xml文档指针  
    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");//文档格式声明  
	writeDoc->LinkEndChild(decl); //写入文档  

	TiXmlElement *RootElement = new TiXmlElement("Project");//根元素  
	RootElement->SetAttribute("name", output); //属性  
	writeDoc->LinkEndChild(RootElement);

	TiXmlElement *Root2Element = new TiXmlElement("tokens");//根元素  
	RootElement->LinkEndChild(Root2Element);
	//开始分词
	CreateWord(buffer, length,Root2Element);
	//分词结束 写入
	writeDoc->SaveFile(output);
	delete writeDoc;

	cout << "Done" << endl;
	system("pause");
	return 0;
};