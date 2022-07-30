#include "GameObject.hpp"

#pragma once

enum class CountMode
{
	UP,
	DOWN
};

class Timer : public GameObject
{
public:
	inline static Timer* instance;

public:
	Timer();
	Timer(float time, CountMode mode = CountMode::DOWN);

	void SetTime(float time);

	void SetCountMode(CountMode mode);

	void Start();

	void Pause();

	void Resume();

	void Reset();

	UINT GetCurrentSecond();

	float GetCurrentSecondFloat();

private:
	float counter;
	float defaultTime;
	bool counting;
	CountMode countMode;

private:
	// GameObject を介して継承されました
	virtual void EventBeginPlay() override;

	virtual void EventTick(float deltaTime) override;

	virtual void EventKeyDown(KeyEvent e) override;

	virtual void EventKeyUp(KeyEvent e) override;

	virtual void EventMouseDown(MouseEvent e) override;

	virtual void EventMouseUp(MouseEvent e) override;

};

