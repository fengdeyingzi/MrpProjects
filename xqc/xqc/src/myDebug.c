#include "mrc_base.h"
#include "myDebug.h"
#include "debug.h"

#define  MALLOC_LIST_MAX 200
#define  MALLOC_INFO_MAX 200
#define	 MyAssert(n) {if ((n)==0) {__asm { int 3 }}   } 

typedef struct MEM_CHECK_T
{
	void* pMem;
	int size;
	char info[MALLOC_INFO_MAX]; 
}MEM_CHECK;

static MEM_CHECK malloc_list[MALLOC_LIST_MAX];
static int malloc_count;

static void * my_memcpy (void * dst,const void * src,int count)
{
	void * ret = dst;

	while (count--) {
		*(char *)dst = *(char *)src;
		dst = (char *)dst + 1;
		src = (char *)src + 1;
	}

	return(ret);
}

void mem_init(void)
{
	malloc_count = 0;
	memset(malloc_list,0,sizeof(malloc_list));
}

static void malloc_list_add(void* address, int size, char* info)
{
	int len = 0;
	int16 i = 0;
	for (i = 0; i < MALLOC_LIST_MAX; i++)
	{
		if (malloc_list[i].pMem == NULL)
		{
			break;
		}
	}

	/*���ԣ�malloc_list����ռ��㹻*/
	MyAssert(i < MALLOC_LIST_MAX);
	if ((len = mrc_strlen(info)) > (MALLOC_INFO_MAX -1))
	{
		len = (MALLOC_INFO_MAX -1);
	}
	my_memcpy(malloc_list[i].info,info,len);
	malloc_list[i].pMem = address;
	malloc_list[i].size = size;
	*(int32*)((char*)address+size) = 19851007;
	malloc_count++;
}

static void malloc_list_delete(void* address)
{
	int i = 0;
	for (i = 0; i < MALLOC_LIST_MAX; i++)
	{
		if (malloc_list[i].pMem == address)
		{
			malloc_list[i].pMem = NULL;
			malloc_count--;
			break;
		}
	}
	/*���ԣ��ͷŵ��ڴ������ͨ��my_malloc���������*/
	MyAssert(i < MALLOC_LIST_MAX);
}

/*
�ڴ�й©��⣬������ڳ����˳�ʱ�����ж����ڴ�û���ֶ��ͷš�
*/
void mem_leak_check(void)
{

	int i = 0;
	for (i = 0; i < MALLOC_LIST_MAX; i++)
	{
		if (malloc_list[i].pMem != NULL)
		{
			FPRINTF("�ڴ�й©:��Ϣ=%s\r\n",malloc_list[i].info);
		}
	}

	FPRINTF("�ڴ�й©:����=%d\r\n",malloc_count);
}

/*
�ͷŵ�ʱ����ڴ���м��(�Ѿ�����)�ͳ����˳�ʱ��һ���ڴ���
���԰�����������ڴ涼�����һ��
*/
void mem_overflow_check(void)
{
	int i = 0;
	for (i = 0; i < MALLOC_LIST_MAX; i++)
	{
		if (malloc_list[i].pMem != NULL)
		{
			void* address = malloc_list[i].pMem;
			int size = malloc_list[i].size;
			int32 check_num = *(int32*)((char*)address+size);
			
			if (check_num != 19851007)
			{
				FPRINTF("�ڴ����:�����=%d,��Ϣ=%s\r\n",check_num, malloc_list[i].info);
				MyAssert(0);
			}
		}
	}
}

/*
����:���е��ڴ涯̬���붼��ͨ��my_malloc����
	 ���е��ڴ��ͷŶ���ͨ��my_free����
*/
void* my_mallocEx(int size, char* filename, int line)
{
	char info[200] = {0};
	/*�������int32���͵�������Ϊ�˼���ڴ��Ƿ�Խ�����*/
	void* address = mrc_malloc(size+sizeof(int32));
	if (address != NULL)
	{
		mrc_sprintf(info,"%s(%d)",filename,line);
		malloc_list_add(address,size,info);
	}
	return address;
}

void my_free(void *address)
{
	mem_overflow_check();
	if (address != NULL)
	{
		malloc_list_delete(address);
	}
	mrc_free(address);
}

