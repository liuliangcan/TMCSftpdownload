/* 
 * File:   Callback_functions_curl.h
 * Author: LIU
 *
 * Created on 2016年2月19日, 下午2:59
 */

#ifndef CALLBACK_FUNCTIONS_CURL_H
#define	CALLBACK_FUNCTIONS_CURL_H

#include "FTPDriver.h"
class Callback_functions_curl {
public:
    
    static size_t data_handle(char *buffer, size_t size, size_t nmemb, void *stream);
    static size_t ScanDirDataFunction(void *buffer, size_t size, size_t nmemb, void *user_p);
    Callback_functions_curl();
    Callback_functions_curl(const Callback_functions_curl& orig);
    virtual ~Callback_functions_curl();
private:

};

#endif	/* CALLBACK_FUNCTIONS_CURL_H */

