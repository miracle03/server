#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>   //针对系统调用的封装    fork,pipe 各种i/o原语 read write 等
#include<sys/socket.h>
#include<netinet/in.h>    //互联网地址族   定义数据结构sockaddr_in
#include<arpa/inet.h>   //提供IP地址转换函数
#include <openssl/bio.h>
#include <openssl/evp.h>
#include<ctype.h>    //一批C语言字符分类函数 用    于 测试字符是否属于特定的字符类别  topper()在这里
#include<time.h>
#define MAX 500 //运算中的最高可能位数
#define NUMP 2 //已提供的素数个数
#define NUMG 32 //随机数g的位数
#define swap(x,y) x^=y^=x^=y
#define SER_PORT 8080
char p_set[NUMP][110]=
{
    {"96872095226205901155646436505653"},
    {"96872095226205901155646436505653"}
};

unsigned char gcm_key[MAX]={'0'};

unsigned char gcm_iv[13] = {'0'};

unsigned char gcm_pt[MAX] = {'0'};

unsigned char gcm_aad[17] = {'0'};

unsigned char gcm_ct[MAX] = {'0'};

unsigned char gcm_tag[MAX] = {'0'};

int get_n(char*a)
{
    int i=MAX-1;
    while(i>=0&&!a[i])i--;
    return i+1;
}
int get_n1(unsigned char*a)
{
    int i=MAX-1;
    while(i>=0&&!a[i])i--;
    return i+1;
}
int get_mul(char*a,char*b)
{
    int na,nb,i,j,n;
    char c[MAX],d[MAX];
    for(i=0; i<MAX; i++)c[i]=a[i],d[i]=b[i],a[i]=0;
    na=get_n(c);
    nb=get_n(d);
    for(i=0; i<nb; i++)for(j=0; j<na; j++)
        {
            a[i+j]+=d[i]*c[j];
            if(a[i+j]>9)a[i+1+j]+=a[i+j]/10,a[i+j]%=10;
        }
    for(i=0; i<MAX&&i<(na+nb); i++)
    {
        if(a[i])n=i+1;
        if(a[i]>9)a[i+1]+=a[i]/10,a[i]%=10;
    }
    return n;
}
int get_mod(char*a,char*b)
{
    int na,nb,i,u,f=0,n;
    na=get_n(a);
    nb=get_n(b);
    u=na-nb;
    if(u<0)return 0;
    while(u+1)
    {
        for(i=na-1,f=0; i>=u; i--)
        {
            if(a[i]>b[i-u])
            {
                f=1;
                break;
            }
            if(a[i]<b[i-u])
            {
                f=-1;
                break;
            }
        }
        if(!f)
        {
            for(i=na-1; i>=u; i--)a[i]=0;
            u-=nb;
            if(u<0)break;
            continue;
        }
        if(f==-1)u--;
        if(f==1)
        {
            for(i=u; i<na; i++)
            {
                a[i]-=b[i-u];
                if(a[i]<0)a[i+1]--,a[i]+=10;
            }
        }
    }
    for(i=0; i<na; i++)if(a[i])n=i+1;

    return n;
}
void reverse(char*a)
{
    int i,n;
    n=get_n(a);
    for(i=0; i<n/2; i++)swap(a[i],a[n-1-i]);
}
void reverse1(unsigned char*a)
{
    int i,n;
    n=get_n1(a);
    for(i=0; i<n/2; i++)swap(a[i],a[n-1-i]);
}
void get_a(char*a)
{
    int i=0;
    while(a[i])a[i++]-='0';
}
void get_g(char*a)
{
    int i,r,j=0;;
    srand(time(0));
    while(1)
    {
        r=rand()%10000;
        for(i=0; i<4; i++)
        {
            a[j++]=r%10;
            r/=10;
            if(j==NUMG)return;
        }
    }
}
void get_p(char*a,int n)
{
    int i;
    for(i=0; i<100; i++)a[i]=p_set[n][i];
}
void display(char*a)
{
    int n,i;
    n=get_n(a);
    reverse(a);
    for(i=0; i<n; i++)printf("%d",a[i]);
    printf("\n");
    reverse(a);
}
void display1(unsigned char*a)
{
    int n,i;
    n=get_n1(a);
    reverse1(a);
    for(i=0; i<n; i++)printf("%d",a[i]);
    printf("\n");
    reverse1(a);
}
void rec(int b,char *p,char *g,char *ans)
{
    char t[MAX]= {0};
    int i;
    for(i=0; i<MAX; i++)t[i]=g[i],ans[i]=0;
    ans[0]=1;
    for(i=0; i<32; i++)
    {
        if(b&1<<i)
        {
            get_mul(ans,t);
            get_mod(ans,p);
        }
        get_mul(t,t);
        get_mod(t,p);
    }
}
void copy(char *a,unsigned char *b)//a to b
{
int n,i;
    n=get_n(a);
    reverse(a);
    for(i=0; i<n; i++){b[i]=a[i];}
    reverse(a);
    reverse1(b);

}

