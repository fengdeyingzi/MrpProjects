
#include "mrc_mape.h"
#include "mrc_tpng.h"

//打印mape信息
int printMape(_MAPE *mape)
{
  int i = 0;
  if (mape != NULL)
  {
    mrc_printf("\n**********MAPE INFO****\n");
    mrc_printf("*%s\n", mape->dir);
    mrc_printf("*path:%s\n", mape->path);
    mrc_printf("*name:%s\n", mape->name);
    mrc_printf("*bkg :%x\n", mape->bkg);
    mrc_printf("*map :(%d, %d)\n",
               mape->mapw, mape->maph);
    mrc_printf("*item:(%d, %d)\n",
               mape->itemw, mape->itemh);
    mrc_printf("**********MAPE CENG****\n");
    mrc_printf("*laye:%s\n", mape->layname);
    mrc_printf("*name:%s\n", mape->resname);
    mrc_printf("*type:%s\n", mape->restype);
    // mrc_printf("*resnum:%d\n", mape->resnum);
    for (i = 0; i < mape->resnum; i++)
    {
      mrc_printf("*res[%d]:%s\n", i, mape->resarr[i]);
    }
    mrc_printf("*data:%d\n", mape->mapw * mape->maph * 4);
    mrc_printf("*end :%d\n", mape->endlen);
    mrc_printf("**********MAPE END ****\n");
  }
  return 0;
}

//加载mape文件
_MAPE *loadMape(char *name)
{
  int32 pos = 0, i = 0, dirlen;
  //读取路径名长
  int16 pathlen;
  int32 len;
  int32 file;
  _MAPE *mape;
  char *end;

  //判断文件是否存在
  if (mrc_fileState(name) != MR_IS_FILE)
  {
    return NULL;
  }

  //读取文件
  len = mrc_getLen(name);
  file = mrc_open(name, 1);

  mape = mrc_malloc(sizeof(_MAPE));

  //记录所在文件夹
  end = strrchr(name, '/');
  dirlen = mrc_strlen(name) - mrc_strlen(end) + 1;
  mape->dir = mrc_malloc(sizeof(char) * dirlen + 1);
  mrc_memset(mape->dir, '\0', dirlen + 1);
  mrc_strncpy(mape->dir, name, dirlen);

  //解析文件
  if (file)
  {
    pos += 1;
    pathlen = mrc_readFileInt16(file, pos);
    pos += 2;
    mrc_readFileMalloc(file, pos, pathlen, &mape->path);
    pos += pathlen;
    mrc_printf("path:%d->%s", pathlen, mape->path);

    //读取地图名
    pathlen = mrc_readFileInt16(file, pos);
    pos += 2;
    mrc_readFileMalloc(file, pos, pathlen, &mape->name);
    pos += pathlen;
    mrc_printf("name:%d->%s", pathlen, mape->name);

    //读取地图信息
    mape->bkg = mrc_readFileInt32(file, pos);
    pos += 4;
    mape->maph = mrc_readFileInt32(file, pos);
    pos += 4;
    mape->mapw = mrc_readFileInt32(file, pos);
    pos += 4;
    mape->itemw = mrc_readFileInt32(file, pos);
    pos += 4;
    mape->itemh = mrc_readFileInt32(file, pos);
    pos += 4;

    //读取层次名
    pos += 8;
    pathlen = mrc_readFileInt16(file, pos);
    pos += 2;
    mrc_readFileMalloc(file, pos, pathlen, &mape->layname);
    pos += pathlen;
    mrc_printf("layname:%d->%s", pathlen, mape->layname);

    //读取资源名
    pathlen = mrc_readFileInt16(file, pos);
    pos += 2;
    mrc_readFileMalloc(file, pos, pathlen, &mape->resname);
    pos += pathlen + 4;

    //读取资源类型
    pathlen = mrc_readFileInt16(file, pos);
    pos += 2;
    mrc_readFileMalloc(file, pos, pathlen, &mape->restype);
    pos += pathlen;

    //读取资源列表
    mape->resnum = mrc_readFileInt32(file, pos);
    pos += 4;
    mape->resarr = mrc_malloc(sizeof(char *) * mape->resnum);
    mape->resdata = mrc_malloc(sizeof(MRC_TPNG *) * mape->resnum);
    for (i = 0; i < mape->resnum; i++)
    {
      pathlen = mrc_readFileInt16(file, pos);
      pos += 2;
      mrc_readFileMalloc(file, pos, pathlen, &mape->resarr[i]);
      mape->resdata[i] = read_mrp_tpng(mape->resarr[i]);
      pos += pathlen + 8;
    }

    //过滤未知数据
    pos += 88 + 24 - 8;

    //读取数据
    pathlen = mape->mapw * mape->maph * 4;
    mrc_readFileMalloc(file, pos, pathlen, (char **)&mape->data);
    pos += pathlen;

    //保留以下数据
    mape->endlen = len - pos;
    mrc_readFileMalloc(file, pos, mape->endlen, (char **)&mape->end);
    //printMape(mape);
    mrc_close(file);
  }
  return mape;
}

