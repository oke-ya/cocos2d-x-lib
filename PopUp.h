#ifndef __okeya__PopUp__
#define __okeya__PopUp__

#include "cocos2d.h"
#include "Touchable.h"

using namespace cocos2d;

class PopUp : protected Touchable
{
public:
    PopUp(Node* node)
    : _world(node)
    {
    }
    virtual ~PopUp()
    {
        if(_board){
            _board->removeFromParent();
            _board = nullptr;
        }
        log("desturct PopUp");
    }
    inline void hide(){ _board->setScale(0.0f); }
    inline void show(){ _board->setScale(1.0f); }
protected:
    inline Node* getWorld(){ return _world; }
    using Callback = std::function<void(const int)>;
    void onButtonPress(const std::string& name, const Touchable::Callback& callback)
    {
        auto button = _board->getChildByName<Button*>(name);
        onTouch(button, [&, callback](Ref* ref){
            callback(ref);
            hide();
        });
    }

    Node* _world;
    Node* _board;
};

#endif /* defined(__okeya__PopUp__) */
