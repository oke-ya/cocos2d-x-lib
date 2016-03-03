#include "platform/CCCommon.h"
#include "CentralClient.h"
#include "Env.h"
#include "EnvPrivate.h"
#include "RestClient.h"
#include "cocos/base/CCUserDefault.h"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#ifdef __APPLE__
#include "TargetConditionals.h"
#endif


#if TARGET_IPHONE_SIMULATOR
    const std::string CentralClient::API_URL = "http://localhost:3000";
#elif COCOS2D_DEBUG
//    const std::string CentralClient::API_URL = "https://ocentral-api-stg.herokuapp.com/";
const std::string CentralClient::API_URL = "http://192.168.0.2:3000";
#else
    const std::string CentralClient::API_URL = "https://ocentral-api.herokuapp.com/";
#endif
//const std::string CentralClient::API_URL = "http://yalab.example.com:3000";


const char* PAYMENT_USER_TOKEN = "paymentUserToken";
const int CentralClient::ChargeNone = -1;
const int CentralClient::INVALID_ITEM_BOUGHT = -1;

CentralClient::CentralClient()
: _init(false)
, _currentCharge(CentralClient::ChargeNone)
, _userToken("")
{
}

CentralClient* CentralClient::getInstance()
{
    static CentralClient s_instance;
    return &s_instance;
}

void CentralClient::init()
{
    if(_init){
        return;
    }
    setUserToken(UserDefault::getInstance()->getStringForKey(PAYMENT_USER_TOKEN));
    if(getUserToken().length() < 1){
        createUser();
    }
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    TIAPDeveloperInfo pGoogleInfo;
    pGoogleInfo["GooglePlayAppKey"] = GOOGLE_APPKEY;
    
    if(pGoogleInfo.empty()) {
        char msg[256] = { 0 };
        sprintf(msg, "Google App Key info is empty. PLZ fill your Google App Key info in %s(nearby line %d)", __FILE__, __LINE__);
        MessageBox(msg, "Google IAP Warning");
    }
    s_protocolIAP = dynamic_cast<ProtocolIAP*>(PluginManager::getInstance()->loadPlugin("IAPGooglePlay"));
    s_protocolIAP->configDeveloperInfo(pGoogleInfo);
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    TIAPDeveloperInfo info;
    s_protocolIAP = dynamic_cast<ProtocolIAP*>(PluginManager::getInstance()->loadPlugin("IOSIAP"));
    
    auto itr = SKU.begin();
    std::string productKeys = (*itr).second;
    itr ++;
    for(; itr != SKU.end(); itr++) {
        productKeys += "," + (*itr).second;
    }
    info["iapKeys"] = productKeys;
    s_protocolIAP->configDeveloperInfo(info);
#endif

#if COCOS2D_DEBUG
    s_protocolIAP->setDebugMode(true);
#endif
    
    _init = true;
}

void CentralClient::buyCharge(const int price, ProtocolIAP::ProtocolIAPCallback callback)
{
    TProductInfo info;
    const std::string IAPId = SKU.at(price);
    RestClient::PostData postData;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    postData["platform"] = "googleplay";
    info["IAPId"] = IAPId;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    postData["platform"] = "appstore";
    info["productId"] = IAPId;
#endif
    auto userToken = getUserToken();
    if(userToken.length() < 1){
        MessageBox("API エラーにより現在課金が行えません", "もうしわけございません");
        return;
    }

    postData["iap_id"] = IAPId;
    postData["token"] = userToken;
    postData["price"] = std::to_string(price);

    RestClient::getInstance()->post(API_URL + "/payments",  postData, [&, callback, info](HttpClient* client, HttpResponse* response){
        if(response->isSucceed()){
            auto v = *response->getResponseData();
            const std::string paymentId(v.begin(), v.end());
#if COCOS2D_DEBUG
            std::string msg = "";
            paymentSuccess(paymentId, callback, PayResultCode::kPaySuccess, msg);
            return;
#endif
            s_protocolIAP->payForProduct(info, [&, callback, paymentId, info](int status, std::string& msg){
                log("Payment result %i %s", status, msg.c_str());
//                s_protocolIAP->consumeForProduct(info);
                if(status == PayResultCode::kPaySuccess){
                    paymentSuccess(paymentId, callback, status, msg);
                }else{
                    callback(status, msg);
                }
            });
        }else{
            MessageBox("API エラーにより現在課金が行えません", "もうしわけございません");
        }
    });
}