//取资源图片数据
uint16 *getMapeRes(_MAPE *mape, int32 k, uint32 *w, uint32 *h)
{
  uint32 plen;
  int32 size;
  char *pname;
  uint16 *bmp = NULL;
  MRC_TPNG *png = NULL;

  if (k < mape->resnum)
  {
    png = mape->resdata[k];
    *w = png->width;
    *h = png->height;

    bmp = mrc_png2bmp_data(png);
    //mrc_printf("bmp%d->(%d, %d), png->%p, %p", k, *w, *h, png, png->data);
  }
  return bmp;
}

uint16 *getMapeResIndex(_MAPE *mape, int32 index, uint32 *w, uint32 *h, uint32 *knum)
{
  int32 i = 0;
  int32 heng, hang;
  uint16 *bmp = NULL;
  int32 *data = mape->data;
  uint8 ddata;

  //判断是否可以通过
  ddata = (data[index] >> 24 & 0xff);
  if ((data[index] & 0xFFFFFF) == 0xFFFFFF)
  {
    ddata = 0xff - (data[index] >> 24 & 0xff) + 1;
  }

  *knum = 0;
  for (i = 0; i < mape->resnum; i++)
  {
    bmp = getMapeRes(mape, i, w, h);
    heng = *w / mape->itemw;
    hang = *h / mape->itemh;
    if (ddata - *knum <= heng * hang)
      break;
    else
    {
      *knum += heng * hang;
      mrc_free(bmp);
      bmp = NULL;
    }
  }
  return bmp;
}

int drawMape(_MAPE *mape)
{
  drawMapePos(mape, 0, 0);
}

//绘制mape到屏幕
int drawMapePos(_MAPE *mape, int32 dx, int32 dy)
{
  int i, j, heng, hang;
  uint32 x, y, knum, w, h;
  uint8 ddata;

  int32 *data = mape->data;
  uint16 *bmp = NULL;
  int32 rop = BM_COPY;

  //由于一个地图有多个资源图片res，所以这里比较麻烦
  mrc_drawRect(0, 0, mape->mapw * mape->itemw, mape->maph * mape->itemh, mape->bkg >> 16 & 0xff, mape->bkg >> 8 & 0xff, mape->bkg & 0xff);
  //绘制图片
  for (i = 0; i < mape->maph; i++)
  {
    for (j = 0; j < mape->mapw; j++)
    {
      //判断是否可以通过
      ddata = (data[i * mape->mapw + j] >> 24 & 0xff);
      if ((data[i * mape->mapw + j] & 0xFFFFFF) == 0xFFFFFF)
      {
        ddata = 0xff - (data[i * mape->mapw + j] >> 24 & 0xff) + 1;
      }

      bmp = getMapeResIndex(mape, i * mape->mapw + j, &w, &h, &knum);
      if (bmp != NULL)
      {
        heng = w / mape->itemw;
        hang = h / mape->itemh;
        x = ((ddata - 1 - knum) % heng) * mape->itemw;
        y = ((ddata - 1 - knum) / heng) * mape->itemh;

        if ((data[i * mape->mapw + j] & 0xFFFFFF) == 0xFFFFFF)
          rop = BM_NOT;
        else
          rop = BM_COPY;

        if (x < w && y < h)
          mrc_bitmapShowEx(bmp, dx + j * mape->itemw, dy + i * mape->itemh, w, mape->itemw, mape->itemh, rop, x, y);
        else
        {
        }

        mrc_free(bmp);
      }
    }
  }
  return 0;
}

//保存png为地图文件
int pngToMape(_MAPE *mape, char *png)
{
  int32 bpng, i, j;
  mr_bitmapSt binfo;
  int32 plen = mrc_strlen(mape->dir) + mrc_strlen(png) + 1;
  char *pname = mrc_malloc(sizeof(char) * plen);

  mrc_memset(pname, '\0', plen);
  mrc_printf(pname, "%s%s", mape->dir, png);

  //读取图片
  bpng = mrc_bitmapLoad(4, pname, 0, 0, binfo.w, binfo.h, binfo.w); //240*225

  //bitmapGetInfo(bpng, &binfo);

  //修改地图资料
  mape->mapw = binfo.w / 16;
  mape->maph = binfo.h / 16;
  mape->itemw = 16; //binfo.width;
  mape->itemh = 16; //binfo.height;

  //修改资源数据
  for (i = 0; i < mape->resnum; i++)
    mrc_free(mape->resarr[i]);

  mape->resnum = 1;
  plen = mrc_strlen(png) + 1;
  mape->resarr[0] = mrc_malloc(sizeof(char) * plen);
  mrc_memset(mape->resarr[0], '\0', plen);
  mrc_printf(mape->resarr[0], "%s", png);

  //修改地图数据
  mrc_free(mape->data);
  mape->data = mrc_malloc(sizeof(int32) * mape->mapw * mape->maph);
  for (i = 0; i < binfo.h / 16; i++)
    for (j = 0; j < mape->maph; j++)
      mape->data[i * mape->mapw + j] = (i * binfo.w / 16 + j + 1) << 24 & 0xff000000;
  return 0;
}

