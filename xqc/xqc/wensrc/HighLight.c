#include "mrc_base.h"
#include "HighLight.H "
#include "font.h"
////////////���ڻ������Ĺؼ����õ�  ///////////////////////////
//by����11
///////////////////////////////////////////////////////////////


HIGHLIGHTCOR HighLightCol;
//��ɫ����
enum {
	COR_keyword,//�ؼ���
	COR_annotation,//ע��
	COR_text,//�ı�
	COR_character,//�ַ�
	COR_macro,//��
	COR_variate,//����
	COR_function,//������

	COR_END
};
mr_colourSt LowLightCol;
static char *KeyWordsList[60];////�ؼ����б� 37+8
static char KeyWordsLenList[50];////�ؼ��ֳ����б�  
static int KeyWordsNum;//�ؼ��ָ���
static char AsciiType[256];//asciitype
static int HighLightType;
extern int SCRW,SCRH;
extern int TextWN(char *text,int w);//����һ���ַ�Ҫw����Ҫ�೤
extern int TextNW(char *text,int len);//����һ���ַ�ǰlen����ʾ���
void DrawTextKeyWord(char *text,int32 x,int32 y);

//�����ͷ���ָ����ɫָ��
mr_colourSt *hig_typecol(long type)
{
	switch(type)
	{
		case COR_keyword://�ؼ���
			return &HighLightCol.keyword;
		case COR_annotation://ע��
			return &HighLightCol.annotation;
		case COR_text://�ı�
			return &HighLightCol.text;
		case COR_character://�ַ�
			return &HighLightCol.character;
		case COR_macro://��
			return &HighLightCol.macro;
		case COR_variate://����
			return &HighLightCol.variate;
		case COR_function://������
			return &HighLightCol.function;
	}
	return NULL;
}
//��������ʾ�ı�
void hig_drawtxt(char *dt,int x,int y,long type)
{
	mr_colourSt *col;
	col=hig_typecol(type);
	if(col)
		gbsez(dt,x,y,col->r,col->g,col->b,0,FONT);
}
//������������ɫ
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

	///ɾ��һЩ���õĿ��Լӿ��ٶ�
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
	for(i=0;i<KeyWordsNum;i++)//����ÿ���ؼ��ֳ���
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
		if(text[i]<=0x7f)//Ӣ��
		{

			if(text[i]=='"')//˫����
			{
				ptype = 0;
				st=mrc_strstr(text+i+1,"\"");
				if(st!=NULL)//�ҵ���
				{
					tempchar = st[1];
					st[1]=0;
					hig_drawtxt(text+i,x+TextNW(text,i),y,COR_text);
					st[1]=tempchar;
					i+=1+st-text-i;//��������ı�
					continue ;
				}
				else
				{
					hig_drawtxt(text+i,x+TextNW(text,i),y,COR_text);
					return ;
				}
			}
			else
				if(text[i]=='\'')//������
				{
					ptype = 0;
					st=mrc_strstr(text+i+1,"'");
					if(st!=NULL)//�ҵ���
					{
						tempchar = st[1];
						st[1]=0;
						hig_drawtxt(text+i,x+TextNW(text,i),y,COR_text);
						st[1]=tempchar;
						i+=1+st-text-i;//��������ı�
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
						if(text[i+1]=='/')//����ע��
						{
							st=mrc_strstr(text+i+1,"/");
							if(st!=NULL)//�ҵ���
							{
								tempchar = st[1];
								st[1]=0;
								hig_drawtxt(text,x,y,COR_annotation);
								st[1]=tempchar;
								i+=1+st-text-i;//��������ı�
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

							//�Ƿ�б�� ������������ǵ���ע��
							ptype = 0;
							if(text[i+1]=='/')//������б�� ����ע��
							{
								hig_drawtxt(text+i,x+TextNW(text,i),y,COR_annotation);
								return ;
							}
							else
								if(text[i+1]=='*')//����ע��
								{
									st=mrc_strstr(text+i+2,"*/");
									if(st!=NULL)//�ҵ���
									{
										tempchar = st[2];
										st[2]=0;
										hig_drawtxt(text+i,x+TextNW(text,i),y,COR_annotation);
										st[2]=tempchar;
										i+=2+st-text-i;//��������ı�
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
						case 0://�����ַ�
							ptype = 0;
							break;
						case 1://��ĸ���»���
							if(!ptype)//�ҵ�һ���µĹؼ���   
							{
								ptype = 1;
								for(j=0;j<KeyWordsNum;j++)
									if(!mrc_strncmp(KeyWordsList[j],text+i,KeyWordsLenList[j]))
										if(!AsciiType[(text+i+KeyWordsLenList[j])[0]])//�ǹؼ���
										{
											hig_drawtxt(KeyWordsList[j],x+TextNW(text,i),y,COR_keyword);
											break;
										}
							}
							else//���ڹؼ�����   ֱ������
							{
							}
							break;
						case 2://����
							//ԭ����ʲô״̬����ʲô״̬  ���账��
							break;
						case 3://#��
							if(!ptype)//�ҵ�һ���µĹؼ���   
							{
								ptype = 1;
								for(j=0;j<KeyWordsNum;j++)
									if(!mrc_strncmp(KeyWordsList[j],text+i,KeyWordsLenList[j]))
										if(!AsciiType[*(text+i+KeyWordsLenList[j])])//�ǹؼ���
										{
											hig_drawtxt(KeyWordsList[j],x+TextNW(text,i),y,COR_keyword);
											break;
										}
							}
							else//���ڹؼ�����   ֱ������
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