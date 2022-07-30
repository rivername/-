#pragma once

/*==================================
             イベントクラス
==================================*/
class Event
{
public:
    // イベントクラス
    // @param eventname : イベント名
    Event(string eventname) : eventname(eventname)
    {
    }

    ~Event()
    {
    }

public:
    friend void operator<<(Event* event, std::function<void()> function)
    {
        if (event != nullptr)
            event->functions.push_back(function);
    }

public:
    // イベント名
    string eventname;

    // イベント発生時の実行
    std::vector<std::function<void()>> functions;
};

/*==================================
            イベントハンドラ
==================================*/
namespace EventHandler
{
    using SP        = std::unique_ptr<Event>;
    using EventList = std::vector<SP>;
    using Function  = std::function<void()>;

    // イベントリスト
    inline EventList& AllEvent()
    {
        static EventList list;
        return list;
    }

    // イベント名からイベントクラスを取得
    // @param eventname : イベント名
    inline Event* GetEventByEventName(string eventname)
    {
        for (auto& e : AllEvent())
        {
            if (e->eventname == eventname) return e.get();
        }

        return nullptr;
    }

    // イベントを登録
    // @param eventname : イベント名
    inline Event* RegisterEvent(string eventname)
    {
        auto event = new Event(eventname);
        AllEvent().push_back(SP(event));
        return event;
    }

    // イベントを呼び出し
    // @param eventname : イベント名
    inline bool Call(string eventname)
    {
        if (GetEventByEventName(eventname) != nullptr)
        {
            for (auto& func : GetEventByEventName(eventname)->functions)
            {
                func();
            }

            return true;
        }

        return false;
    }

    // イベントリスナを追加
    // @param eventname : イベント名
    // @param function  : 実行
    inline bool AddListenner(string eventname, Function function)
    {
        if (GetEventByEventName(eventname) != nullptr)
        {
            GetEventByEventName(eventname) << function;
            return true;
        }

        return false;
    }
}

#define EVENT(eventname) EventHandler::GetEventByEventName(eventname)

/*
if (e.keyCode == 'R')
{
    EventHandler::RegisterEvent("rbtn");
}

if (e.keyCode == 'I')
{
    EVENT("rbtn") << [&]()
    {
        auto loc = this->GetTransform().location;
        MessageBox(std::format("{} / {} / {}", loc.x, loc.y, loc.z));
    };
}

if (e.keyCode == 'C')
{
    EventHandler::Call("rbtn");
}
*/