void CentralClient::createUser()
{
    RestClient::PostData postData;
    postData["game_id"] = GAME_ID;
    RestClient::getInstance()->post(API_URL + "/players", postData, [&](HttpClient* client, HttpResponse* response){
        if(response->isSucceed()){
            auto v = *response->getResponseData();
            const std::string body(v.begin(), v.end());
            if(body.length() == 64){
                UserDefault::getInstance()->setStringForKey(PAYMENT_USER_TOKEN, body);
                setUserToken(body);
            }
        }
    });
}

void CentralClient::paymentSuccess(const std::string& paymentId, ProtocolIAP::ProtocolIAPCallback callback, int status, std::string& msg)
{
    RestClient::PostData postData;
    postData["receipt_data"] = msg;
    postData["token"] = getUserToken();
    auto url = API_URL + "/payments/" + paymentId;
    RestClient::getInstance()->put(url, postData, wrapCallback(callback));
}

void CentralClient::getCurrentCharge(ProtocolIAP::ProtocolIAPCallback callback)
{
    RestClient::getInstance()->get(API_URL + "/payments?token=" + getUserToken(), [&, callback](HttpClient* client, HttpResponse* response){
        auto status = response->getResponseCode();
        auto v = *response->getResponseData();
        std::string msg(v.begin(), v.end());
        setCurrentCharge(std::stoi(msg));
        callback(static_cast<int>(status), msg);
    });
}

void CentralClient::buyItem(const int price, const int itemId, ProtocolIAP::ProtocolIAPCallback callback)
{
    RestClient::PostData postData;
    postData["token"] = getUserToken();
    postData["amount"] = std::to_string(price);
    auto url = "/charges/" + std::to_string(itemId);
    const int currentCharge = getCurrentCharge();
    RestClient::getInstance()->destroy(url, postData, [&, callback, currentCharge, price](HttpClient* client, HttpResponse* response){
        auto status = response->getResponseCode();
        auto v = *response->getResponseData();
        if(status == 402){
            std::string msg = "";
            callback(static_cast<int>(status), msg);
            return;
        }
        std::string msg(v.begin(), v.end());
        const int charge = std::stoi(msg);
        setCurrentCharge(charge);
        if(currentCharge - price != charge){
            status = CentralClient::INVALID_ITEM_BOUGHT;
        }
        callback(static_cast<int>(status), msg);
    });
}

ccHttpRequestCallback CentralClient::wrapCallback(ProtocolIAP::ProtocolIAPCallback& callback)
{
    return [callback](HttpClient* client, HttpResponse* response){
        auto status = response->getResponseCode();
        auto v = *response->getResponseData();
        std::string msg(v.begin(), v.end());
        callback(static_cast<int>(status), msg);
    };
}

void CentralClient::postPlayLog(const std::string& data)
{
    RestClient::PostData postData;
    postData["token"] = getUserToken();
    RestClient::getInstance()->post("/play_logs", postData, [](HttpClient* client, HttpResponse* response){});
}

void CentralClient::backup()
{
    auto token = getUserToken();
    const std::string fpath =  token.substr(0, 3) + "/" + token + ".plist";
    auto futils = FileUtils::getInstance();
    const auto data = futils->getStringFromFile(getBackupFilePath());;
    const auto sha256 = SHA256Hash(data);;
    RestClient::PostData postData;
    postData["hashed_payload"] = sha256;
    postData["method"] = "PUT";
    postData["filename"] = fpath;
    const std::string url = API_URL + "/players/" + token + "/s3_header";
    RestClient::getInstance()->post(url, postData, [&, data, fpath](HttpClient* client, HttpResponse* response){
        if(response->getResponseCode() != 200){
            return;
        }
        auto v = *response->getResponseData();
        const std::string header(v.begin(), v.end());
        putToS3(fpath, data, header);
    });
}

