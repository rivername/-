#include "Timer.hpp"
#include "Debug.hpp"
#include "Window.hpp"
#include "Game.hpp"

Timer::Timer()
{
	Timer::instance = this;

	this->counter = 0.0;
	this->defaultTime = 0.0;
	this->counting = false;
	this->countMode = CountMode::UP;
}

Timer::Timer(float time, CountMode mode)
{
	Timer::instance = this;

	this->counter = time;
	this->defaultTime = time;
	this->counting = false;
	this->countMode = mode;
}

void Timer::SetTime(float time)
{
	this->counter = time;
	this->defaultTime = time;
	this->counting = false;
	this->countMode = CountMode::DOWN;
}

void Timer::SetCountMode(CountMode mode)
{
	this->countMode = mode;
}

void Timer::Start()
{
	this->counter = this->defaultTime;
	this->counting = true;
}

void Timer::Pause()
{
	this->counting = false;
}

void Timer::Resume()
{
	this->counting = true;
}

void Timer::Reset()
{
	this->counter = this->defaultTime;
	this->counting = false;
}

UINT Timer::GetCurrentSecond()
{
	return static_cast<UINT>(ceilf(counter));
}

float Timer::GetCurrentSecondFloat()
{
	return counter;
}

void Timer::EventBeginPlay()
{
}

void Timer::EventTick(float deltaTime)
{
    if (Game::instance->gameStatus == GameStatus::PAUSING) return;

	if (this->counting)
    {
		if (this->countMode == CountMode::UP)
        {
			this->counter += deltaTime;
		} 
		else
		{
			this->counter -= deltaTime;

			if (this->counter <= 0)
            {
				this->counter = 0;

				this->counting = false;
			}
		}
	}

	Debug::AddLine(" {} {} ", this->counter, deltaTime);
	Debug::AddLine(" {} ", this->GetCurrentSecond());
}

void Timer::EventKeyDown(KeyEvent e)
{
}

void Timer::EventKeyUp(KeyEvent e)
{
}

void Timer::EventMouseDown(MouseEvent e)
{
}

void Timer::EventMouseUp(MouseEvent e)
{
}
