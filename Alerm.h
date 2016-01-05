#ifndef Alerm_hpp
#define Alerm_hpp

#include "cocos2d.h"

USING_NS_CC;

class Alerm
{
public:
    virtual ~Alerm()
    {
    }
    inline void setRemainningSec(const int sec){ _remainingSec = sec; }
    void start(Node* node);
    virtual void render();

    const int getSec() const;
    const int getMin() const;
    const int getHour() const;
    const int getDay() const;
    const int daySec() const;
    const int hourSec() const;
    const int minSec() const;
    inline void setFinishCallback(const std::function<void(void)> fn){ _finishCallback = fn; }
protected:
    Node* _node;
    std::function<void(void)> _finishCallback;
private:
    inline const std::string getName(){ return _node->getName() + "alerm"; };
    int _remainingSec;
    float _dt;
};


#endif /* Alerm_hpp */
