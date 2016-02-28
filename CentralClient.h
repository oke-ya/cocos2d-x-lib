#ifndef __okeya__CentralClient__
#define __okeya__CentralClient__

#include "ProtocolIAP.h"
#include "PluginManager.h"
#include "network/HttpRequest.h"
#include "extensions/cocos-ext.h"
#include "platform/CCFileUtils.h"
#include <ctime>

using namespace cocos2d;
using namespace cocos2d::plugin;
using namespace cocos2d::network;
using namespace cocos2d::extension;

class CentralClient
{
public:
    CentralClient();
    static const std::string API_URL;
    static const int ChargeNone;
    static const int INVALID_ITEM_BOUGHT;
    static CentralClient* getInstance();
    void init();
    void buyCharge(const int price, ProtocolIAP::ProtocolIAPCallback callback);
    void getCurrentCharge(ProtocolIAP::ProtocolIAPCallback callback);
    void buyItem(const int price, const int itemId, ProtocolIAP::ProtocolIAPCallback callback);
    void postPlayLog(const std::string& data);
    inline int getCurrentCharge() const { return _currentCharge; }
    inline void setCurrentCharge(const int i){ _currentCharge = i; }
    void backup();
    inline const std::string getUserToken(){ return _userToken; }
    void downloadAssets(const std::function<void(EventAssetsManagerEx* event)>& callback);
    void savePlayData(ValueMap& dict);
    const ValueMap loadPlayData();
    
    inline const std::string getBackupFilePath(){
        return FileUtils::getInstance()->getWritablePath() + "/backup.plist";
    }
private:
    inline const std::string getAssetDir(){ return FileUtils::getInstance()->getWritablePath() + "/Reources"; }
    void addSpriteCaches();
    void putToS3(const std::string& fpath, const std::string& data, const std::string& header);
    ccHttpRequestCallback wrapCallback(ProtocolIAP::ProtocolIAPCallback& callback);
    inline void setUserToken(const std::string& userToken){ _userToken = userToken; }
    void paymentSuccess(const std::string& paymentId, ProtocolIAP::ProtocolIAPCallback callback, int status, std::string& msg);
    void createUser();
    static CentralClient* _singleton;
    int _currentCharge;
    bool _init;
    ProtocolIAP* s_protocolIAP;
    std::string _userToken;
public:
    const std::string SHA256Hash(const std::string& key);
};

#endif /* defined(__okeya__CentralClient__) */
