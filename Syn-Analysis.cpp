#include "LEX.h"
using namespace std;

int main(int argc, char **argv)
{
	char target[255]; 
	char output[255];
	scanf("%s", target);
	//strcpy(target, argv[1]);//给目标赋值
	read(target);
	//预处理完毕
	//进入递归下降函数进行处理
	int p = 0;
	while (target[p] != '.')
	{
		p++;
	}
	target[p] = '\0';//生成目标串名称
					   //开始
	sprintf(output, "%s.tree.xml", target);
	//写入xml文件的根节点
	TiXmlDocument *writeDoc = new TiXmlDocument; //xml文档指针  
	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");//文档格式声明  
	writeDoc->LinkEndChild(decl); //写入文档 

	TiXmlElement *RootElement = new TiXmlElement("Project");//根元素  
	RootElement->SetAttribute("name", output); //属性  
	writeDoc->LinkEndChild(RootElement);


	CMPL_UNIT(RootElement);

	writeDoc->SaveFile(output);
	delete writeDoc;

	cout << "Lex-Analysis Done" << endl;
	system("pause");
	return 0;
}