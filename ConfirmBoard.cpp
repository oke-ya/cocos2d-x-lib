//
//  ConfirmBoard.cpp
//  witch
//
//  Created by Atsushi Yoshida on 2016/03/10.
//
//

#include "ConfirmBoard.h"
#include "CommonTypes.h"
#include "cocos/ui/CocosGUI.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "SupportFunctions.h"

using namespace cocostudio;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;

void ConfirmBoard::init()
{
    _board = CSLoader::createNode("Csbs/Common/ConfirmBoard.csb");
    _board->setPosition(supportfunctions::bottomLeft());
    _world->addChild(_board);
}

template <typename T>
void ConfirmBoard::setString(const std::string& nodeName, const std::string& msg)
{
    auto node = _board->getChildByName<T*>(nodeName);
    if(node){
        node->setString(msg);
    }
}

void ConfirmBoard::setTitle(const std::string& title)
{
    setString<TextBMFont>("Title", title);
}

void ConfirmBoard::setDescription(const std::string& description)
{
    setString<TextBMFont>("Description", description);
}

void ConfirmBoard::setMessage(const std::string& text)
{
    setString<Text>("Message", text);
}

void ConfirmBoard::onCancel(const Touchable::Callback& callback)
{
    onButtonPress("Cancel", callback);
}

void ConfirmBoard::onSubmit(const Touchable::Callback& callback)
{
    onButtonPress("Submit", callback);
}
