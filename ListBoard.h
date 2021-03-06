#ifndef __okeya__ListBoard__
#define __okeya__ListBoard__

#include "cocos2d.h"
#include "Touchable.h"
#include "PopUp.h"

using namespace cocos2d;

class ListBoard : public PopUp
{
public:
    ListBoard(Node* node)
    : PopUp(node)
    {
        init();
    }
    virtual ~ListBoard() = default;
    void setTitle(const std::string& title);
    void setList(Node* node);
    void onClose(const Touchable::Callback& callback);
    inline void setBoxTemplate(Widget* node){ _boxTemplate = node; }

    template<typename T, typename ID_T>
    void render(std::unordered_map<ID_T, const T> datas, const std::vector<ID_T>& orderedIds, std::function<void(Widget* node, const ID_T dataId, const T& data)> callback)
    {
        auto tmpl = getBoxTemplate();
        resetList();
        for(auto& dataId : orderedIds){
            auto node = tmpl->clone();
            callback(node, dataId, datas.at(dataId));
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
            setList(node);
        }
        refreshList();
        show();
    };
    using Callback = std::function<void(const int)>;
    void onPressConfirm(const Callback& callback){ _onPressConfirm = callback; }
    void onPressDetail(const Callback& callback){ _onPressDetail = callback; }
private:
    void resetList();
    void refreshList();
    Callback _onPressConfirm;
    Callback _onPressDetail;
    inline Widget* getBoxTemplate() const { return _boxTemplate; }
    void onButtonPress(const std::string& name, const Touchable::Callback& callback);
    inline Node* getWorld(){ return _world; }
    void init();
    Widget* _boxTemplate{ nullptr };
};

#endif /* defined(__okeya__ListBoard__) */
