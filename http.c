/*File : http.c
 *Auth : sjin
 *Date : 20141206
 *Mail : 413977243@qq.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "http.h"
#include "mylib.h"
#define BUFFER_SIZE 4096
//#define HTTP_POST "POST /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n"\
"Content-Type:application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s"
//#define HTTP_GET "GET /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n\r\n"
#define MSG_DEBUG   0x01
#define MSG_INFO    0x02
#define MSG_ERROR   0x04

typedef struct
{
    char * databuf;                       //与服务器通信的socket
    int totallen;
} mydata;


static int http_parse_url(const char *url,char *host,char *file,int *port)
{
    char *ptr1,*ptr2;
    int len = 0;
    if(!url || !host || !file || !port)
    {
        return -1;
    }

    ptr1 = (char *)url;

    if(!strncmp(ptr1,"http://",strlen("http://")))
    {
        ptr1 += strlen("http://");
    }
    else
    {
        return -1;
    }

    ptr2 = strchr(ptr1,'/');
    if(ptr2)
    {
        len = strlen(ptr1) - strlen(ptr2);
        memcpy(host,ptr1,len);
        host[len] = '\0';
        if(*(ptr2 + 1))
        {
            memcpy(file,ptr2 + 1,strlen(ptr2) - 1 );
            file[strlen(ptr2) - 1] = '\0';
        }
    }
    else
    {
        memcpy(host,ptr1,strlen(ptr1));
        host[strlen(ptr1)] = '\0';
    }
    //get host and ip
    ptr1 = strchr(host,':');
    if(ptr1)
    {
        *ptr1++ = '\0';
        *port = atoi(ptr1);
    }
    else
    {
        *port = MY_HTTP_DEFAULT_PORT;
    }

    return 0;
}


static char *http_parse_result(const char*lpbuf)
{
    char *ptmp = NULL;
    char *response = NULL;
    //    printf(lpbuf);
    ptmp = (char*)strstr(lpbuf,"HTTP/1.1");
    if(!ptmp)
    {
        printf("http/1.1 not faind\n");
        return NULL;
    }
    if(atoi(ptmp + 9)!=200)
    {
        printf("result:\n%s\n",lpbuf);
        return NULL;
    }

    ptmp = (char*)strstr(lpbuf,"\r\n\r\n");

    if(!ptmp)
    {
        printf("ptmp is NULL\n");
        return NULL;
    }
    response = (char *)malloc(strlen(ptmp)+1);
    if(!response)
    {
        printf("malloc failed \n");
        return NULL;
    }
// printf(ptmp);
    strcpy(response,ptmp+4);
    return response;
}


char *http_header(const char*lpbuf)
{
    char *p=NULL;
    char *ptmp = NULL;
    char *response = NULL;
    //   printf(lpbuf);
    ptmp = (char*)strstr(lpbuf,"HTTP/1.1");
    if(!ptmp)
    {
        printf("http/1.1 not faind\n");
        return NULL;
    }
    if(atoi(ptmp + 9)!=200)
    {
        printf("result:\n%s\n",lpbuf);
        return NULL;
    }
    p = strncasestr(ptmp, "Content-length");
    if(p)
    {
        p = strchr(p, ':');
        p += 2;     // 跳过冒号和后面的空格

        printf( "Content-length: %d\n",  atoi(p));
    }
    ptmp = (char*)strstr(lpbuf,"\r\n\r\n");

    if(!ptmp)
    {
        printf("ptmp is NULL\n");
        return NULL;
    }
    response = (char *)malloc(strlen(ptmp)+1);
    if(!response)
    {
        printf("malloc failed \n");
        return NULL;
    }
// printf(ptmp);
// memcpy(response,ptmp+4,4095-(&lpbuf-&ptmp-4));
    strcpy(response,ptmp+4);
    return response;
}
char * http_get(const char *url)
{

    char post[BUFFER_SIZE] = {'\0'};
    int socket_fd = -1;
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE] = {'\0'};
    int port = 0;
    int sockfd, ret, i, h;
    struct sockaddr_in servaddr;
    char str1[4096], str2[4096], buf[BUFFER_SIZE], *str;
    socklen_t len;
    fd_set   t_set1;
    struct timeval  tv;
// mydata *data=NULL;
    int total_len;
    unsigned long *addr;
    char *hostname;
    char *pdatabuf;
    char *pdst;
    char *psrc;
    if(!url)
    {
        printf("      failed!\n");
        return NULL;
    }

    if(http_parse_url(url,host_addr,file,&port))
    {
        printf("http_parse_url failed!\n");
        return NULL;
    }
    //printf("host_addr : %s\tfile:%s\t,%d\n",host_addr,file,port);
    //创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        printf("创建网络连接失败,本线程即将终止---socket error!\n");
       return NULL;
      //  exit(1);
    };

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    hostname=host_addr;
    addr=dns(hostname);
    servaddr.sin_addr.s_addr = addr;
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("连接到服务器失败,connect error!\n");
     return NULL;
     //   exit(1);
    }
   // printf("与远端建立了连接\n");
    memset(str2, 0, 4096);
    strcat(str2, "theDataToPost");
    str=(char *)malloc(128);
    len = strlen(str2);
    sprintf(str, "%d", len);
    memset(str1, 0, 4096);
    strcat(str1, "GET /");
    strcat(str1,file);
    strcat(str1," HTTP/1.1\n");
    strcat(str1, "Host:");
    strcat(str1,host_addr);
    strcat(str1,"\n");
    strcat(str1, "Content-Type: text/html\n");
    strcat(str1, "Content-Length: ");
    strcat(str1, str);
    strcat(str1, "\n\n");
    strcat(str1, str2);
    strcat(str1, "\r\n\r\n");
   free(str);
    ret = write(sockfd,str1,strlen(str1));
    if (ret < 0)
    {
        printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
      return NULL;
      //  exit(1);
    }
    else
    {
     //   printf("消息发送成功，共发送了%d个字节！\n\n", ret);
    }
    //  data = malloc(sizeof(mydata));
    // sleep(1);
    pdatabuf = (char*)malloc(200000*(sizeof(char)));
    pdst=pdatabuf;
    char *dest;
    char *src;
    FD_ZERO(&t_set1);
    FD_SET(sockfd, &t_set1);
    while(1)
    {
        tv.tv_sec= 3;
        tv.tv_usec= 0;
        h= 0;
        //  printf("--------------->1\n");
        h= select(sockfd +1, &t_set1, NULL, NULL, &tv);
        if (h == 0)
            break;
        if (h < 0)
        {
            close(sockfd);
            printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
            return -1;
        };

        if (h > 0)
        {
    memset(buf, 0, BUFFER_SIZE);
            len= read(sockfd, buf, BUFFER_SIZE);
            psrc=&buf;
            total_len+=strlen(psrc);
            i=len;
            if (i==0)
            {
                close(sockfd);
                printf("读取数据报文时发现远端关闭，该线程终止！\n");
                return -1;
            }
            else
            {
                src=psrc;
                dest=pdst;
                if ((src < dest) && (dest < src + i))  //重叠，从尾部开始复制
                {
                    pdst = pdst + i - 1;
                    psrc = psrc + i - 1;
                    while (i--)
                    {
                        *pdst-- = *psrc--;
                    }
                }
                else
                {
                    while (i--)
                    {
                        *pdst++ = *psrc++;
                    }
                }

            }

            //   printf("%s\n", buf);

        }

    }
     close(sockfd);
    char* httpcontent=http_header(pdatabuf);
 //  buftofile( "/home/cool/2.txt",pdatabuf);
    free(pdatabuf);
    return httpcontent;
}


char* http_post(const char *url,const char * post_str)
{

    int contentlen;
    contentlen=strlen(post_str);
   char contentstr[contentlen];
  //  int socket_fd = -1;
    char host_addr[BUFFER_SIZE] = {'\0'};
    char file[BUFFER_SIZE]={'0'};
    int port = 0;
    int sockfd, ret, i, h;
    struct sockaddr_in servaddr;
    char str1[contentlen+200], buf[BUFFER_SIZE], *str;
    socklen_t len;
    fd_set   t_set1;
    struct timeval  tv;
// mydata *data=NULL;
    int total_len;
    unsigned long *addr;
    char *hostname;
    char *pdatabuf;
    char *pdst;
    char *psrc;
    if(!url)
    {
        printf("      failed!\n");
        return NULL;
    }

    if(http_parse_url(url,host_addr,file,&port))
    {
        printf("http_parse_url failed!\n");
        return NULL;
    }
    //printf("host_addr : %s\tfile:%s\t,%d\n",host_addr,file,port);
    //创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        printf("创建网络连接失败,本线程即将终止---socket error!\n");
      return NULL;
    };

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    hostname=host_addr;
    addr=dns(hostname);
    servaddr.sin_addr.s_addr = addr;
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("连接到服务器失败,connect error!\n");
        return NULL;
    }
   // printf("与远端建立了连接\n");
    memset(contentstr, 0, contentlen);
    strcat(contentstr, post_str);
    str=(char *)malloc(128);
    sprintf(str, "%d", contentlen);
    memset(str1, 0, strlen(str1));
    strcat(str1, "POST /");
    strcat(str1,file);
    strcat(str1," HTTP/1.1\n");
    strcat(str1, "Host:");
    strcat(str1,host_addr);
    strcat(str1,"\n");
//   strcat(str1, "Content-Type: application/x-www-form-urlencoded\n");
    strcat(str1, "Content-Type: application/json\n");
    strcat(str1, "Content-Length: ");
    strcat(str1, str);
    strcat(str1, "\n\n");
    //str2的值为post的数据
    strcat(str1, contentstr);
    strcat(str1, "\r\n\r\n");
    free(str);
    ret = write(sockfd,str1,strlen(str1));
    if (ret < 0)
    {
        printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
       return NULL;
       }
    else
    {
    //    printf("消息发送成功，共发送了%d个字节！\n\n", ret);
    }
    //   data = malloc(sizeof(mydata));
    pdatabuf = (char*)malloc(200000*(sizeof(char)));
    pdst=pdatabuf;
    char *dest;
    char *src;
    FD_ZERO(&t_set1);
    FD_SET(sockfd, &t_set1);
    while(1)
    {
        tv.tv_sec= 3;
        tv.tv_usec= 0;
        h= 0;
        //   printf("--------------->1\n");
        h= select(sockfd +1, &t_set1, NULL, NULL, &tv);
        if (h == 0)
          {
                break;
          }
        if (h < 0)
        {
            close(sockfd);
            printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
            return -1;
        };

        if (h > 0)
        {
            memset(buf, 0, BUFFER_SIZE);
            len= read(sockfd, buf, BUFFER_SIZE);
            psrc=&buf;
            total_len+=strlen(psrc);
            i=len;
            if (i==0)
            {
                close(sockfd);
                printf("读取数据报文时发现远端关闭，该线程终止！\n");
                return -1;
            }
            else
            {
                src=psrc;
                dest=pdst;
                if ((src < dest) && (dest < src + i))  //重叠，从尾部开始复制
                {
                    pdst = pdst + i - 1;
                    psrc = psrc + i - 1;
                    while (i--)
                    {
                        *pdst-- = *psrc--;
                    }
                }
                else
                {
                    while (i--)
                    {
                        *pdst++ = *psrc++;
                    }
                }

            }


        }

    }
    close(sockfd);
    char* httpcontent=http_header(pdatabuf);
   //buftofile( "/home/cool/2.txt",content);
    free(pdatabuf);
    return httpcontent;

}

