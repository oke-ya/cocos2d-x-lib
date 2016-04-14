#include "ListBoard.h"
#include "cocos/ui/CocosGUI.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "SupportFunctions.h"

using namespace cocostudio;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;

void ListBoard::init()
{
    _board = CSLoader::createNode("Csbs/ListBoard.csb");
    _board->setPosition(supportfunctions::bottomLeft());
    _world->addChild(_board);
    hide();
}

void ListBoard::resetList()
{
    auto listView = _board->getChildByName<ListView*>("ListView");
    listView->removeAllItems();
}

void ListBoard::refreshList()
{
    auto listView = _board->getChildByName<ListView*>("ListView");
    listView->setBounceEnabled(true);
    listView->setScrollBarEnabled(false);
    listView->forceDoLayout();
}

void ListBoard::setTitle(const std::string& title)
{
    std::stringstream titleSS;
    titleSS << title;
    _board->getChildByName<TextBMFont*>("Title")->setString(titleSS.str());
}

void ListBoard::setList(Node* node)
{
    auto listView = _board->getChildByName<ListView*>("ListView");
    listView->addChild(node);
}

void ListBoard::onClose(const Touchable::Callback& callback)
{
    onButtonPress("Close", callback);
}

void ListBoard::onButtonPress(const std::string& name, const Touchable::Callback& callback)
{
    auto button = _board->getChildByName(name)->getChildByName<Button*>("Button");
    onTouch(button, [&, callback](Ref* ref){
        callback(ref);
        hide();
    });
}
