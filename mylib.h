#ifndef MYLIB_H_INCLUDED
#define MYLIB_H_INCLUDED
struct  channel
{
    char *title;
    char *img;
    char *latform_type;
    char *number;
} ;
#ifdef __cplusplus
extern "C" {
#endif
void * mymemcpy(char *dest,const char *scr,int len);
void buftofile(char* tofile,char *buf);
/* 不区分大小写的strstr */
int strpos(const char *haystack,const char *needle,int ignorecase);
char *strncasestr(char *str, char *sub);
char * jsonstrchannel(char* channel,struct channel *chlist,int listcount);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MYLIB_H_INCLUDED
