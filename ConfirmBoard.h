//
//  ConfirmBoard.h
//  witch
//
//  Created by Atsushi Yoshida on 2016/03/10.
//
//

#ifndef __witch__ConfirmBoard__
#define __witch__ConfirmBoard__

#include "cocos2d.h"
#include "PopUp.h"

using namespace cocos2d;

class ConfirmBoard : public PopUp
{
public:
    virtual ~ConfirmBoard() = default;
    ConfirmBoard(Node* node)
    : PopUp(node)
    {
        init();
    }
    virtual void setTitle(const std::string& title);
    virtual void setDescription(const std::string& description = "");
    virtual void setMessage(const std::string& text);
    virtual void onCancel(const Touchable::Callback& callback);
    virtual void onSubmit(const Touchable::Callback& callback);
protected:
    template <typename T>
    void setString(const std::string& nodeName, const std::string& msg);
    void init();
};

#endif /* defined(__witch__ConfirmBoard__) */
