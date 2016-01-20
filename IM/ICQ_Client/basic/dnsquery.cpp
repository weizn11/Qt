#include "dnsquery.h"

int dns_query(char *domain,char *ip)
{
    char **tmp=NULL;
    struct hostent *Target;
    struct in_addr addr;

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
    Target=gethostbyname(domain);
    if(Target==NULL)
    {
        return -1;
    }

    for(tmp=Target->h_addr_list; *tmp!=NULL; tmp++)
    {
        memcpy(&addr.S_un.S_addr,*tmp,Target->h_length);
        memcpy(ip,inet_ntoa(addr),strlen(inet_ntoa(addr))+1);
    }

    return 0;
}
