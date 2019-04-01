#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "http.h"
#include"cJSON.h"
#include"mylib.h"
#include"room.h"
/*如果定义了 DEBUG  就会显示出  请求头和返回头*/
#define DEBUG 1
void room(struct channel chlist[],int count)
{
    char *json_string;
    char *chjson,*temp;
    char *jsonback,*pt;
    char chname[50];
    int i,j,k,roomcount,txtpos,zbindex;
    char *baseurl= {"http://api.hclyz.com:81/mf/"};
    struct channel *choklist;
    k=0;
    for (j = 0; j < 100; j++)
    {
        if (!(strcmp(chlist[j].number,"0")==0))
        {
            choklist[k]= chlist[j];
            k++;
        }
    }
    while( 1)
    {
        for (j = 0; j <= k; j++)
        {
            if (!(strcmp(choklist[j].number,"0")==0))
            {
                pt=(char*)malloc(128);
                memset(pt,0,128);
                strcpy(pt,choklist[j].latform_type);
                // chname=choklist[i].latform_type;
                temp=strstr(pt,".txt");
                if (!temp==NULL)
                {
                    txtpos=temp-pt-4;
                   //*temp++="\0";
               memset(chname,0,50);
                   mymemcpy(chname,pt+4,txtpos);
                }

strcat(chname,"\0");
               printf("current channel:%s char len:%d",chname,txtpos);
                char roomurl[256]= {"\0"};
                strcat(roomurl,baseurl);
                strcat(roomurl,choklist[j].latform_type);
           //   if (strpos(chname,"yumen",0)>=0){
          //          printf("yumen coming.");
          //    }
                json_string=http_get(roomurl);
                cJSON* json = cJSON_Parse(json_string);
                if(json == NULL)
                {
                    printf("json1 pack into cjson error...");
                }
                else //打包成功调用cJSON_Print打印输出
                {
                    cJSON *node = NULL;
                    node = cJSON_GetObjectItem(json,"zhubo");
                    if(node == NULL)
                    {
                        printf("json2 pack into cjson error...");
                    }
                    else
                    {
                        roomcount = cJSON_GetArraySize(node);
                        struct  zhubo zblist[roomcount];

                        for (i = 0; i < roomcount; i++)
                        {
                            cJSON *one = cJSON_GetArrayItem(node, i);
                            if (NULL == one)
                            {
                                continue;
                            }
                            cJSON *address = cJSON_GetObjectItem(one, "address");
                            if (NULL == address)
                            {
                                continue;
                            }
                            cJSON *img = cJSON_GetObjectItem(one, "img");
                            if (NULL == img)
                            {
                                continue;
                            }

                            cJSON *title = cJSON_GetObjectItem(one, "title");
                            if (NULL == title)
                            {
                                continue;
                            }
                            //  printf("%s\n",address->valuestring);

                            zblist[i].address=address->valuestring;
                            zblist[i].img=img->valuestring;
                            zblist[i].title=title->valuestring;
                        }
                        free(node);
                        chjson=(char*)malloc(200000*sizeof(char));
                        jstrroom(chjson,zblist,chname,roomcount);
                      //  buftofile( "/home/cool/0.txt",chjson);
                        http_post("http://izb8.info/interface.php",chjson);
                        free(chjson);

                    }
                }
                     free(pt);
                //  free(chname);
            }
        }

        sleep(1);
    }
}
int main1()
{

    char *json_string;
    char *chjson;
    char *jsonback;
    int i, count;

    struct  channel *choklist;
    //http_download("http://api.hclyz.com:81/mf/jiekou.php","/home/cool/1.txt");
    json_string=http_get("http://api.hclyz.com:81/mf/json.txt");
    //  printf(json_string);
    cJSON* json = cJSON_Parse(json_string);
    if(json == NULL)
    {
        printf("json pack into cjson error...");
    }
    else //打包成功调用cJSON_Print打印输出
    {
        cJSON *node = NULL;
        node = cJSON_GetObjectItem(json,"pingtai");
        if(node == NULL)
        {
            printf("json pack into cjson error...");
        }
        else
        {
            count = cJSON_GetArraySize(node);
            struct  channel chlist[count];

            for (i = 0; i < count; i++)
            {
                cJSON *one = cJSON_GetArrayItem(node, i);
                if (NULL == one)
                {
                    continue;
                }
                cJSON *address = cJSON_GetObjectItem(one, "address");
                if (NULL == address)
                {
                    continue;
                }
                cJSON *xinimg = cJSON_GetObjectItem(one, "xinimg");
                if (NULL == xinimg)
                {
                    continue;
                }
                cJSON *number = cJSON_GetObjectItem(one, "number");
                if (NULL == number)
                {
                    continue;
                }
                cJSON *title = cJSON_GetObjectItem(one, "title");
                if (NULL == title)
                {
                    continue;
                }
                //  printf("%s\n",address->valuestring);

                chlist[i].latform_type=address->valuestring;
                chlist[i].img=xinimg->valuestring;
                chlist[i].number=number->valuestring;
                chlist[i].title=title->valuestring;
            }
            free(node);
            chjson=(char*)malloc(100000*sizeof(char));
            jsonstrchannel(chjson,chlist,count);
            choklist=chlist;
              http_post("http://izb8.info/interface.php",chjson);
            free(chjson);
        }
        if (count>0)
        {
            room(choklist,count);

        }
    }
//   getchar();
    return 0;
}

