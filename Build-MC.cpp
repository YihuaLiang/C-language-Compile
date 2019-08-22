#include "MAP.h"

int main(int argc, char **argv)
{
	TiXmlDocument *myDocument = new TiXmlDocument("mytest.tree.xml");    //打开一个xml文件
	myDocument->LoadFile();

	TiXmlElement *node = myDocument->FirstChildElement();      //返回当前节点的子节点
	if (node == NULL)               //若为空则返回
	{
		return 0;
	}
     build(node);
	//readxml(node);
	return 0;
}