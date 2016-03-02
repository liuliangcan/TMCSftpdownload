/* 
 * File:   Callback_functions_curl.cpp
 * Author: LIU
 * 
 * Created on 2016年2月19日, 下午2:59
 */

#include "Callback_functions_curl.h"

Callback_functions_curl::Callback_functions_curl()
{
}

Callback_functions_curl::Callback_functions_curl(const Callback_functions_curl& orig)
{
}

Callback_functions_curl::~Callback_functions_curl()
{
}

size_t Callback_functions_curl::data_handle(char *buffer, size_t size, size_t nmemb, void *stream)
{
    FTPDriver* ftpdriver = (FTPDriver*)stream;
    return ftpdriver->data_handle(buffer, size, nmemb);
}
size_t Callback_functions_curl::ScanDirDataFunction(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    FTPDriver* ftpdriver = (FTPDriver*)user_p;
    return ftpdriver->ScanDirDataFunction(buffer, size, nmemb);
}