/*
��ָ����mrp�ļ��ж�ȡָ�����ļ����ú�����mrc_readFileFromMrp
�������������������ͬ�⣬��������һ����ͬ:
mrc_readFileFromMrp����������ȡ��mrp�е��ļ��������ļ��Լ���ȡ
����������飻mrc_readFileFromMrpEx�Զ�ȡ������ǿ��飬���ļ�
��ѹ����ʽ��ţ�������ļ��������ݵ�ǿ��顣�ú����ʺ�
ʹ���ڼ��ش������Ҫ�ϸ���ȷ�Ա�֤�Ļ�����
����:
packname     mrp�ļ���
filename        ����ȡ�ļ����ļ���
*filebuf          ��ȡ�ļ����������
���ļ���ѹ����ʽ���ʱ����:
*filebuf==NULL����������һƬ�ռ��ŷ��ص��ļ�
*filebuf!=NULL����ʱ����ʹ��*filebufָ��Ŀռ���
���ص��ļ�����
*filelen          ��*filebuf!=NULL���ļ���ѹ����ʽ���ʱ��ָ��
*filebufָ��Ŀռ��С�����ÿռ��С������
��Ž�ѹ�Ժ���ļ�����������ʧ��
lookfor          ָ�������Ĳ�����ʽ:
0:    ��ȡmrp���ļ������ݣ���ͨ��*filebuf��*filelen
���ظ����ݣ���*filebuf==NULLʱ�����ص��ڴ�
��Ҫ�������ͷţ�
1:    ��������mrp���Ƿ���ڸ��ļ���������ȡ
�ļ�����
2:    ��mrp�ļ�λ��ROM��RAM��ʱ����ȡ����ȡ��
����ԭʼ���ݣ���ʹ�ļ�������ѹ��Ҳ
�����н�ѹ�����ص��ڴ治��Ҫ�ͷţ�
��mrp�ļ�λ���ļ�ϵͳ��ʱ����ͬ��
lookfor==0
3:    ��ǿ���ķ�ʽ��ȡmrp���ļ������ݣ�����
lookfor==0�ķ�ʽ��ȡ�ļ�ʱ���ļ��Ƿ�ѹ��
���ж�������Ӧ�ģ����Բ�����ȫ��֤�ļ�
����ȷ�ԣ�lookfor==3ʱ��ǿ��Լ���ļ�������
ѹ����������ǿ��顣��*filebuf==NULLʱ������
���ڴ���Ҫ�������ͷţ�
4:��3��ͬ����ͬ����ʹ��mrc_malloc8�����ڴ棻
5:��ͨ��filelen�����ļ����ȣ�

���:
*filebuf         ��lookfor==0��2��3ʱ�����ļ�����ָ��
*filelen         ��lookfor==0��2��3ʱ�����ļ����ݳ���

����:
MR_SUCCESS   �ɹ�����lookfor==1ʱ��ʾ�ļ�����
MR_FAILED       ʧ��

(      
���շ����в��ִ�����ʹ��mrc_readFileFromMrpEx����ʱ��
�Խ�����жϽ�ʹ����filebuf���ж��Ƿ�ΪNULL����������
�˺����ķ���ֵ��
���������ѣ�mrc_readFileFromMrpEx�����ڲ���ʧ��ʱ������
ֵ��ʧ�ܣ���filebuf��ֵ��δ���壨Ҳ����˵��������
NULL��Ҳ���ܲ���NULL�����������ʹ�ú�������ֵ��
�н���жϣ�ʹ��filebuf�����ж��ǲ����յġ�
)
*/
int32 my_readFileFromMrpEx(char* packname, const char* filename, 
							uint8 ** filebuf, int32 *filelen, int32 lookfor)
{
	int32 irtn = 0;
	int IsNeedFree = FALSE;
	if (*filebuf == NULL && lookfor == 0)
	{
		IsNeedFree = TRUE;
	}

	irtn = mrc_readFileFromMrpEx(packname,filename,filebuf,filelen,lookfor);

	if (IsNeedFree == TRUE && *filelen > 0 && MR_SUCCESS == irtn)
	{
		//void* pbuff = my_malloc(*filelen);
		void* pbuff = my_mallocEx(*filelen,(char*)filename,__LINE__);
		my_memcpy(pbuff,*filebuf,*filelen);
		mrc_free(*filebuf);
		*filebuf = (uint8*)pbuff;
	}

	return irtn;
}


