
typedef struct
{uint16 ge;uint16 *f;char *dt;}res;

res *lang0,*lang1,*lang1000;

void *loadlang(int nn,uint16 ge)
{res *a=malloc(sizeof(res));
a->ge=ge;a->f=NULL;uint32 len;
char *name=malloc(128);
sprintf(name,"res_lang%d.rc",nn);
a->f=filenei(name,&len);
free(name);
a->dt=(char *)(a->f+ge+1);
return a;
}
char *lang(uint8 i,int nn)
{res *a=!i?lang0:(i==1?lang1:(i==2?lang1000:NULL));
if(a==NULL){return NULL;}
uint16 *l=a->f;
char *dt=a->dt;
return dt==NULL?NULL:(char *)dt+l[nn];
}

lang0=loadlang(0,28);
lang1=loadlang(1,28);
lang1000=loadlang(1000,28);