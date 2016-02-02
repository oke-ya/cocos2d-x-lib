#include "RestClient.h"
#include <future>
#include "curl/curl.h"
#include "Env.h"
#include <iostream>

const std::string RestClient::URL_PREFIX = API_URL;

RestClient* RestClient::getInstance()
{
    static RestClient s_instance;
    return &s_instance;
}

void RestClient::post(const std::string path, const ccHttpRequestCallback callback)
{
    PostData postData;
    send(HttpRequest::Type::POST, path, postData, callback);
}

void RestClient::post(const std::string& path, PostData& postData, const ccHttpRequestCallback callback)
{
    send(HttpRequest::Type::POST, path, postData, callback);
}

void RestClient::get(const std::string path, const ccHttpRequestCallback callback)
{
    PostData postData;
    send(HttpRequest::Type::GET, path, postData, callback);
}

void RestClient::put(const std::string& path, PostData& postData, const ccHttpRequestCallback callback)
{
    send(HttpRequest::Type::PUT, path, postData, callback);
}

void RestClient::destroy(const std::string& path, PostData& postData, const ccHttpRequestCallback callback)
{
    send(HttpRequest::Type::DELETE, path, postData, callback);
}

void RestClient::send(HttpRequest::Type type, const std::string& path, PostData& postData, const ccHttpRequestCallback& callback)
{
    std::string postString;
    for(auto pair: postData){
        std::string k = std::get<0>(pair);
        std::string v = std::get<1>(pair);
        if(postString.length() > 0){
            postString += "&";
        }
        postString += curl_escape(k.c_str(), static_cast<int>(k.length()));
        postString += "=";
        postString += curl_escape(v.c_str(), static_cast<int>(v.length()));
    }
    auto* request = new HttpRequest();
    std::string url;
    if(path.substr(0, 4) == "http"){
        url = path;
    }else{
        url = RestClient::URL_PREFIX + path;
    }
    if(type == HttpRequest::Type::DELETE){
        if(url.find("?") == std::string::npos){
            url += "?" + postString;
        }else{
            url += "&" + postString;
        }
    }
    
    request->setUrl(url.c_str());
    
    request->setRequestType(type);
    request->setResponseCallback(callback);
    
    request->setRequestData(postString.c_str(), postString.length());
    HttpClient::getInstance()->send(request);
    request->release();
}