/*
��ȡ�ļ����������ݵ�������ڴ��С�
����:
filename	�ļ���
���:
len           ��ȡ�������ݳ���
����:
��NULL         ָ���ȡ�������ݵ�ָ�룬���ڴ���Ҫ
�������ͷ�
NULL              ʧ��
*/
void* my_readAll(const char* filename, uint32 *len)
{
	void* prtn = NULL;
	prtn = mrc_readAll(filename,len);

	if (prtn != NULL)
	{
		//void* pbuff = my_malloc(*len);
		void* pbuff = my_mallocEx(*len,(char*)filename,__LINE__);
		my_memcpy(pbuff,prtn,*len);
		mrc_free(prtn);
		prtn = pbuff;
	}

	return prtn;
}

/*
��gb�ַ���ת��ΪUnicode�ַ�����������һƬ�ڴ汣
��Unicode�ַ�������Unicode�ַ�����ָ�뷵�ء�
��������mythroad�汾��Ӧ��ʹ�øú���ʱ��Ҫ����
����ת����ʹ�øú�����ʹĿ���������Լ16K��
����:
cp: �����gb�ַ���
err����NULL����errΪ��NULL������ת������ʱerr���س�
���ַ�������
���:
err����errΪ��NULL������ת������ʱerr���س����ַ�
������
size�������Unicode�ַ�������
����:
NULL        ת������
����    Unicode�ַ���ָ��
*/
uint16* my_c2u(char *cp, int32 *err, int32 *size)
{
	uint16* prtn = NULL;
	prtn = mrc_c2u(cp,err,size);

	if (prtn != NULL)
	{
		void* pbuff = my_malloc((*size+1)*sizeof(uint16));
		my_memcpy(pbuff,prtn,(*size+1)*sizeof(uint16));
		mrc_free(prtn);
		prtn = (uint16*)pbuff;
	}

	return prtn;
}

/*
��mrp�ж�ȡ�ļ����������ݵ�������ڴ��С�
ʹ�øú����õ����ڴ棬��Ҫʹ��my_freeFileData�����ͷš�

����:
filename     mrp�ļ���
lookfor          ָ�������Ĳ�����ʽ:
                    0:    ��ȡmrp���ļ�������
                    1:    ��������mrp���Ƿ���ڸ��ļ���������ȡ
                               �ļ�����

���:
*filelen         ��lookfor==0ʱ�����ļ����ݳ���
                   ��lookfor==1ʱδ֪

����:
   ��lookfor==0ʱ
      ��NULL         ָ���ȡ�������ݵ�ָ�룬���ڴ���Ҫ
                              �������ͷ�
      NULL         ʧ��
   ��lookfor==1ʱ
      1         mrp�д��ڸ��ļ�
      NULL         mrp�в����ڸ��ļ�
*/

void * my_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor)
{
	void* prtn = NULL;
	int IsNeedFree = FALSE;
	if ( lookfor == 0)
	{
		IsNeedFree = TRUE;
	}

	prtn = mrc_readFileFromMrp(filename,filelen,lookfor);

	if (IsNeedFree == TRUE && *filelen > 0 && prtn != NULL)
	{
		//void* pbuff = my_malloc(*filelen);
		void* pbuff = my_mallocEx(*filelen,(char*)filename,__LINE__);
		my_memcpy(pbuff,prtn,*filelen);
		mrc_freeFileData(prtn,*filelen);
		prtn = pbuff;
	}

	return prtn;

}

/*
�ͷ���my_readFileFromMrp������ȡ���ļ����ݿռ䡣
����:
data     �ļ�����ָ��
filelen          �ļ����ݳ���
*/
void my_freeFileData(void* data, int32 filelen)
{
	/*mrc_malloc�����ķ��ص�ָ�룬ǰһ��int32�������ڴ�ĳ���*/
	int32 len = *((int32*)data -1);
	/*���data�������Ƿ���mrc_malloc���������*/
	if (len == (filelen + sizeof(int32)))
	{
		my_free(data);
	}
	else
	{
		mrc_freeFileData(data,filelen);
		/*���ԣ��ͷ�my_readFileFromMrp�������ڴ��ǲ����ܵ���������*/
		MyAssert(0);	
	}
}