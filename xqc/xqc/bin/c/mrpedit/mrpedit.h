//由雪枫PaPo移植封装
//注:本头文件是mrpedit的雏形，注意是mrpedit，不是打包用的。
	char filename[24];  //文件名 24
	char appname[48];   //应用名 48
	char code[34];      //序列号 34
	char appidstr[22];  //APPID字串形式 22
	char appverstr[22]; //应用版本字串 22
	char vender[80];    //作者 80
	char descrip[128];  //应用描述 128
//读取mrp信息
void ReadMrpInfo(char*mrpname)
{
 int32 mrpHandel;
 int32 appid, appver;
 char*buffer=malloc(128);
	mrpHandel = open(mrpname, 1);
//	if(!mrpHandel) return;
	//1.从开始位置把指针移到偏移28，读取mrp显示名，22字节(假设全为中文)
	seek(mrpHandel, 28, SEEK_SET);
read(mrpHandel,filename, 22);
printf("显示名:%s",filename);

	//2.从开始位置把指针移到偏移16，读取mrp内部名，12字节（假设全为英文）
	seek(mrpHandel, 16, SEEK_SET);
	read(mrpHandel, appname, 12);
	printf("\n内部名:%s",appname);
//3.从开始位置把指针移到偏移52，读取mrp序列号，16字节（假设全文英文）
	seek(mrpHandel, 52, SEEK_SET);
	read(mrpHandel, code, 16);
	printf("\n序列号:%s",code);

	//4.读取APPID
	seek(mrpHandel, 68, SEEK_SET);
	read(mrpHandel, &appid, 4);
	printf("\nAPPID:%d",appid);
sprintf(appidstr,"%d",appid);
	//5.读取应用版本
	seek(mrpHandel, 72, SEEK_SET);
	read(mrpHandel,& appver, 4);
	printf("\n应用版本:%d",appver);
sprintf(appverstr,"%d",appver);
 
	//6.从开始位置把指针移到偏移88，读取mrp作者，38字节（假设全为中文）
	seek(mrpHandel, 88, SEEK_SET);
//	memset(mrp.vender, 0, sizeof(mrp.vender));
	read(mrpHandel,vender, 38);
	printf("\n作者:%s",vender);

	//7.从开始位置把指针移到偏移128，读取mrp应用说明，60字节
	seek(mrpHandel, 128, SEEK_SET);
//	memset(mrp.descrip, 0, sizeof(mrp.descrip));
	read(mrpHandel,descrip, 60);
	printf("\n应用说明:%s",descrip);

	close(mrpHandel); //读取完毕，关闭文件
}
int WriteMrpInfo(char*mrpname)
{
int32 mrpHandel = 0, ret  = 0;
//待写入MRP文件句柄
//char* buffer=malloc(128);//待写入缓冲区数据
	int appid, appver;
 //打开mrp文件
	mrpHandel = open(mrpname, 4);
	if(!mrpHandel) return -1;
	//1.从开始位置把指针移到偏移28，写入mrp显示名，22字节
	seek(mrpHandel, 28, SEEK_SET);
write(mrpHandel, filename, 22);
//2.从开始位置把指针移到偏移16，读取mrp内部名，11字节
 seek(mrpHandel, 16, SEEK_SET);
write(mrpHandel, appname, 11);
	//3.从开始位置把指针移到偏移52，读取mrp序列号，16字节
seek(mrpHandel, 52, SEEK_SET);
write(mrpHandel, code, 16);
	//4.写APPID(有两处一是68，一是192)
seek(mrpHandel, 68, SEEK_SET);
write(mrpHandel, appidstr, 4);
	//5.写应用版本(两处 偏移 72, 196)
seek(mrpHandel, 72, SEEK_SET);
write(mrpHandel, appverstr, 4);
//6.从开始位置把指针移到偏移88，读取mrp作者，38字节
seek(mrpHandel, 88, SEEK_SET);
write(mrpHandel, vender, 38);
	//7.从开始位置把指针移到偏移128，读取mrp应用说明，60字节

seek(mrpHandel, 128, SEEK_SET);
write(mrpHandel, descrip, 60);

close(mrpHandel);
return 0;
}

