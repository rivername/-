#include "GameObject.hpp"
#include "Game.hpp"
#include <functional>
#pragma once

using ULONGLONG = unsigned long long;

class Delay : public GameObject
{
public:
    Delay(std::function<void()> function, ULONGLONG duration_ms)
    {
        this->current_tick = 0ULL;
        this->function = function;
        this->duration = duration_ms * 60 / 1000;
    }

private:
    ULONGLONG current_tick;
    std::function<void()> function;
    ULONGLONG duration;

public:
    virtual void EventBeginPlay() override {}
    virtual void EventTick(float deltaTime) override
    {
        IF_PAUSE_RETURN;

        if (this->function != nullptr)
        if (this->current_tick > this->duration)
        {
            this->function();
            this->function = nullptr;
            this->Release();
        }

        this->current_tick++;
    }
    virtual void EventKeyDown(KeyEvent e) override {}
    virtual void EventKeyUp(KeyEvent e) override {}
    virtual void EventMouseDown(MouseEvent e) override {}
    virtual void EventMouseUp(MouseEvent e) override {}
};

#define DELAY new Delay
