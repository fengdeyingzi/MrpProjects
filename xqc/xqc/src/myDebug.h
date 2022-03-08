#ifndef MY_DEBUG_H
#define MY_DEBUG_H


//�ڴ��ʼ������ ��ʹ�����к���ǰ������.
void mem_init(void); 
 

//�ڴ�������ͷź��� 
// __FILE__����my_malloc���������ڵ��ļ�. 
// __LINE__����my_malloc���������ڵ��ļ��к�. 
// ��ֱ��ʹ��my_malloc����.��Ҫ������������װmy_malloc����,�����ᵼ��__FILE__��__LINE__��ʵ������. 
#define my_malloc(n) my_mallocEx(n,__FILE__,__LINE__) 
void my_free(void *address); 


//�ڴ�й¶�������⺯�� 
// ������������⺯���ŵ�mrc_exitApp������,�������˳������ʱ�������ڴ���. 
// ����Ҳ���Էŵ��κ�ֵ�û��ɵĵط������ڴ�ļ��. 
void mem_leak_check(void); 
void mem_overflow_check(void); 


//mythroad�ڲ������ڴ�ĺ��� 
// ʹ��ʱ��mrc_xxx ��Ϊmy_xxx���ɣ����ܺ�ԭ������ͬ��
// ֻ�ʺ���ģ������ʹ�ã����ֻ�����ı�Դ����С� 
// ������Ĵ���Ҫ��ģ�����ϵ��ԵĴ���һ�£�����궨�������ȫһ������Щ��ᵼ�²�ͬ�ı���Ч����
int32 my_readFileFromMrpEx(char* packname, const char* filename, uint8 ** filebuf, int32 *filelen, int32 lookfor); 
void* my_readAll(const char* filename, uint32 *len); 
uint16* my_c2u(char *cp, int32 *err, int32 *size); 
void * my_readFileFromMrp(const char* filename, int32 *filelen, int32 lookfor); 
void my_freeFileData(void* data, int32 filelen); 




void* my_mallocEx(int size, char* filename, int line);

#endif