# **mrp_core��Ŀ���**
mrp��Ϸ����ģ�棬ʵ��Window, Sprite, Object, Map�ȹ��ܣ��ӿ�mrp��Ϸ����


## **Windowϵͳ**
mrp��SGL����ڸ��ӣ����ø��Ӽ���winϵͳ��˹��SDK��mrc_win.c
��ʱδ���з�װ�����˼���windowģ��ɹ��ο�

## **Sprite����**
�ƻ�ʵ�����ַ�����mrp�Դ�Sprite����Ͷ����༭��SpriteX����

### **˹��SDK��Sprite**
˹��sdk�ڰ����˼򵥵�Sprite���ƹ��ܣ����Զ���ͼ�����Ƕ�֡����bmpͼƬ��Ҫ�������ͼ����Sprite�����˼򵥵ķ�װ

``` 
//����
typedef struct sprite
{
    int32 bmp_id; //��ӦbmpID
    int32 width;
    int32 height;
    int32 image_number;
    int32 start; //����һ��ͼƬ�������, Ĭ����0

    int32 posx; //���ĵ������ͼƬλ�ã������������ƹ���
    int32 posy;
    int32 bmmode;

    Bbox box; //�����posx��posy�ľ���
} Sprite; 
```

��װ���Sprite֧�ֶ�֡���ƣ��������������ײ�е��ŵ�

### **�����༭����SpriteX**
���幦�������У���Ϊ��Ҫ����Object��ģ�飬��չ����  

## **Object����**
��Sprite�������߼�����ȣ�Ŀǰ��ʵ�ּ򵥹���

## **map��ͼ**
��ͼ���ܰ����˻��ƺ���ײ��⣬�Զ�Ѱ·�ȹ���
Ŀǰʵ��mape�ļ��Ķ�ȡ��ʾ���ƻ�ʵ��tmx��ͼ�Ķ�ȡ����

## **����ģ��**
����Ŀ���õ���ʵ�ֵ���������

### **TPNGģ��**
pngͼƬ����


