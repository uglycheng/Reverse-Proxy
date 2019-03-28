#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
    int proxy_sock_ListenToClient, client_sock, proxy_sock_TransferToServer;
    char request[BUF_SIZE], response[BUF_SIZE];
    int str_len_from_client, str_len_from_server, i;
    char *clientend_port = "80";
    char *server_ip = "192.168.0.20";

    struct sockaddr_in proxy_adr, client_adr, server_adr;
    socklen_t client_adr_sz;

    proxy_sock_ListenToClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    proxy_sock_TransferToServer = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);


    memset(&proxy_adr,0,sizeof(proxy_adr));
    proxy_adr.sin_family = AF_INET;
    proxy_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    proxy_adr.sin_port = htons(atoi(clientend_port));

    memset(&server_adr,0,sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    server_adr.sin_addr.s_addr = inet_addr(server_ip);
    server_adr.sin_port = htons(atoi(argv[1]));

    if(bind(proxy_sock_ListenToClient, (struct sockaddr*)&proxy_adr, sizeof(proxy_adr))==-1)
        printf("bind error\n");

    if(listen(proxy_sock_ListenToClient,5)==-1) printf("listen error \n");

    if(connect(proxy_sock_TransferToServer, (struct sockaddr*)&server_adr, sizeof(server_adr))==-1)
        printf("connect error\n");
    else
        printf("connected to server....");

    
    client_adr_sz = sizeof(client_adr);

    for(i=0; i<5; i++)
    {
        client_sock = accept(proxy_sock_ListenToClient, (struct sockaddr*)&client_adr,&client_adr_sz);
        if(client_sock == -1) printf("accept error %d\n",i);
        else printf("Connected client %d \n",i);
        while(str_len_from_client=read(client_sock,request,BUF_SIZE)!=0){
            printf("%s",request);
            write(proxy_sock_TransferToServer,request,strlen(request));
            str_len_from_server=read(proxy_sock_TransferToServer,response,BUF_SIZE-1);
            write(client_sock, response,str_len_from_server);
        }
        close(client_sock);
        
    }
    close(proxy_sock_TransferToServer);
    close(proxy_sock_ListenToClient);
    return 0;

}