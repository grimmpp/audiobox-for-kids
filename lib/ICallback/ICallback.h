#include <Arduino.h>

#ifndef ICallback_H
#define ICallback_H

class ICallback
{
    public:
        virtual ~ICallback() {}
        virtual void notify(uint8_t track) = 0;
};

#endif