typedef struct _mrp_filelist
{
	char* filename;	//文件名
	uint8 type;		//类型
	int32 flen;		//文件长度
	int32 offset;	//文件偏移
	uint16 index;	//文件序号
	struct _mrp_filelist *next;
	struct _mrp_filelist *prev;
}*PMRPFILESINFO;
struct _mrp_filelist MRPFILESINFO;
int32	count;					//文件列表数
 PMRPFILESINFO	pHead, pCur;	//文件列表链表
#define L			sizeof(MRPFILESINFO)

//读取mrp文件列表
int ReadMrpFiles(char*MrpN)
{
	int32 fd = 0;  
	int32 flStar = 0, flEnd = 0; //MRP 文件列表起、止位置
	int32 fnLen = 0, fLen;		//mrp 内文件名长度,文件长度
	int32 fCount = 0;			//mrp 内文件数目
	char mrphead[5];		//验证mrp文件头
	char fName[64];		//文件名
	PMRPFILESINFO p = NULL;
	char*ntemp;
	int32 off;

	fd = open(MrpN, 4);
	if(!fd) return -1;

	//先判断是否为正确的MRP格式
	seek(fd, 0, SEEK_SET);
	read(fd, mrphead, 4);
	if( strncmp(mrphead, "MRPG", 4) != 0)
	{
		close(fd);
		return -1;
	}

	//读取文件列表终点位置
	read(fd, &flEnd, 4);
	flEnd += 8;
	//读取文件列表起始位置
	seek(fd, 12, SEEK_SET);
	read(fd, &flStar, 4);
	
	//创建文件链表头结点
		p = pCur = pHead =malloc(L);

	memset(pHead, 0, L);
	pHead->prev = NULL;
	pHead->next = NULL;
	fCount = 0;
	while(flStar < flEnd)
	{
		//1.读取文件名
		seek(fd, flStar, SEEK_SET);
		read(fd, &fnLen, 4);			//获取文件名长度
		read(fd, fName, fnLen);			//读取文件名
		ntemp = malloc(fnLen+1);
		memset(ntemp, 0, fnLen+1);
		memcpy(ntemp, fName, fnLen);
		p->filename = ntemp;
	//	p->type = GetFileFormat(fName);  //获取文件格式

		//2.读取文件长度、偏移
		read(fd, &off, 4);
		read(fd, &fLen, 4);
		p->flen = fLen;
		p->offset = off;
		p->index = fCount;	//序号从0开始
		fCount++;			//注意总数其实是
 printf("\n%d %s len:%d %d ",fCount,ntemp,fLen,off);

		//3.准备读取下一个文件
		flStar = flStar + fnLen + 16; //查找下个文件
		fnLen = 0;
		if(flStar < flEnd)	//这里就要检查有没有到末尾
		{
			PMRPFILESINFO tmp = p;

			p->next = malloc(L);
			p = p->next;
			memset(p, 0, L);
			p->prev = tmp;
			p->next = NULL;
		}
	 }
	//读取完毕记录总数
	close(fd);
	count = fCount;		//记录文件数

	return 0;
}

//焦点转移到链表
void Focus2List(int32 curIndex)
{
	if(pCur == NULL) pCur = pHead;
	if(pCur->index < curIndex){			//前移
		while(pCur->index != curIndex)
			pCur = pCur->next;
	}else if(pCur->index > curIndex){	//后移
		while(pCur->index != curIndex)
			pCur = pCur->prev;
	}else
		pCur = pCur;
	
	sprintf(LabelCaption, "%d/%d  Offset=%d, Length=%d", curIndex+1, count, pCur->offset, pCur->flen);
}
//从链表中删除一项
void Dele(int32 index)
{
	int i;
	PMRPFILESINFO p1, p2;

	//1.删除链表首
	if(index == 0)	
	{
		p1 = pHead;
		pCur = pHead = pHead->next;
		pHead->prev = NULL;
		free(p1->filename);
		free(p1);
		p1 = pHead;
		i = 0;
		while(p1){
			p1->index = i++;
			p1 = p1->next;
		}
		return;
	}

	//2.删除中间项或尾项
	p1 = pHead;
	for(i=0; i<index-1; i++)
		p1 = p1->next;			//找到删除节点的前一项
	if(p1->next->next == NULL)	//说明该项为链表尾
	{
		p2 = p1->next;
		p1->next = NULL;
		pCur = p1;
	}else	//中间节点
	{
		p2 = p1->next;
		pCur = p1->next = p2->next;
		p1->next->prev = p1;
		i = p1->index;
		while(p1){
			p1->index = i++;
			p1 = p1->next;
		}
	}
	free(p2->filename);
	free(p2);
}

