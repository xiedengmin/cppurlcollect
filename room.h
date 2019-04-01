struct  zhubo
{
    char *title;
    char *img;
char *address;
} ;
#ifdef __cplusplus
extern "C" {
#endif
char * jstrroom(char* zb,struct zhubo zhubolist[],char *chname,int listcount);
#ifdef __cplusplus
}
#endif // __cplusplus
