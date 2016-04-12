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
    virtual ~BaseScene() = default;
    static cocos2d::Scene* createScene()
    {
        auto scene = Scene::create();
        auto layer = T::create();
        scene->addChild(layer);
        return scene;
    }
    
protected:
    Node* _csb{ nullptr };
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
        const auto numPrefix = std::regex("^.*[0-9]+");
        const auto basename = std::string(typeid(T).name());
        const auto classname = std::regex_replace( basename, numPrefix, "") ;
        std::string name;
        if(basename.substr(0, 1) ==  "N"){
            name = classname.substr(0, classname.size() - 1);
        }else{
            name = classname;
        }
        std::stringstream ss;
        ss << "Csbs/";
        ss << name;
        ss << ".csb";
        const auto fname = ss.str();
        if(FileUtils::getInstance()->isFileExist(fname)){
            _csb = CSLoader::createNode(fname);
            this->addChild(_csb);
        }
        useTouchMaker(this);
        return true;
    }
};

template <typename T>
class PhysicsBaseScene : public BaseScene<T>
{
public:
    virtual ~PhysicsBaseScene() = default;
    static cocos2d::Scene* createScene()
    {
        auto scene = Scene::createWithPhysics();
        auto layer = T::create();
        scene->addChild(layer);
        return scene;
    }
};



#endif /* defined(__okeya__BaseScene__) */
