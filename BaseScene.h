#ifndef __okeya__BaseScene__
#define __okeya__BaseScene__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "lib/Touchable.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include <sstream>
#include <typeinfo>
#include <regex>

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;

template <typename T>
class BaseScene : public Layer, protected Touchable
{
public:
    virtual ~BaseScene(){}
    static cocos2d::Scene* createScene()
    {
        auto scene = Scene::create();
        auto layer = T::create();
        scene->addChild(layer);
        return scene;
    }
    
protected:
    Node* _csb;
    inline Node* getCsb() const { return _csb; }
    
    static T* create()
    {
        T *pRet = new(std::nothrow) T();
        if (pRet && pRet->setCsb() && pRet->init())
        {
            pRet->autorelease();
            return pRet;
        }
        else
        {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
    
    bool setCsb(){
        const auto numPrefix = std::regex("^[0-9]+");
        auto basename = std::string(typeid(T).name());
        auto name = std::regex_replace( basename, numPrefix, "") ;
        std::stringstream ss;
        ss << "Csbs/";
        ss << name;
        ss << ".csb";
        _csb = CSLoader::createNode(ss.str());
        this->addChild(_csb);
        useTouchMaker(this);
        return true;
    }
};


#endif /* defined(__okeya__BaseScene__) */
