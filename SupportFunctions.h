#ifndef SupportFunctions_h
#define SupportFunctions_h

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
#include <spine/SkeletonAnimation.h>

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace spine;

namespace supportfunctions{
    static inline const Vec2 middleCenter()
    {
        auto winSize = Director::getInstance()->getWinSize();
        return Vec2(winSize.width / 2, winSize.height / 2);
    }

    static inline SkeletonAnimation* createSpineAnimation(const std::string& fname)
    {
        return SkeletonAnimation::createWithFile("Animations/" + fname + ".json", "Animations/" + fname + ".atlas");
    }

    static inline Button* createButton(const std::string& fname)
    {
         return Button::create("Images/" + fname + "_n.png", "Images/" + fname + "_p.png", "Images/" + fname + "_d.png");
    }

    template <typename SceneT, typename TransitionT>
    static void transition(float duration)
    {
        auto scene = SceneT::createScene();
        auto transition = TransitionT::create(duration, scene);
        Director::getInstance()->replaceScene(transition);
    }
}

#endif /* SupportFunctions_h */