//获取列表项回调
void AddFileToList1(int index)
{
	PMRPFILESINFO ptmp = pCur;

	if (!pRowData || index<0 || index > count - 1) //如果出错跳出
		return;

	//当前获得焦点的链表
	if(ptmp == NULL) ptmp = pHead;

	//获取当前焦点
	if(ptmp->index < index){			//前移
		while(ptmp->index != index)
			ptmp = ptmp->next;
	}else if(ptmp->index > index){	//后移
		while(ptmp->index != index)
			ptmp = ptmp->prev;
	}else
		ptmp = ptmp;

}


//释放文件数据内存
void FreeFileData(void)
{
	PMRPFILESINFO p1, p2 ;
	p1 = pHead;
	while(1)
	{
	if(p1!=0)
	{
	 printf("\n释放数据:%s",p1->filename);
		free(p1->filename);
		p2 = p1->next;
 	free(p1);
		p1 = p2;}
		else
		{
		break;		
		}
	}
	pCur = pHead = NULL;
	count = 0;
}

uint8 *buf;
int32 len,f;

//mkdir("c/例程");
void unpack(char*mrp,char*in,char*out)
{
    buf=NULL;
    readfilefrommrp(mrp,in,&buf,&len,0);
    f=open(out,12);
    write(f,buf,len);
    close(f);
    free(buf);
}

int upMrpFiles(char*name,char*path, int isUnZip)
{
int imrp;
char buf[100];
ReadMrpFiles(name);
PMRPFILESINFO p1, p2 ;
p1 = pHead;
if(filestate(path)!=2)
{
mkdir(path);}
while(imrp!=count)
{
  sprintf(buf,"%s%s",path,p1->filename);
	unpack(name,p1->filename,buf);
printf("\n%s",buf);
		p2 = p1->next;
		p1 = p2;

++imrp;
}
return 0;}

//解包MRP文件
int UnPackFileFromMrp(PMRPFILESINFO p,char*Mname, char*tofile)
{


	int32 mrphd, filehd;
	int32 off, r;
	char*buf;
	int32 buflen = 10*1024;
	int32 total;

//if(!p || !tofile || p->flen < 0) return -1;

	//打开源文件和目标文件
	mrphd = open(Mname, 4);
	if(!mrphd) return -1;
	filehd = open(tofile, 4|8);
	if(!filehd){
		close(mrphd);
		return -1;
	}

	//分配缓冲区
	buf = malloc(buflen);
/*	if(!buf){
		close(mrphd);
		close(filehd);
		return -1;
	}*/

	//开始读取文件
	off = p->offset;	//文件开始位置
	seek(mrphd, off, SEEK_SET);
	seek(filehd, 0, SEEK_SET);
	total = p->flen;	//总共要读取的长度
	while(total>0)
	{
	   memset(buf,0,buflen);
		r= read(mrphd, buf,(buflen<=total)?buflen:total);
		write(filehd, buf, r);
		total -= r;
			printf("\n%s还有:%d",tofile,total);

	}
	free(buf);
	//读写完毕关闭文件
	close(mrphd);
	close(filehd);

	return 0;
}

