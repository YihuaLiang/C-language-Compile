#ifndef MyJudge
#define MyJudge

#include <string.h>
#include <ctype.h>

#define NUM_KEY 42 //关键字
#define NUM_OP 39
#define NUM_BORDER 8

#define KEY 1 //关键字
#define OP 2  //运算符
#define BORDER 3 //界限符
#define NAME 4  //标识符
#define CONST_I 51 //整数常量
#define CONST_S 52  //字符串常量
#define CONST_C 53  //字符常量
#define CONST_F 54 //浮点数常量
#define ERROR 6 //错误 - NAME 命名错误

#define STR 7   //位图字符串标记
#define CHAR 8  //字符标记
#define CONV 9  //位图转义标记
#define NORMAL 10 //正常标记
char keyword[NUM_KEY][20]
{
	"auto" ,"break","case","char","const","continue","default","do",
	"double","else","enum","extern","float","for","goto","if",
	"int","long","register","return","short","signed","sizeof","static",
	"struct","switch","typedef","unsigned","union","void","volatile","while"
	,"_Alignas","_Alignas","_Atomic","_Bool","_Complex","_Generic","_Imaginary","_Noreturn"
	,"_Static_assert","_Thread_local" };
char op[39][10] = {
	"[","]","+","++","+=","-","--","-=",
	"*","*=","/","//=","=","==","<","<=",
	">",">=","<<","<<=",">>=",">>","%","%=",
	"!","!=","&","&&","|","||","|=","&=",
	"~","~=","^","^=","?:","->","."
};
char border[8][10] = {
	"{","}","(",")",",","\"","\'",";"
};

int JudgeKey(char word[])
{
	for (int i = 0; i < NUM_KEY; i++)
	{
		if (strcmp(word, keyword[i]) == 0) return 1;
	}
	return 0;
}

int JudgeOp(char word[])
{
	for (int i = 0; i < NUM_OP; i++)
	{
		if (strcmp(word, op[i]) == 0) return 1;
	}
	return 0;
}

int JudgeBorder(char word[])
{
	for (int i = 0; i < NUM_BORDER; i++)
	{
		if (strcmp(word, border[i]) == 0) return 1;
	}
	return 0;
}

int JudgeType(int flag_str, int flag_conv)
{//参数分别表示当前指针位置，两个标记，标记位图
 //判定依据 当出现字符串标记的时候，转义符号有效 说明转义符号应当被消去 后面的字符认为是字符 而非符号
 //当不出现字符串标记时，不能够出现转义符号有效的情况
	if (flag_str == 0)//不做处理
		return  NORMAL;
	else
	{
		if (flag_conv == 1)
		{
			return CONV;//返回CONV表示当前字符不应该读入成为变量
		}
		else
		{
			return STR; //表示该字符是字符串中的部分，不用进行处理
		}
	}
}
//区分字符串常量和变量名的方法 - 在输入的时候包含两侧的双引号 
int JudgeWord(char word[])
{
	if (JudgeKey(word) == 1) return KEY;
	if (JudgeOp(word) == 1) return OP;
	if (JudgeBorder(word) == 1) return BORDER;
	if (word[0] == 'u' || word[0] == 'U' || word[0] == 'L')
		if (word[1] == '\'') return CONST_C;
		else if (word[1] == '\"') return CONST_S;
		else if (word[0] == 'u'&&word[1] == '8'&&word[2] == '\"') return CONST_S;
		else if (word[0] == 'u'&&word[1] == '8'&&word[2] == '\'') return CONST_C;
	if (word[0] == '\"') return CONST_S;
	else if (word[0] == '\'') return CONST_C; //说明是字符串
	else if (isdigit(word[0])||word[0] == '-'||word[0] == '+')//如果是数字那么全都是数字 其中只能包含一个小数点
	{
		int p = 1; int flag_point = 0, flag_E = 0, flag_pn = 0;
		while (word[p] != '\0')
		{
			if (word[p] == '.')
			{
				if (flag_point == 0) flag_point = 1;
				else if (flag_point == 1) return ERROR; //既不是字符串也不符合命名规则
			}
			else if (word[p] == 'e' || word[p] == 'E')
			{
				if (flag_E == 0) flag_E = 1;
				else if (flag_E == 1) return ERROR;
			}
			else if (word[p] == '+' )
			{
				if (flag_pn == 0) flag_pn = 1;
				else if (flag_pn != 0) return ERROR;
			}
			else if (word[p] == '-')
			{
				if (flag_pn == 0) flag_pn = -1;
				else if (flag_pn != 0) return ERROR;
			}
			p++;
		}
		if (flag_E==1&&(flag_pn == -1)) return CONST_F;
		else if (flag_point == 1 && flag_E == 0) return CONST_F;
		else return CONST_I;
	}//
	else//不是数字开头的常量 也不是字符串 - 那么应该是标识符
	{
		int p = 0;
		while (word[p] != '\0')
		{
			//一旦出现不可显示的字符，则判定为非法
			//isctrl && ! isspace 或直接用isprint
			if (!isalpha(word[p]) && !isdigit(word[p]) && word[p] != '_') return ERROR;
			p++;
		}
	}
	return NAME;
}


#endif // Judge.h