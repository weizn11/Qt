#include "global.h"
#include "encode.h"

wchar_t * ANSIToUnicode( const char* str )
{
    int textlen ;
    wchar_t * result;
    textlen = MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 );
    result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
    memset(result,0,(textlen+1)*sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );
    return result;
}

char * UnicodeToANSI( const wchar_t* str )
{
    char* result;
    int textlen;
    textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
    result =(char *)malloc((textlen+1)*sizeof(char));
    memset( result, 0, sizeof(char) * ( textlen + 1 ) );
    WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
    return result;
}

wchar_t * UTF8ToUnicode( const char* str )
{
    int textlen ;
    wchar_t * result;
    textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
    result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
    memset(result,0,(textlen+1)*sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
    return result;
}

char * UnicodeToUTF8( const wchar_t* str )
{
    char* result;
    int textlen;
    textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
    result =(char *)malloc((textlen+1)*sizeof(char));
    memset(result, 0, sizeof(char) * ( textlen + 1 ) );
    WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
    return result;
}

/*宽字符转换为多字符Unicode - ANSI*/

char* w2m(const wchar_t* wcs)
{
    int len;
    char* buf;
    len =wcstombs(NULL,wcs,0);
    if (len == 0)           return NULL;
    buf = (char *)malloc(sizeof(char)*(len+1));
    memset(buf, 0, sizeof(char) *(len+1));
    len =wcstombs(buf,wcs,len+1);
    return buf;
}

/*多字符转换为宽字符ANSI - Unicode*/

wchar_t* m2w(const char* mbs)
{
    int len;
    wchar_t* buf;
    len =mbstowcs(NULL,mbs,0);
    if (len == 0)           return NULL;
    buf = (wchar_t *)malloc(sizeof(wchar_t)*(len+1));
    memset(buf, 0, sizeof(wchar_t) *(len+1));
    len =mbstowcs(buf,mbs,len+1);
    return buf;
}

char* ANSIToUTF8(const char* str)
{
    return UnicodeToUTF8(ANSIToUnicode(str));
}

char* UTF8ToANSI(const char* str)
{
    return UnicodeToANSI(UTF8ToUnicode(str));
}

char *QString_to_char(QString qstring)
{
    char *str=NULL;

    std::string str2=qstring.toStdString();

    str=(char *)malloc(str2.size()+1);
    if(str==NULL) return NULL;
    memset(str,NULL,str2.size()+1);
    strcat(str,(char *)str2.data());

    return str;
}

















