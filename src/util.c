#include "hog.h"

int submit(int s, const void *buf, ssize_t len)
{
    while(len > 0){
        int ret = send(s, buf, len, 0);
        if(ret == 0) return EBADF;
        else if(ret < 0) return ret;
        buf += ret;
        len -= ret;
    }
    return 0;
}

int submit_chunk(int s, const char *buf)
{
    uint32_t len = strlen(buf);
    uint32_t nlen = htonl(len);
    if(submit(s, &nlen, sizeof(nlen)) != 0) return -1;
    if(submit(s, buf, len) != 0) return -1;
    return 0;
}

int receive(int s, void *buf, ssize_t len)
{
    while(len > 0){
        int ret = recv(s, buf, len, 0);
        if(ret == 0) return EBADF;
        else if(ret < 0) return ret;
        buf += ret;
        len -= ret;
    }
    return 0;
}

void ntoh_buf(void *buf, uint32_t len, char type){
    switch(type){
    case GRN_DB_INT16: case GRN_DB_UINT16:
        *(uint16_t*)buf = ntohs(*(uint16_t*)buf);
        break;
    case GRN_DB_INT32: case GRN_DB_UINT32:
        *(uint32_t*)buf = ntohl(*(uint32_t*)buf);
        break;
    }
}

void hton_buf(void *buf, uint32_t len, char type){
    switch(type){
    case GRN_DB_INT16: case GRN_DB_UINT16:
        *(uint16_t*)buf = htons(*(uint16_t*)buf);
        break;
    case GRN_DB_INT32: case GRN_DB_UINT32:
        *(uint32_t*)buf = htonl(*(uint32_t*)buf);
        break;
    }
}