//解包MRP文件
int ExtractMrpFiles(PMRPFILESINFO p,char*Mname,char*path, int isUnZip)
{
	char*filebuf = NULL;  //解压文件缓冲
	int32 filelen = 0;		//解压文件长度
	int32 fd = 0;
	char name[128];	//待解压文件名

	//验证数据正确性
	//if(!p) return FAILED;
	//检查目录
	if(2!= filestate(path)) mkdir(path);

	//解压文件名
	sprintf(name, "%s%s", path, p->filename);

	//解压/解包文件
	if(isUnZip)	//解压文件
	{
	//解压文件名
	sprintf(name, "%s%s", path, p->filename);

		if(0 == readfilefrommrp(Mname, p->filename, &filebuf, &filelen, 0))
		{
			fd = open(name,4|8);
			if(!fd){
				free(filebuf);
				return -1;
			}
			seek(fd, 0, SEEK_SET);
			write(fd, filebuf, filelen);
			close(fd);
			free(filebuf);

			return 0;
		}else
			return -1;
	}else	//解包文件
	{
		//解压文件名
	sprintf(name, "%s%s", path, p->filename);

		return UnPackFileFromMrp(p,Mname ,name);
	}



	return 0;
}


//解包/解压全部
void ExtractAll(char*name,char*path,int UnZip)
{
	int scount = 0;
	char buf[100];
	char*uni;
	ReadMrpFiles(name);
	PMRPFILESINFO p = pHead;

	while(scount!=count)
	{

			ExtractMrpFiles(p,name,path, UnZip);
				scount++;
		p = p->next;
	}

}

