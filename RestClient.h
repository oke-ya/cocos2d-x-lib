#ifndef __okeya__RestClient__
#define __okeya__RestClient__

#include "network/HttpRequest.h"
#include "network/HttpClient.h"
#include <unordered_map>

using namespace cocos2d::network;

class RestClient
{
public:
    typedef std::unordered_map<std::string, std::string> PostData;
    static RestClient* getInstance();
    static const std::string URL_PREFIX;

    void get( const std::string path, const ccHttpRequestCallback callback);
    void post(const std::string path, const ccHttpRequestCallback callback);
    void post(const std::string& path, PostData& postData, const ccHttpRequestCallback callback);
    void put(const std::string& path, PostData& postData, const ccHttpRequestCallback callback);
    void destroy(const std::string& path, PostData& postData, const ccHttpRequestCallback callback);
    void send(HttpRequest::Type type, const std::string& path, PostData& postData, const ccHttpRequestCallback& callback);
};

#endif /* defined(__okeya__RestClient__) */
