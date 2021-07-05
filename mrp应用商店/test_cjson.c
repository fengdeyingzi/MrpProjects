#include <stdio.h>
#include <cJSON.h>
int main(){
    //用char* 模拟一个JSON字符串
char* json_arr_string =\
"{,\
 \"test_1\":\"1\",\
 \"test_2\":\"2\",\
 \"test_arr\":\"[{\",\
\"{\",\
  \"test_1\":\"arr_1\",\
  \"test_2\":\"arr_2\",\
  \"test_3\":\"arr_3\",\
\"}\",\
\"{\"\
 \"test_1\":\"1\",\
 \"test_2\":\"2\",\
 \"test_3\":\"3\",\
\"}\",\
\"}]\",\
\"test_3\" :\"3\",\
}";//json_arr string;
 
//第一步打包JSON字符串
cJSON* cjson = cJSON_Parse(json_arr_string);
 
//判断是否打包成功
if(cjson == NULL){
printf("cjson error…");
}
else{//打包成功调用cJSON_Print打印输出
cJSON_Print(cjson);
}
 
//获取数组对象
cJSON* test_arr = cJSON_GetObjectltem(cjson,"test_arr");
 
//获取数组对象个数便于循环
int arr_size = cJSON_GetArraySize(test_arr);//return arr_size 2
 
//获取test_arr数组对象孩子节点
cJSON* arr_item = test_arr->child;//子对象
 
//循环获取数组下每个字段的值并使用cJSON_Print打印
for(int i = 0;i <=(arr_size-1)/*0*/;++i){
cJSON_Print(cJSON_GetObjectItem(arr_item,"test_1"));
cJSON_Print(cJSON_GetObjectItem(arr_item,"test_2"));
cJSON_Print(cJSON_GetObjectItem(arr_item,"test_3"));
arr_item = arr_item->next;//下一个子对象
}
 
//delete cjson 这里只需要释放cjson即可，因为其它的都指向它
cJSON_Delete(cjson);