#define FULLNAME_LEN	200		//完整文件名长度
#define HEADBUF_LEN		240		//mrp头部长度
#define WRITEBUF_LEN	20*1024 //文件读写缓冲长度
//打包MRP实现
int PackagedMrp(PMRPFILESINFO p,char*Mname,char*WorkDir,char*tofile)
{
	PMRPFILESINFO p1;		//链表
	int32 tomrpfd;			//保存到的新MRP的文件句柄
	int32 delmrpfd;			//当前处理的MRP文件句柄
	int32 tempfd;			//正在写入的文件句柄
	int32 flistlen;			//mrp内部文件列表部分长度
	int32 len, templ;		//获取一个长度
	uint8 buf[100];			//对话框内容缓冲
	char fullname[FULLNAME_LEN];			//完整文件名
	char mrphead[HEADBUF_LEN];				//mrp文件头部缓冲
	char*pwbuf;				//写文件的缓冲区
	char*pflistbuf;			//将文件列表部分数据读取到内存方便修改列表部分

	//2.打开源文件和目标文件
	if(filestate(tofile) == 1)
		remove(tofile);
	delmrpfd = open(Mname,4);
	tomrpfd = open(tofile, 4|8);
	if(delmrpfd == 0 || tomrpfd == 0)
		goto ERROR;		//失败点一打开文件失败

	//3.写头部

	seek(delmrpfd, 0, SEEK_SET);
	read(delmrpfd, mrphead, 240);
	seek(tomrpfd, 0, SEEK_SET);
	write(tomrpfd, mrphead, 240);

	//3.写文件列表 正在写文件列表。。。


	int32 wklen = strlen(WorkDir);

	//计算整个文件列表的长度
	p1 = pHead;
	if(p1!=NULL)
	{
		char buf2[13] ;	//用来填充空白
		char*tempn;
		flistlen = 0;
		memset(fullname, 0, FULLNAME_LEN);
		memcpy(fullname, WorkDir, wklen);
		while(p1!=NULL)
		{
		
			memset(&fullname[wklen], 0, FULLNAME_LEN - wklen - 2); //-2防越界
			len = strlen(p1->filename);		//文件名长
			memcpy(&fullname[wklen], p1->filename, len);

			//计算列表长度同时要考虑文件是否存在
			if(filestate(fullname)==1&&len<FULLNAME_LEN)
			{
		//			printf("\n+++++");

				templ = len + 1;
				write(tomrpfd, &templ, 4);  //写文件名长
				tempn = p1->filename;
				write(tomrpfd, tempn, len);  //写文件名
				write(tomrpfd, buf2, 13); 
				flistlen += len + 17;
			}
			p1 = p1->next;
		}
	}else
		goto ERROR;	

	//4.写文件 正在写入文件。。。


	p1 = pHead;
	if(p1!=NULL)
	{
		char*pt1;
		int32 r = 0, l = 0, curpos = 0;//当前文件读写指针
	//	int32 wklen = strlen(WorkDir);	//WorkDir长度

		pwbuf =malloc(WRITEBUF_LEN);	//分配读写缓冲
		memset(pwbuf, 0, WRITEBUF_LEN);
		
		pflistbuf = malloc(flistlen);	//申请一片内存来保存文件列表部分数据
		seek(tomrpfd, 240, SEEK_SET);
		read(tomrpfd, pflistbuf, flistlen);		//文件指针又到了末尾哦
		pt1 = pflistbuf;
		curpos = 240 + flistlen;	//记录当前读写位置

		memset(fullname, 0, FULLNAME_LEN);
		memcpy(fullname, WorkDir, wklen);
		while(p1!=NULL)
		{
			memset(&fullname[wklen], 0, FULLNAME_LEN - wklen - 2); //-2防越界
			len = strlen(p1->filename);		//文件名长
			memcpy(&fullname[wklen], p1->filename, len);
			printf("\n打包文件: %s",fullname);  
		if(filestate(fullname) == 1)
{
				memcpy(&l, pt1, 4);			//从缓冲区获取文件名长度保存到i
				write(tomrpfd, pt1, l+4);	//写文件名 tomrpfd
				curpos += (4+l);				//记录文件指针
				pt1 += 4;		//缓冲区偏移4字节到文件名处
				pt1 += l;		//缓冲区偏移加上文件名长度,待写入文件数据长度

				len = getlen(fullname);		//获取文件字节数
				write(tomrpfd, &len, 4);	//写入文件字节数 tomrpfd
				curpos += 4;					//记录文件指针

				memcpy(pt1, &curpos, 4);	//将文件偏移记录到文件列表区
				pt1 += 4;						//缓冲区偏移加4字节，待写入文件数据长度
				memcpy(pt1, &len, 4);		//将文件长度记录到文件列表区
				pt1 += 8;						//缓冲区偏移加8字节，数据长度和4个空字节

				tempfd = open(fullname,4);	//OK可以写文件数据了，先打开文件
				if(tempfd == 0)	//打开文件失败
				{
					free(pflistbuf);
					free(pwbuf);
					goto ERROR;
				}
				r= read(tempfd, pwbuf, WRITEBUF_LEN);	//循环读写文件
				while(r)
				{
		write(tomrpfd, pwbuf, r);	//tomrpfd
					curpos += r;
					memset(pwbuf,0,WRITEBUF_LEN);
	r = read(tempfd, pwbuf,WRITEBUF_LEN);
				}
				close(tempfd);
				tempfd = 0;
   	}//if(
			
			p1 = p1->next;	//链表移到下一个文件
		}//while(p1)

		//文件都写完了，做最后的工作
		//1.更新文件列表数据
		seek(tomrpfd, 240, SEEK_SET);
		write(tomrpfd, pflistbuf, flistlen);

		//2.更新头部数据
		seek(tomrpfd, 4, SEEK_SET);	//4-8MRP数据开始位置+8
		l = 240 + flistlen + 8;
		write(tomrpfd, &l, 4);
		seek(tomrpfd, 8, SEEK_SET);	//8-12mrp文件总字节数
		write(tomrpfd, &curpos, 4);

		//3.释放一切数据
		free(pflistbuf);
		free(pwbuf);
	}else
		goto ERROR;	

	//5.打包完成，善后
	free(pwbuf);

	//关闭文件句柄
	close(delmrpfd);
	close(tomrpfd);
	
	return 0;

ERROR:	//错误跳出
	{
		close(delmrpfd);
		close(tomrpfd);
		return -1;
	}
}
// CopyFile 文件复制函数
//srcfile 源文件,dest目标文件
int CopyFile(char*srcfile,char*destfile)
{
	char*buffer;     //文件读写缓冲
	int32 fd_dest_file = 0, fd_src_file = 0; //目标文件和源文件句柄
	int32 ret, size, r, w=0;
 buffer=malloc(20*1024);	//分配缓冲区内存

	//获取源文件大小
	size =getlen(srcfile);	

	//打开源文件(只读)
	fd_src_file =open(srcfile, 1); 

	//打开目标文件(不存在创建)
	fd_dest_file = open(destfile,4|8); 
	
	if(fd_src_file != 0 && fd_dest_file != 0) //打开文件都成功
	{
	
		r = read(fd_src_file, buffer,20*1024);   //读取文件

		while(r != 0)//如果读取的字节数不为0,则进行写文件
		{
			w += write(fd_dest_file, buffer, r);

			r = read(fd_src_file, buffer,r);
		}

		ret = close(fd_src_file); //关闭源文件
		ret = close(fd_dest_file); //关闭目标文件  
		free(buffer);

		//通知：成功完成一个文件
printf("\n复制:%s->%s成功",srcfile,destfile);
		return 0;  //复制成功
	}else  //执行这里说明源/目标文件打开失败
	{ 	
		//这里不确定哪个文件打开失败，所以先判断下
		if(fd_dest_file != 0) 
			ret = close(fd_dest_file); //关闭目标文件  
		else 

		if(fd_src_file != 0) 
			ret = close(fd_src_file); //关闭源文件
		else

		free(buffer);

		return -1;     
	}
}

