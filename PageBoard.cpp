#include "PageBoard.h"
#include "cocos/ui/CocosGUI.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "SupportFunctions.h"

using namespace cocostudio;
using namespace cocos2d::ui;
using namespace cocos2d::experimental;

void PageBoard::init()
{
    _board = CSLoader::createNode("Csbs/PageBoard.csb");
    _pageView = _board->getChildByName<PageView*>("PageView");
    _board->setPosition(supportfunctions::middleCenter());
    _world->addChild(_board);
    hide();
}

void PageBoard::setTitle(const std::string& title)
{
    std::stringstream titleSS;
    titleSS << title;
    _board->getChildByName<TextBMFont*>("Title")->setString(titleSS.str());
}

void PageBoard::resetPage()
{
    getPageView()->removeAllPages();
}

void PageBoard::setPage(Layout* page)
{
    auto pageView = getPageView();
    pageView->setBounceEnabled(true);
    pageView->addPage(page);
}

void PageBoard::onClose(const Touchable::Callback& callback)
{
    onButtonPress("Close", callback);
}

void PageBoard::setPager()
{
    auto pageView = getPageView();
    pageView->setCurrentPageIndex(pageView->getItems().size() - 1);
    pageView->scrollToItem(0);
    renderPageNumber(pageView);
    static_cast<ListView*>(pageView)->addEventListener([&](Ref* ref, ScrollView::EventType eventType){
        if(eventType == ScrollView::EventType::CONTAINER_MOVED){
            renderPageNumber(static_cast<PageView*>(ref));
        }
    });

    auto prev = _board->getChildByName<Button*>("Prev");
    onTouch(prev, [&, pageView](Ref* ref){
        pageView->scrollToItem(pageView->getCurrentPageIndex() - 1);
    });
    auto next = _board->getChildByName<Button*>("Next");
    onTouch(next, [&, pageView](Ref* ref){
        pageView->scrollToItem(pageView->getCurrentPageIndex() + 1);
    });
}

void PageBoard::renderPageNumber(PageView* pageView)
{
    auto pages = pageView->getItems();
    auto currentPage = pageView->getCurrentPageIndex() + 1;
    auto maxPage = pages.size();
    std::stringstream pageSS;
    pageSS << currentPage;
    pageSS << "/";
    pageSS << maxPage;
    auto prev = _board->getChildByName<Button*>("Prev");
    auto next = _board->getChildByName<Button*>("Next");
    if (currentPage <= 1) {
        prev->setVisible(false);
    } else {
        prev->setVisible(true);
    }
    if (currentPage >= maxPage) {
        next->setVisible(false);
    } else {
        next->setVisible(true);
    }
    auto num = _board->getChildByName<TextBMFont*>("PageNum");
    num->setString(pageSS.str());
}