void CentralClient::putToS3(const std::string& fpath, const std::string& data, const std::string& header)
{
    auto* request = new HttpRequest();
    request->setUrl(("https://oke-ya-central.s3.amazonaws.com" + fpath).c_str());
    request->setRequestType(HttpRequest::Type::PUT);
    request->setRequestData(data.c_str(), data.length());
    std::vector<std::string> headers;
    std::istringstream iss(header);
    std::string line;
    while(std::getline(iss, line)){
        headers.push_back(line);
    }
    request->setHeaders(headers);
    request->setResponseCallback([](HttpClient* client, HttpResponse* response){
        auto v = *response->getResponseData();
        const std::string body(v.begin(), v.end());
        log("PutToS3 finish %s", body.c_str());
    });
    HttpClient::getInstance()->send(request);
    request->release();
}

const std::string CentralClient::SHA256Hash(const std::string& key)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, key.c_str(), key.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    log("%s", ss.str().c_str());
    return ss.str();
}


void CentralClient::downloadAssets(const std::function<void(EventAssetsManagerEx* event)>& callback)
{
    std::stringstream ss;
    ss << "Manifests/";
    ss << GAME_ID;
    ss << ".project.";
#if TARGET_IPHONE_SIMULATOR
    ss << "l";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    ss << "a";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    ss << "i";
#endif
    
    ss << ".manifest";
    
#if TARGET_IPHONE_SIMULATOR
#elif COCOS2D_DEBUG
    ss << ".stg";
#else
    ss << ".prod";
#endif
    
    const std::string manifest = ss.str();
    log("manifest %s", manifest.c_str());
    auto destDir = getAssetDir();
    auto assetManager = AssetsManagerEx::create(manifest, destDir);
    assetManager->retain();
    auto eventListener = EventListenerAssetsManagerEx::create(assetManager, [=](EventAssetsManagerEx* event){
        auto eventCode = event->getEventCode();
        switch(eventCode){
            case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
            case EventAssetsManagerEx::EventCode::UPDATE_FINISHED: {
                addSpriteCaches();
            }
            default:{
            }
        }

        callback(event);
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(eventListener, 1);
    assetManager->update();
}

#include <dirent.h>
void CentralClient::addSpriteCaches()
{
    auto assetDir = getAssetDir();
    std::string cacheExt = ".plist";
    struct dirent *ent;
    DIR *dir;
    if((dir = opendir(assetDir.c_str())) != NULL){
        while((ent = readdir (dir)) != NULL){
            auto plist = std::string(ent->d_name);
            std::string::size_type index = plist.find_last_of(".");
            if(index == std::string::npos){
                continue;
            }
            auto extname = plist.substr(index);
            if(extname != cacheExt){
                continue;
            }
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist);
        }
        closedir(dir);
    }

}

void CentralClient::savePlayData(ValueMap& dict)
{
    FileUtils::getInstance()->writeToFile(dict, getBackupFilePath());
}

const ValueMap CentralClient::loadPlayData()
{
    return FileUtils::getInstance()->getValueMapFromFile(getBackupFilePath());
}

void CentralClient::printValueMap(cocos2d::ValueMap& vmap)
{
    auto v = Value(vmap);
    log("%s", valueToStr(v).c_str());
}

const std::string CentralClient::valueToStr(Value& value)
{
    std::stringstream ss;
    switch(value.getType()){
        case Value::Type::MAP :{
            ss << "{";
            auto& vmap = value.asValueMap();
            auto size = vmap.size();
            int i = 0;
            for(auto& v: vmap){
                ss << "\"" + v.first + "\"";
                ss << ": ";
                ss << valueToStr(v.second);
                i++;
                if(i < size){
                    ss << ", ";
                }
            }
            ss << "}";
            return ss.str();
            break;
        }
        case Value::Type::VECTOR :{
            ss << "[";
            auto& vec = value.asValueVector();
            auto size = vec.size();
            int i = 0;
            for(auto& v: vec){
                ss << valueToStr(v);
                i++;
                if(i < size){
                    ss << ", ";
                }
            }
            ss << "]";
            return ss.str();
            break;
        }
        case Value::Type::INTEGER :{
            ss << value.asInt();
            return ss.str();
            break;
        }
        case Value::Type::STRING :{
            ss << "\"";
            ss << value.asString();
            ss << "\"";
            return ss.str();
            break;
        }
        case Value::Type::NONE :{
            return "null";
        }
        default: {
            log("Not defined print type");
        }
    }

}

