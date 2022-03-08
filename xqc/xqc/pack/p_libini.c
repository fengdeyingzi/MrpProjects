#include "libini.h"

static void list_append(INI *pini,
		char *key,
		char *value,
		char *section
		)
{
	ini_item *item = (ini_item *)malloc(sizeof(ini_item));
	
	item->section = section;
	item->key = key;
	item->value = value;
	item->next = NULL;
	if (!pini->tailer) {
		pini->header = pini->tailer = item;
	} else {
		pini->tailer->next = item;
		pini->tailer = item;
	}
	pini->length += 1;
	
	return;
}

static ini_item *list_search(INI *pini,const char *key)
{
	ini_item *p = pini->header;
	char temp[VCHR_LEN];
	char *k = temp, *sect=temp;
	ini_item *_ret = NULL;
	
	if (!p) return NULL;
	strcpy(temp,key);
	for (; *k && SECT_DOT!=*k; k++);
	if (SECT_DOT==*k && k==sect)
		sect = ++k;
	else if (SECT_DOT == *k)
		*k++ = 0;
	else
		k = sect;
	
	if (k!=sect && (unsigned int)(k-sect)==strlen(key)) {
		while (p) {
			if (0 != strcmp(p->section,sect)) {
				p = p->next;
				continue;
			}
			if (!p->next)
				return p;
			if (0 != strcmp(p->next->section,sect))
				return p;
			p = p->next;
		}
		return _ret;
	}
	
	while (p) {
		if (0 != strcmp(p->key,k)) {
			p = p->next;
			continue;
		}
		if (k!=sect && 0!=strcmp(p->section,sect)) {
			p = p->next;
			continue;
		}
		if (k==sect && 0!=strcmp(p->section,
				pini->header->section)) {
			p = p->next;
			continue;
		}
		_ret = p;
		break;
	}
	
	return _ret;
}

int ini_readline(int32 f, char out[LINE_LEN])
{
	int i=0,tmp=0;

	do{
		if(1>mrc_read(f,&tmp,1))
			goto AA;
	}while(tmp=='\n' || tmp=='\r');
	do{
		out[i]=tmp;
		i++;
		if(1>mrc_read(f,&tmp,1))
		{
AA:         out[i]=0;
			return -1;
		}
		if(tmp=='\n' || tmp=='\r')
			break;
	}while(i<255 && tmp);
	if(i==255) mrc_seek(f,-1,MR_SEEK_CUR);
	return out[i]=0;
}

INI *ini_open(const char *fname,const char *DEF_SECT_ROOT)
{
	INI *pini;
	int32 fp = mrc_open(fname,1);
	char buffer[LINE_LEN];
	char *p;
	char *s;
	char sect[SECT_LEN] = DEF_SECT_ROOT;
	char *section;
	char *key;
	char *value;
	
	if (!fp) return NULL;
	pini = (INI *)malloc(sizeof(INI));
	pini->fname = (char *)malloc(strlen(fname)+1);
	strcpy(pini->fname,fname);
	pini->length = 0;
	pini->header = pini->tailer = NULL;
	
	while (!ini_readline(fp,buffer)) {
		for (p=buffer; ' '==*p||'\t'==*p; p++);
		if ('#'==*p || '\n'==*p || '\r'==*p) {
			continue;
		} else if ('[' == *p) {
			for (p++; ' '==*p||'\t'==*p; p++);
			for (s=p; ' '!=*p&&'\t'!=*p&&']'!=*p; p++);
			*p = 0;
			strcpy(sect,s);
		} else {
			for (; ' '==*p||'\t'==*p; p++);
			for (s=p; ' '!=*p&&'\t'!=*p&&'='!=*p&&':'!=*p; p++);
			*p = 0;
			key = (char *)malloc(strlen(s)+1);
			strcpy(key,s);
			
			for (p++; ' '==*p||'\t'==*p||'='==*p||':'==*p; p++);
			for (s=p; '#'!=*p&&';'!=*p&&'\n'!=*p&&'\r'!=*p; p++);
			*p = 0;
			for (p=s+strlen(s)-1; ' '==*p; *p=0,p--);
			value = (char *)malloc(strlen(s)+1);
			strcpy(value,s);
			
			section = (char *)malloc(strlen(sect)+1);
			strcpy(section,sect);
			
			list_append(pini,key,value,section);
		}
	}
	mrc_close(fp);
	
	return pini;
}

