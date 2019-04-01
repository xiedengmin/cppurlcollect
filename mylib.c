#include <stdio.h>
#include <stdlib.h>
#include "http.h"
#include <string.h>
#include"cJSON.h"
#include"mylib.h"
void * mymemcpy(char *dest,const char *scr,int len)
{
    if (dest == NULL || scr == NULL)
        return;
    char *pdest = dest;
    char *pscr = (char *)scr;
    if ((scr < dest) && (dest < scr + len))  //重叠，从尾部开始复制
    {
        pdest = pdest + len - 1;
        pscr = pscr + len - 1;
        while (len--)
        {
            *pdest-- = *pscr--;
        }
    }
    else
    {
        while (len--)
        {
            *pdest++ = *pscr++;
        }
    }
    return *dest;
}
void buftofile(char* tofile, char *buf)
{
    FILE * pFile;
    pFile = fopen (tofile, "wb+" );
    fwrite (buf, sizeof(char), strlen(buf), pFile );
    fclose (pFile);

}
/* 不区分大小写的strstr */
char* strncasestr(char *str, char *sub)
{
    char * ret=NULL;
    if(!str || !sub)
        return NULL;

    int len = strlen(sub);
    if (len == 0)
    {
        return NULL;
    }

    while (*str)
    {
        if (strncasecmp(str, sub, len) == 0)
        {
            ret=str;
            return ret;
        }
        ++str;
    }
    return NULL;
}
int strpos(const char *haystack,const char *needle, int ignorecase)
{
    register unsigned char c, needc;
    unsigned char const *from, *end;
    int len = strlen(haystack);
    int needlen = strlen(needle);
    from = (unsigned char *)haystack;
    end = (unsigned char *)haystack + len;
    const char *findreset = needle;
    for (int i = 0; from < end; ++i) {
        c = *from++;
        needc = *needle;
        if (ignorecase) {
            if (c >= 65 && c < 97)
                c += 32;
            if (needc >= 65 && needc < 97)
                needc += 32;
        }
        if(c == needc) {
            ++needle;
            if(*needle == '\0') {
                if (len == needlen)
                    return 0;
                else
                    return i - needlen+1;
            }
        }
        else {
            if(*needle == '\0' && needlen > 0)
                return i - needlen +1;
            needle = findreset;
        }
    }
    return  -1;
}
//create a key-value pair
int str_to_cJSON(char *data)
{
    char * out=NULL;
    cJSON *root=cJSON_CreateObject();
    if (!root)
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    }
    else
    {
        cJSON *item=cJSON_CreateString("AddChannellist");
        cJSON_AddItemToObject(root,"Action",item);

        out=cJSON_Print(root);
        printf("out2:%s\n",out);
        cJSON_Delete(root);
        if(out!=NULL)
        {
            free(out);
        }
    }
    return 0;
}
//a struct array to CJSON
 char * struct_array_to_cJSON(char *text,struct channel channellist[],int listcount)
{
    cJSON *json,*arrayItem,*item,*object;
    int i,j,txtpos,drop;
    char *chname,*temp;
    char *pt;
    struct channel choklist[listcount];
    if((text==NULL) || (channellist==NULL))
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
        const char* dropch[4]= {"星光","龙珠","齐齐","映客"};
        for(i=0; i<listcount; i++)
        {
            drop=0;
            for (j=0; j<4; j++)
            {
                if (strcmp(channellist[i].title,dropch[j])==0)
                {
                    drop=1;
                   channellist[i].number="0";
                    break;
                }
            }
            if (atoi(channellist[i].number)<10)
            {
                drop=1;
                   channellist[i].number="0";
            }

            if (drop==0)
            {
                cJSON *obj_ch=cJSON_CreateObject();

                cJSON *item=cJSON_CreateString(channellist[i].title);
                cJSON_AddItemToObject(obj_ch,"title",item);
                item=cJSON_CreateString(channellist[i].img);
                cJSON_AddItemToObject(obj_ch,"img",item);
                 pt=(char*)malloc(128);

                strcpy(pt,channellist[i].latform_type);
                // chname=choklist[i].latform_type;
                temp=strstr(pt,".txt");
                if (!temp==NULL)
                {
                    txtpos=temp-pt-4;
                    chname=(char*)malloc(txtpos);
               memset(chname,0,strlen(chname));
                    mymemcpy(chname,pt+4,txtpos);
                }
            strcat(chname,"\0");
                item=cJSON_CreateString(chname);
                cJSON_AddItemToObject(obj_ch,"latform_type",item);
                item=cJSON_CreateString(channellist[i].number);
                cJSON_AddItemToObject(obj_ch,"number",item);
                cJSON_AddItemToArray(array_ch,obj_ch);
                  free(pt);
                  free(chname);
            }
        }
        cJSON *item=cJSON_CreateString("AddChannellist");
        cJSON_AddItemToObject(root,"Action",item);
        cJSON_AddItemToObject(root,"channellist",array_ch);
        out=cJSON_Print(root);
       // printf("out:%s\n",out);
        cJSON_Delete(root);
        if(out!=NULL)
        {
            memcpy(text,out,strlen(out));
            free(out);
        }
    }
//return text;

}


// create CJSON, write file
char * jsonstrchannel(char *chjson,struct channel *chlist,int listcount)
{
char *title;
//	char str_name[40];
//	int ret = str_to_cJSON(data, str_name);
    /*
        channel channellist[3]=
        {
            {"testtile1","zhong1.jpg","Jason","100"},
            {"testtile2","zhong2.jpg","Jason","100"},
            {"testtile3","zhong3.jpg","Jason","100"}
        };
        */
//   struct_to_cJSON(data, &channellist[1]);
   struct_array_to_cJSON(chjson, chlist,listcount);
    return chjson;
}

//create a object from struct
int struct_to_cJSON(char *json_string, struct channel *ch)
{

    if((json_string==NULL) || (ch==NULL))
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
        cJSON *obj_ch=cJSON_CreateObject();

        cJSON *item=cJSON_CreateString(ch->title);
        cJSON_AddItemToObject(obj_ch,"title",item);

        item=cJSON_CreateString(ch->img);
        cJSON_AddItemToObject(obj_ch,"img",item);

        item=cJSON_CreateString(ch->latform_type);
        cJSON_AddItemToObject(obj_ch,"latform_type",item);
        item=cJSON_CreateString(ch->number);
        cJSON_AddItemToObject(obj_ch,"number",item);
        cJSON_AddItemToObject(root,"ch",obj_ch);
        out=cJSON_Print(root);
        printf("out2:%s\n",out);
        cJSON_Delete(root);
        if(out!=NULL)
        {
            memcpy(json_string,out,strlen(out));
            free(out);
        }
    }

    return 0;
}