//保存mape地图文件
int saveMape(_MAPE *mape, char *name)
{
  int32 file, pos = 0, i;

  mrc_remove(name);
  file = mrc_open(name, 4 | 8);
  if (file)
  {
    //写入路径
    mrc_writeInt8File(file, pos, 0);
    pos += 1;
    mrc_writeInt16File(file, pos, mrc_strlen(mape->path));
    pos += 2;
    mrc_writeDataFile(file, pos, mape->path, mrc_strlen(mape->path));
    pos += mrc_strlen(mape->path);

    //写入地图名
    mrc_writeInt16File(file, pos, mrc_strlen(mape->name));
    pos += 2;
    mrc_writeDataFile(file, pos, mape->name, mrc_strlen(mape->name));
    pos += mrc_strlen(mape->name);

    //写入地图数据
    mrc_writeInt32File(file, pos, mape->bkg);
    pos += 4;
    mrc_writeInt32File(file, pos, mape->maph);
    pos += 4;
    mrc_writeInt32File(file, pos, mape->mapw);
    pos += 4;
    mrc_writeInt32File(file, pos, mape->itemw);
    pos += 4;
    mrc_writeInt32File(file, pos, mape->itemh);
    pos += 4;

    //写入不确定数据
    mrc_writeInt32File(file, pos, 4);
    pos += 4;
    mrc_writeInt32File(file, pos, 8);
    pos += 4;

    //写入层次名
    mrc_writeInt16File(file, pos, mrc_strlen(mape->layname));
    pos += 2;
    mrc_writeDataFile(file, pos, mape->layname, mrc_strlen(mape->layname));
    pos += mrc_strlen(mape->layname);

    //写入资源名
    mrc_writeInt16File(file, pos, mrc_strlen(mape->resname));
    pos += 2;
    mrc_writeDataFile(file, pos, mape->resname, mrc_strlen(mape->resname));
    pos += mrc_strlen(mape->resname);

    //写入未知数据
    mrc_writeInt32File(file, pos, 5);
    pos += 4;

    //写入资源类型
    mrc_writeInt16File(file, pos, mrc_strlen(mape->restype));
    pos += 2;
    mrc_writeDataFile(file, pos, mape->restype, mrc_strlen(mape->restype));
    pos += mrc_strlen(mape->restype);

    //写入图片信息
    mrc_writeInt32File(file, pos, mape->resnum);
    pos += 4;
    for (i = 0; i < mape->resnum; i++)
    {
      mrc_writeInt16File(file, pos, mrc_strlen(mape->resarr[i]));
      pos += 2;
      mrc_writeDataFile(file, pos, mape->resarr[i], mrc_strlen(mape->resarr[i]));
      pos += mrc_strlen(mape->resarr[i]);

      //写入间隔
      mrc_writeInt32File(file, pos, 16);
      pos += 4;
      mrc_writeInt32File(file, pos, 16);
      pos += 4;
    }

    //写入固定头
    mrc_writeInt32File(file, pos, 0);
    pos += 4;
    mrc_writeInt32File(file, pos, 16);
    pos += 4;
    mrc_writeInt32File(file, pos, 16);
    pos += 4;
    mrc_writeInt32File(file, pos, 0x023C);
    pos += 4;

    //写入未知88字节
    for (i = 0; i < 88; i++)
    {
      mrc_writeInt8File(file, pos, 0x00);
      pos += 1;
    }

    //写入地图数据
    mrc_writeDataFile(file, pos, (char *)mape->data, mape->mapw * mape->maph * 4);
    pos += mape->mapw * mape->maph * 4;

    //写入结尾数据
    mrc_writeDataFile(file, pos, mape->end, mape->endlen);
    mrc_close(file);
  }
  return 0;
}

//导出mape为图片
int exportMapePng(_MAPE *mape, char *name)
{
  return 0;
}

//释放mape结构体
int freeMape(_MAPE *mape)
{
  int i;
  if (mape != NULL)
  {
    mrc_free(mape->dir);

    mrc_free(mape->path);
    mrc_free(mape->name);

    mrc_free(mape->layname);
    mrc_free(mape->resname);
    for (i = 0; i < mape->resnum; i++)
    {
      mrc_free(mape->resarr[i]);
      mrc_free_png(mape->resdata[i]);
      mrc_free(mape->resdata[i]);
    }

    mrc_free(mape->data);
    mrc_free(mape->end);

    mape = NULL;
  }
  return 0;
}
