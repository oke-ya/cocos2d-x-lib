#ifndef __okeya__PageBoard__
#define __okeya__PageBoard__

#include "cocos2d.h"
#include "Touchable.h"
#include "PopUp.h"

using namespace cocos2d;

class PageBoard : public PopUp
{
public:
    PageBoard(Node* node)
    : PopUp(node)
    , _sctollToPage(0)
    {
        init();
    }
    virtual ~PageBoard() = default;
    void setTitle(const std::string& title);
    void setPage(Layout* page);
    void onClose(const Touchable::Callback& callback);
    inline void setBoxTemplate(Widget* node){ _boxTemplate = node; }

    template<typename T, typename ID_T>
    void render(std::unordered_map<ID_T, const T> datas, const std::vector<ID_T>& orderedIds, const Vec2& graphSize, std::function<void(Widget* node, const ID_T dataId, const T& data)> callback)
    {
        int x = 0;
        int y = 0;
        auto page = Layout::create();
        auto tmpl = getBoxTemplate();
        auto size = tmpl->getContentSize();
        resetPage();
        for(auto& dataId : orderedIds){
            auto node = tmpl->clone();
            auto posY = (graphSize.y - 1) - y;
            auto pos = Vec2(x * size.width, posY * size.height);
            node->setPosition(pos);
            callback(node, dataId, datas.at(dataId));
            page->addChild(node);
            auto confirm = node->getChildByName<ImageView*>("BgImage");
            if (confirm) {
                onTouch(confirm, [&, dataId](Ref* ref){
                    _onPressConfirm(static_cast<int>(dataId));
                });
            }
            auto detail = node->getChildByName<Button*>("Detail");
            if (detail) {
                detail->setLocalZOrder(confirm->getLocalZOrder() + 2);
                onTouch(detail, [&, dataId](Ref* ref){
                    _onPressDetail(static_cast<int>(dataId));
                });
            }
            x++;
            if(x >= graphSize.x){
                x = 0;
                y ++;
            }
            if(y >= graphSize.y){
                setPage(page);
                page = Layout::create();
                x = 0;
                y = 0;
            }
        }
        if(x > 0 || y > 0){
            setPage(page);
        }
        setPager();
        show();
    };
    void onPressConfirm(const Callback& callback){ _onPressConfirm = callback; }
    void onPressDetail(const Callback& callback){ _onPressDetail = callback; }
    void setScrollToPage(const int scrollToPage);
private:
    inline PageView* getPageView() const { return _pageView; }
    void resetPage();
    Callback _onPressConfirm{ [](const int itemId){ log("Not implement onPressConfirm."); } };
    Callback _onPressDetail{ [](const int itemId){ log("Not implement onPressDetail."); } };
    inline Widget* getBoxTemplate() const { return _boxTemplate; }
    inline Node* getWorld(){ return _world; }
    void renderPageNumber(PageView* pageView);
    void setPager();
    void init();
    Widget* _boxTemplate{ nullptr };
    PageView* _pageView;
    int _sctollToPage;
};

#endif /* defined(__okeya__PageBoard__) */