INI *ini_create(const char *fname)
{
	int32 fp = mrc_open(fname,12);
	INI *pini;
	
	if (!fp) return NULL;
	
	pini = (INI *)malloc(sizeof(INI));
	pini->fname = (char *)malloc(strlen(fname));
	strcpy(pini->fname,fname);
	pini->length = 0;
	pini->header = pini->tailer = NULL;
	
	mrc_close(fp);
	
	return pini;
}
int32 ini_puts(int32 f,char *text)
{
	return mrc_read(f,text,mrc_strlen(text));
}
int ini_save(INI *pini)
{
	int32 fp;
	ini_item *p;
	char *sect = NULL;
	char buffer[LINE_LEN];
	
	p = pini->header;
	if (!p) return 0;
	fp = mrc_open(pini->fname,12);
	if (!fp) return 0;
	
	while (p) {
		if (!sect || 0!=
		strcmp(p->section,sect)) {
			sect = p->section;
			buffer[0] = '[';
			strcpy(buffer+1,sect);
			strcat(buffer,"]\n");
			ini_puts(fp,buffer);
		}
		strcpy(buffer,p->key);
		strcat(buffer,"\t= ");
		strcat(buffer,p->value);
		strcat(buffer,"\n");
		ini_puts(fp,buffer);
		
		p = p->next;
	}
	
	mrc_close(fp);
	
	return 1;
}

void ini_close(INI *pini)
{
	ini_item *p = pini->header;
	ini_item *temp;
	
	if (!p) return;
	while (p) {
		free(p->key);
		free(p->value);
		free(p->section);
		temp = p;
		p = p->next;
		free(temp);
	}
	
	free(pini->fname);
	free(pini);
	
	return;
}

char *ini_get(INI *pini,const char *key,char *value)
{
	ini_item *item;
	
	item = list_search(pini,key);
	*value = 0;
	if (item) strcpy(value,item->value);
	
	return value;
}

int ini_get_int(INI *pini,const char *key)
{
	char value[VINT_LEN];
	return atoi(ini_get(pini,key,value));
}

int ini_set(INI *pini,const char *key,const char *value)
{
	ini_item *item;
	char *temp;
	
	item = list_search(pini,key);
	if (!item) return 0;
	temp = item->value;
	item->value = (char *)malloc(strlen(value)+1);
	strcpy(item->value,value);
	free(temp);
	
	return 1;
}

int ini_set_int(INI *pini,const char *key,int value)
{
	char buffer[VINT_LEN];
	sprintf(buffer,"%d\0",value);
	return ini_set(pini,key,buffer);
}

int ini_append(INI *pini,const char *key,const char *value)
{
	ini_item *item, *sear, *temp;
	char buffer[LINE_LEN];
	char *k, *v, *s, *p;
	
	strcpy(buffer,key);
	if (NULL == (p=strchr(buffer,SECT_DOT))) {
		if (pini->header)
			strcpy(buffer,pini->header->section);
		else	
			strcpy(buffer,DEF_SECT_ROOT);
		strcat(buffer,".");
		strcat(buffer,key);
	} else if (p == buffer) {
		strcpy(buffer,pini->header->section);
		strcat(buffer,key);
	} else if ((unsigned int)(p+1-buffer) == strlen(buffer))
		return 0;
	p = strchr(buffer,SECT_DOT);
	sear = list_search(pini,buffer);
	if (sear && 0==strcmp(sear->key,p+1))
		return 0;
	
	item = (ini_item *)malloc(sizeof(ini_item));
	k = (char *)malloc(strlen(key)+1);
	strcpy(k,p+1);
	item->key = k;
	v = (char *)malloc(strlen(value)+1);
	strcpy(v,value);
	item->value = v;
	
	*(p+1) = 0;
	sear = list_search(pini,buffer);
	if (sear) {
		s = (char *)malloc(strlen(sear->section)+1);
		strcpy(s,sear->section);
		temp = sear->next;
		sear->next = item;
		item->next = temp;
	} else {
		*p = 0;
		s = (char *)malloc(strlen(buffer)+1);
		strcpy(s,buffer);
		item->next = NULL;
		if (pini->header) {
			pini->tailer->next = item;
			pini->tailer = item;
		} else
			pini->header = pini->tailer = item;
	}
	item->section = s;
	pini->length += 1;
	
	return 1;
}

int ini_append_int(INI *pini,const char *key,int value)
{
	char buffer[VINT_LEN];
	sprintf(buffer,"%d\0",value);
	return ini_append(pini,key,buffer);
}

int ini_remove(INI *pini,const char *key)
{
	ini_item *item, *temp, *p;
	
	item = list_search(pini,key);
	if (!item) return 0;
	p = pini->header;
	if (p && !p->next) {
		if (p != item) return 0;
		free(p->section);
		free(p->key);
		free(p->value);
		free(item);
		pini->header = pini->tailer = NULL;
		pini->length -= 1;
		return 1;
	}
	while (p && p->next) {
		if (p->next == item)
			break;
		p = p->next;
	}
	if (!p || !p->next) return 0;
	temp = p->next;
	p->next = temp->next;
	free(temp->section);
	free(temp->key);
	free(temp->value);
	free(temp);
	pini->length -= 1;
	
	return 1;
}
