#include "Alerm.h"

const int ONE_HOUR = 3600;
const int ONE_DAY = 24 * 3600;
const int ONE_MINUTE = 60;

void Alerm::start(Node* node)
{
    _dt = 0;
    _node = node;
    node->schedule([&](float dt){
        _dt += dt;
        if(_dt < 1.0){
            return;
        }
        _dt = 0;
        _remainingSec -= 1;
        render();
        if(_remainingSec < 1){
            _finishCallback();
            _node->unschedule(getName());
        }
    }, getName());
    render();
}

const int Alerm::getSec() const
{
    return _remainingSec - minSec();
}

const int Alerm::getMin() const
{
    return (_remainingSec - hourSec()) / 60;
}

const int Alerm::getHour() const
{
    return (_remainingSec - daySec()) / ONE_HOUR;
}

const int Alerm::getDay() const
{
    return _remainingSec / ONE_DAY;
}

const int Alerm::daySec() const
{
    return getDay() * ONE_DAY;
}

const int Alerm::hourSec() const
{
    return daySec() + getHour() * ONE_HOUR;
}

const int Alerm::minSec() const
{
    return hourSec() + getMin() * ONE_MINUTE;
}

void Alerm::render()
{
}
