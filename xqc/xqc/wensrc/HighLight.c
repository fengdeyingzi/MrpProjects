#include "mrc_base.h"
#include "HighLight.H "
#include "font.h"
////////////用于画高亮的关键字用的  ///////////////////////////
//by望尘11
///////////////////////////////////////////////////////////////


HIGHLIGHTCOR HighLightCol;
//颜色类型
enum {
	COR_keyword,//关键字
	COR_annotation,//注释
	COR_text,//文本
	COR_character,//字符
	COR_macro,//宏
	COR_variate,//变量
	COR_function,//函数名

	COR_END
};
mr_colourSt LowLightCol;
static char *KeyWordsList[60];////关键字列表 37+8
static char KeyWordsLenList[50];////关键字长度列表  
static int KeyWordsNum;//关键字个数
static char AsciiType[256];//asciitype
static int HighLightType;
extern int SCRW,SCRH;
extern int TextWN(char *text,int w);//测试一段字符要w宽需要多长
extern int TextNW(char *text,int len);//测试一段字符前len的显示宽度
void DrawTextKeyWord(char *text,int32 x,int32 y);

//按类型返回指定颜色指针
mr_colourSt *hig_typecol(long type)
{
	switch(type)
	{
		case COR_keyword://关键字
			return &HighLightCol.keyword;
		case COR_annotation://注释
			return &HighLightCol.annotation;
		case COR_text://文本
			return &HighLightCol.text;
		case COR_character://字符
			return &HighLightCol.character;
		case COR_macro://宏
			return &HighLightCol.macro;
		case COR_variate://变量
			return &HighLightCol.variate;
		case COR_function://函数名
			return &HighLightCol.function;
	}
	return NULL;
}
//按类型显示文本
void hig_drawtxt(char *dt,int x,int y,long type)
{
	mr_colourSt *col;
	col=hig_typecol(type);
	if(col)
		gbsez(dt,x,y,col->r,col->g,col->b,0,FONT);
}
//按类型设置颜色
void hig_setcol(long type,uint8 r,uint8 g,uint8 b)
{
	mr_colourSt *col;
	col=hig_typecol(type);
	if(col)
	{
		col->r=r;
		col->g=g;
		col->b=b;
	}
}
void HighLightInit()//
{
	int i=0;
	HighLightType=1;

	///删除一些不用的可以加快速度
	KeyWordsList[i++]="auto";
	KeyWordsList[i++]="break";
	KeyWordsList[i++]="case";
	KeyWordsList[i++]="char";
	KeyWordsList[i++]="const";
	KeyWordsList[i++]="continue";
	KeyWordsList[i++]="default";
	KeyWordsList[i++]="do";
	KeyWordsList[i++]="double";
	KeyWordsList[i++]="else";
	KeyWordsList[i++]="enum";
	KeyWordsList[i++]="extern";
	KeyWordsList[i++]="float";
	KeyWordsList[i++]="for";
	KeyWordsList[i++]="goto";
	KeyWordsList[i++]="if";
	KeyWordsList[i++]="inline";
	KeyWordsList[i++]="int32";
	KeyWordsList[i++]="uint32";
	KeyWordsList[i++]="int16";
	KeyWordsList[i++]="uint16";
	KeyWordsList[i++]="int8";
	KeyWordsList[i++]="uint8";
	KeyWordsList[i++]="int";
	KeyWordsList[i++]="long";
	KeyWordsList[i++]="register";
	KeyWordsList[i++]="restrict";
	KeyWordsList[i++]="return";
	KeyWordsList[i++]="short";
	KeyWordsList[i++]="signed";
	KeyWordsList[i++]="sizeof";
	KeyWordsList[i++]="static";
	KeyWordsList[i++]="struct";
	KeyWordsList[i++]="switch";
	KeyWordsList[i++]="typedef";
	KeyWordsList[i++]="union";
	KeyWordsList[i++]="unsigned";
	KeyWordsList[i++]="void";
	KeyWordsList[i++]="volatitle";
	KeyWordsList[i++]="while";
	KeyWordsList[i++]="NULL";
	KeyWordsList[i++]="_Bool";
	KeyWordsList[i++]="_Complex";
	KeyWordsList[i++]="_Imaginary";
	KeyWordsList[i++]="#define";
	KeyWordsList[i++]="#undef";
	KeyWordsList[i++]="#if";
	KeyWordsList[i++]="#ifdef";
	KeyWordsList[i++]="#ifndef";
	KeyWordsList[i++]="#else";
	KeyWordsList[i++]="#endif";
	KeyWordsList[i++]="#include";

	KeyWordsNum=i;
	for(i=0;i<KeyWordsNum;i++)//计算每个关键字长度
		KeyWordsLenList[i]=mrc_strlen(KeyWordsList[i]);
	mrc_memset(AsciiType,0,256);
	for(i='A';i<='Z';AsciiType[i++]=1);
	for(i='a';i<='z';AsciiType[i++]=1);
	AsciiType['_']=1;
	for(i='0';i<='9';AsciiType[i++]=2);
	AsciiType['#']=3;
	//AsciiType['/']=4;

	hig_setcol(COR_keyword,85,50,240);
	hig_setcol(COR_annotation,50,200,100);
	hig_setcol(COR_text,240,20,10);
	hig_setcol(COR_character,240,20,10);
	LowLightCol.r=50;LowLightCol.g=200;LowLightCol.b=100;
}
void DrawTextKeyWord(char *text,int32 x,int32 y)
{
	int i=0,j=0;
	char ptype,tempchar;
	char *st;
	ptype = 0 ;
	if(HighLightType!=1)
		return ;
	for(i=0;text[i]!=0;)
	{
		if(text[i]<=0x7f)//英文
		{

			if(text[i]=='"')//双引号
			{
				ptype = 0;
				st=mrc_strstr(text+i+1,"\"");
				if(st!=NULL)//找到了
				{
					tempchar = st[1];
					st[1]=0;
					hig_drawtxt(text+i,x+TextNW(text,i),y,COR_text);
					st[1]=tempchar;
					i+=1+st-text-i;//跳过这段文本
					continue ;
				}
				else
				{
					hig_drawtxt(text+i,x+TextNW(text,i),y,COR_text);
					return ;
				}
			}
			else
				if(text[i]=='\'')//单引号
				{
					ptype = 0;
					st=mrc_strstr(text+i+1,"'");
					if(st!=NULL)//找到了
					{
						tempchar = st[1];
						st[1]=0;
						hig_drawtxt(text+i,x+TextNW(text,i),y,COR_text);
						st[1]=tempchar;
						i+=1+st-text-i;//跳过这段文本
						continue ;
					}
					else
					{
						hig_drawtxt(text+i,x+TextNW(text,i),y,COR_character);
						return ;
					}
				}
				else
					if(text[i]=='*')
					{
						ptype = 0;
						if(text[i+1]=='/')//多行注释
						{
							st=mrc_strstr(text+i+1,"/");
							if(st!=NULL)//找到了
							{
								tempchar = st[1];
								st[1]=0;
								hig_drawtxt(text,x,y,COR_annotation);
								st[1]=tempchar;
								i+=1+st-text-i;//跳过这段文本
								continue ;
							}
							else
							{
								//hig_drawtxt(text+i,x+TextNW(text,i),y,COR_annotation);
								return ;
							}
						}

					}
					else
						if(text[i]=='/')
						{

							//是反斜杠 表明后面可能是单行注释
							ptype = 0;
							if(text[i+1]=='/')//两个反斜杠 单行注释
							{
								hig_drawtxt(text+i,x+TextNW(text,i),y,COR_annotation);
								return ;
							}
							else
								if(text[i+1]=='*')//多行注释
								{
									st=mrc_strstr(text+i+2,"*/");
									if(st!=NULL)//找到了
									{
										tempchar = st[2];
										st[2]=0;
										hig_drawtxt(text+i,x+TextNW(text,i),y,COR_annotation);
										st[2]=tempchar;
										i+=2+st-text-i;//跳过这段文本
										continue ;
									}
									else
									{
										hig_drawtxt(text+i,x+TextNW(text,i),y,COR_annotation);
										return ;
									}
								}
						}
						switch(AsciiType[text[i]])
						{
						case 0://其他字符
							ptype = 0;
							break;
						case 1://字母和下划线
							if(!ptype)//找到一个新的关键字   
							{
								ptype = 1;
								for(j=0;j<KeyWordsNum;j++)
									if(!mrc_strncmp(KeyWordsList[j],text+i,KeyWordsLenList[j]))
										if(!AsciiType[(text+i+KeyWordsLenList[j])[0]])//是关键字
										{
											hig_drawtxt(KeyWordsList[j],x+TextNW(text,i),y,COR_keyword);
											break;
										}
							}
							else//正在关键字中   直接跳过
							{
							}
							break;
						case 2://数字
							//原来是什么状态就是什么状态  无需处理
							break;
						case 3://#号
							if(!ptype)//找到一个新的关键字   
							{
								ptype = 1;
								for(j=0;j<KeyWordsNum;j++)
									if(!mrc_strncmp(KeyWordsList[j],text+i,KeyWordsLenList[j]))
										if(!AsciiType[*(text+i+KeyWordsLenList[j])])//是关键字
										{
											hig_drawtxt(KeyWordsList[j],x+TextNW(text,i),y,COR_keyword);
											break;
										}
							}
							else//正在关键字中   直接跳过
							{
							}
							break;
						}
						i++;
		}
		else
			i+=text[i]?2:1;

	}
	//hig_drawtxt(text,x,y,HighLightCol.r,HighLightCol.g,HighLightCol.b,0,FONT);
}