void sent(int a,char* p,char* g,char *ans)
{
    char t[MAX]= {0};
    int p_n=0;
    int i;
    memset(p,0,MAX*sizeof(p[0]));
    memset(g,0,MAX*sizeof(g[0]));
    srand(time(0));
    p_n=rand()%NUMP;   //随机得到p_n
    get_g(g);
    get_p(p,p_n);
    get_a(p);
    reverse(p);
    for(i=0; i<MAX; i++)t[i]=g[i],ans[i]=0;
    ans[0]=1;
    for(i=0; i<32; i++)
    {
        if(a&1<<i)
        {
            get_mul(ans,t);
            get_mod(ans,p);
        }
        get_mul(t,t);
        get_mod(t,p);
    }
}
void get_key(int b,char *rec,char*p,char*g,char *ans)
{
    char t[MAX]= {0};
    int i;

    for(i=0; i<MAX; i++)t[i]=rec[i],ans[i]=0;
    ans[0]=1;
    for(i=0; i<32; i++)
    {
        if(b&1<<i)
        {
            get_mul(ans,t);
            get_mod(ans,p);
        }
        get_mul(t,t);
        get_mod(t,p);
    }
} 


int aes_gcm_decrypt(void)
{
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen, rv,ctlen;
    unsigned char outbuf[1024];
    printf("AES GCM Derypt:\n");
    printf("Ciphertext:\n");
    BIO_dump_fp(stdout, gcm_ct, (int)strlen(gcm_ct));
    ctx = EVP_CIPHER_CTX_new();
    /* Select cipher */
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    /* Set IV length, omit for 96 bits */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, (int)strlen(gcm_iv), NULL);
    /* Specify key and IV */
    EVP_DecryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);
    /* Zero or more calls to specify any AAD */
    EVP_DecryptUpdate(ctx, NULL, &outlen, gcm_aad, (int)strlen(gcm_aad));
    /* Decrypt plaintext */

	ctlen=(int)strlen(gcm_ct);
	tmplen=0;	
	while(tmplen<=ctlen-16){
		EVP_DecryptUpdate(ctx, gcm_pt+tmplen, &outlen, gcm_ct+tmplen, 16);
		tmplen+=16;	
	}
    EVP_DecryptUpdate(ctx, gcm_pt+tmplen, &outlen, gcm_ct+tmplen, ctlen-tmplen);
    /* Output decrypted block */
    printf("Plaintext:\n");
    BIO_dump_fp(stdout, gcm_pt, (int)strlen(gcm_pt));
    /* Set expected tag value. */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, (int)strlen(gcm_tag), 
                        (void *)gcm_tag);
    /* Finalise: note get no output for GCM */
    rv = EVP_DecryptFinal_ex(ctx, outbuf, &outlen);
    /*
     * Print out return value. If this is not successful authentication
     * failed and plaintext is not trustworthy.
     */
    printf("Tag Verify %s\n", rv > 0 ? "Successful!" : "Failed!");
    EVP_CIPHER_CTX_free(ctx);
	if(rv>0) return 1;
	else return 0;
}

