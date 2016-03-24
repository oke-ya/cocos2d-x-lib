#include "Touchable.h"

const std::string Touchable::LONG_TOUCH_KEY = "longTouch";
const float Touchable::LONG_TOUCH_SEC = 1.0f;
void Touchable::onLongTouch(Widget* widget, const Callback& action)
{
    widget->setTouchEnabled(true);
    widget->addTouchEventListener([&, action](Ref* ref, Widget::TouchEventType eventType){
        auto widget = static_cast<Widget*>(ref);
        if(eventType == Widget::TouchEventType::BEGAN){
            widget->schedule([&, ref, action](float dt){
                _dt += dt;
                if(_dt > Touchable::LONG_TOUCH_SEC){
                    _dt = 0;
                    action(ref);
                    static_cast<Widget*>(ref)->unschedule(Touchable::LONG_TOUCH_KEY);
                }
            }, Touchable::LONG_TOUCH_KEY);
        }
        if(eventType == Widget::TouchEventType::ENDED){
            static_cast<Widget*>(ref)->unschedule(Touchable::LONG_TOUCH_KEY);
            _dt = 0;
        }
    });
}

void Touchable::onTouch(Widget* widget,  const Callback& action)
{
    widget->setTouchEnabled(true);
    widget->addTouchEventListener([widget, action](Ref* ref, Widget::TouchEventType eventType){
        if(eventType == Widget::TouchEventType::ENDED){
            action(ref);
        }
    });
}

void Touchable::disableTouch(Layer* layer)
{
    auto disable = Layout::create();
    disable->setTouchEnabled(true);
    disable->setContentSize(Director::getInstance()->getWinSize());
    disable->addTouchEventListener([](Ref* ref,Widget::TouchEventType eventType){
        log("disable touch");
    });
    disable->setName(DISABLE_TOUCH_LAYER);
    layer->addChild(disable);
}

void Touchable::enableTouch(Layer* layer)
{
    while(auto disable = layer->getChildByName(DISABLE_TOUCH_LAYER)){
        disable->removeFromParent();
    }
}

void Touchable::useTouchMaker(Node* node)
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* touch, Event*event){
        auto particle = ParticleSystemQuad::create("Particles/touch.plist");
        particle->setPosition(touch->getLocation());
        event->getCurrentTarget()->addChild(particle);
        particle->setAutoRemoveOnFinish(true);
        return true;
    };
    auto layer = Layer::create();
    node->addChild(layer);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);
}
