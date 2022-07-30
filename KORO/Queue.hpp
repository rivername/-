#include <functional>
#include "utils.hpp"
#pragma once

/*==================================
         負担軽減処理用実行キュー
==================================*/
namespace queue
{
    // 関数リスト
    inline std::vector<std::function<void()>> list;

    // Step
    inline UINT step = 1U;

    // 関数を追加
    inline void add_function(std::function<void()> function)
    {
        list.push_back(function);
    }

    // キューを実行
    inline void excute_queue()
    {
        if (list.empty()) return;

        for (UINT i = 0; i < step; i++)
        {
            if (list.empty()) break;

            // 実行
            list[0]();

            // キューリストから削除
            list.erase(list.begin());
        }
    }
}

class Queue
{
public:
    static Queue& instance()
    {
        static Queue q;
        return q;
    }

public:
    void operator<<(std::function<void()> function)
    {
        queue::add_function(function);
    }
};

// キュー
#define QUEUE Queue::instance()

// キュー取り出しのStepを設定
#define QUEUE_SET_STEP(...) \
    QUEUE << [&]() \
    { \
        queue::step = __VA_ARGS__; \
    };