void client(int connfd){
	memset(gcm_key,0,MAX);
	memset(gcm_iv,0,MAX);
	memset(gcm_aad,0,MAX);
	int b; 
	int i,n;  	
	srand((int)time(0));
	b=rand()%10000+1;
    char sa[MAX],sb[MAX],p[MAX],g[MAX],ak[MAX],bk[MAX];
    printf("生成密钥中..........\nrand:%d\n",b);            
	for(int i=0;i<3;i++)
    {
		if(i==0)
        {memset(p,0,MAX);n = read(connfd,p,MAX);}  
        if(i==1)
        {memset(g,0,MAX);n = read(connfd,g,MAX);}
		if(i==2)
        {memset(sa,0,MAX);n = read(connfd,sa,MAX);}	    
    }
    memset(sb,0,MAX);
    rec(b,p,g,sb);
	get_key(b,sa,p,g,bk);
	printf("密钥为："); 
	display(bk);
    memset(gcm_key,0,MAX);
    copy(bk,gcm_key);
	write(connfd,sb,n);   //   // 向客户端写入数据   
	unsigned char temp[MAX]={'0'};
	int flag=0;
	for(;;){
		memset(temp,0,MAX);
		memset(gcm_pt,0,MAX);
		memset(gcm_tag,0,MAX);
		memset(gcm_ct,0,MAX);
		printf("-------receiving---------\n");
		
		if(flag==0){
			read(connfd,temp,MAX);//aad
			if(strcmp(temp,"quit")==0){
				printf("client quit\n");
				break;
			}
			strncpy(gcm_aad,temp,16);
			printf("recv aad:\n");
			BIO_dump_fp(stdout, gcm_aad, (int)strlen(gcm_aad));		
		
			memset(temp,0,MAX);
			read(connfd,temp,MAX);//iv
			strncpy(gcm_iv,temp,12);
			printf("recv iv:\n");
			BIO_dump_fp(stdout, gcm_iv, (int)strlen(gcm_iv));
			flag=1;
		}
		
		memset(temp,0,MAX);
		read(connfd,temp,16);//tag
		//printf("ret:%d\n",(int)strlen(temp));
		while(strcmp(temp,"")==0){read(connfd,temp,16);}//tag
		if(strcmp(temp,"quit")==0){
				printf("client quit\n");
				break;
		}
		strncpy(gcm_tag,temp,16);
		printf("recv tag:\n");
		BIO_dump_fp(stdout, gcm_tag, (int)strlen(gcm_tag));		

		memset(temp,0,MAX);
		read(connfd,temp,MAX);//ct
		strncpy(gcm_ct,temp,MAX);
		printf("recv ct:\n");
		BIO_dump_fp(stdout, gcm_ct, (int)strlen(gcm_ct));

		int ret=aes_gcm_decrypt();

		if(ret==1)	
			write(connfd,"yes!",(int)strlen("yes!"));
		else 
			write(connfd,"no!",(int)strlen("no!"));
	}
	close(connfd);

}
int main(void){
    struct sockaddr_in servaddr,cliaddr;
    socklen_t cliaddr_len;

    int listenfd,connfd;
    char buf[MAX];
    char str[INET_ADDRSTRLEN];

    listenfd = socket(AF_INET,SOCK_STREAM,0); 
    bzero(&servaddr,sizeof(servaddr));   //初始化赋值为0

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   //任何ip   //这里是大小端的 转换问题。。可以 百度
    servaddr.sin_port = htons(SER_PORT);     //端口
    bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));   //绑定链接的套接字描述符  和  地址和端口
    listen(listenfd,20);  
    cliaddr_len = sizeof(cliaddr);
    
	while(1){
		printf("Accepting connections ... \n");
		connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&cliaddr_len); 
		client(connfd);
	}
                                               
	//close(listen);
    return 0;                       

}



