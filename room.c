#include <stdio.h>
#include <stdlib.h>
#include "http.h"
#include <string.h>
#include"cJSON.h"
#include"mylib.h"
#include"room.h"
//a struct array to CJSON
char * struct_array_to_cJSON2(char *text,struct zhubo zhubolist[],char *chname,int listcount)
{
    cJSON *json,*arrayItem,*item,*object;
    int i,j,drop;
    char *temp;
    char *pt;
    struct zhubo choklist[listcount];
    if((text==NULL) || (zhubolist==NULL))
    {
        printf("%s: input is invalid",__func__);
    }

    char * out=NULL;
    cJSON *root=cJSON_CreateObject();

    if (!root)
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    }
    else
    {
        cJSON *array_ch=cJSON_CreateArray();

        for(i=0; i<listcount; i++)
        {


            cJSON *obj_ch=cJSON_CreateObject();

            cJSON *item=cJSON_CreateString(zhubolist[i].title);
            cJSON_AddItemToObject(obj_ch,"nickname",item);
            item=cJSON_CreateString(zhubolist[i].img);
            cJSON_AddItemToObject(obj_ch,"picurl",item);
            item=cJSON_CreateString(zhubolist[i].address);
            cJSON_AddItemToObject(obj_ch,"playurl",item);

            cJSON_AddItemToArray(array_ch,obj_ch);

        }
        cJSON *item=cJSON_CreateString("AddRoomlist");
        cJSON_AddItemToObject(root,"Action",item);
        item=cJSON_CreateString(chname);
        cJSON_AddItemToObject(root,"channel",item);
        cJSON_AddItemToObject(root,"roominfo",array_ch);
        out=cJSON_Print(root);
        // printf("out:%s\n",out);
        cJSON_Delete(root);
        if(out!=NULL)
        {
            strcpy(text,out);
            free(out);
        }
    }
//return text;

}


// create CJSON, write file
char * jstrroom(char *chjson,struct zhubo *zhubolist,char *chname,int listcount)
{
//   struct_to_cJSON(data, &zhubolist[1]);
    struct_array_to_cJSON2(chjson, zhubolist,chname,listcount);
    return chjson;
}