//替换/插入文件处理
int ChangeFile(char*Cname,char*path,int32 index,int mode)
	{
		//获取文件返回,在此处完成文件替换或插入
		if(pHead!=0)
		{
	 int32 i,c;
	 char*name, fullname;
	 char buf[128];	//存放加入到temp的文件名
	char buf2[128];	//MRP包内文件对应的临时文件名
			PMRPFILESINFO p = pHead,pc=pHead ,pc1,p2;

			//1.获取文件名和完整文件名
//磁盘中文件路径
		fullname = Cname;	//获取文件路径名
			if(!fullname) break;			
			while(1)
			{
			if(c==index)
			{
   name=pc->filename;	
 			break;
			}
		pc1 = pc->next;
		pc = pc1;
			c++;
			}
		//	name = ExtractFileName(fullname);	//获取文件名
			if(!name) break;

			//2.将加入的文件复制到工作目录(如果存在同名文件呢？)
			sprintf(buf, "%s%s",path, name);
			if(1== filestate(buf)) remove(buf);	//已存在则删除之
			CopyFile(fullname, buf, NULL);	//将文件复制到工作目录

			//3.处理链表
	if(mode == 0)	//替换模式，替换应该是文件名相同哦
			{
		sprintf(buf2, "%s%s", path, p->filename);
				remove(buf2);
		rename(buf, buf2);	//再将加入的文件改名OK
		//		free(name);			//释放短文件名
			}else		//插入模式
			{
				PMRPFILESINFO tmp;

				//分配文件结构内存
				tmp = malloc(L);
				memset(tmp, 0, L);
	 tmp->filename = name;//直接使用查找短文件名分配的内存
				tmp->flen = -1;
				tmp->offset = -1;

				if(pCur->index !=index) 
					Focus2List(index);

				if(pCur->index == 0)
				{	//头插
					pHead->prev = tmp;
					pHead = tmp;
					tmp->next = pCur;
					tmp->prev = NULL;
					i = 0;				//重定序号
					while(tmp)
					{
						tmp->index = i++;
						tmp = tmp->next;
					}
				}
				else
				{
				//中间插插
					p2 = pCur->prev;
					pCur->prev = tmp;
					p2->next = tmp;
					tmp->next = pCur;
					tmp->prev = p2;
					i = tmp->index = pCur->index;
					while(tmp)
					{
						tmp->index = i++;
						tmp = tmp->next;
					}
				}
				count += 1;			//文件总数加1
			}
 }
}