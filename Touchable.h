#ifndef Touchable_hpp
#define Touchable_hpp

#include "cocos2d.h"
#include "CocosGUI.h"
#include "NonCopyable.h"

USING_NS_CC;
using namespace cocos2d::ui;

class Touchable : NonCopyable<Touchable>
{
protected:
    static constexpr auto DISABLE_TOUCH_LAYER = "disableTouchLayer";
    typedef std::function<void(Ref* ref)> Callback;
    static const std::string LONG_TOUCH_KEY;
    static const float LONG_TOUCH_SEC;
    Touchable()
    : _dt(0.0)
    {
    }
    
    virtual ~Touchable()
    {
    }
    
    void onLongTouch(Widget* widget, const Callback& action);
    void onTouch(Widget* widget,  const Callback& action);
    void useTouchMaker(Node* node);
    void disableTouch(Layer* layer);
    void enableTouch(Layer* layer);
private:

    float _dt;
};

#endif /* Touchable_h */
