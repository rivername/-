#include "Gui.hpp"
#pragma once

/*==================================
   GUIやCanvas要素を複数持てるCanvas
==================================*/
class Canvas : public GameObject
{
public:
    Canvas();
    ~Canvas();

public:
    // Guiを追加
    // @param gui  : Gui
    // @param name : 名前
    void AddGui
    (
        Gui* gui,
        string  name
    );

    // Canvasを追加
    // @param canvas : Canvas
    // @param name   : 名前
    void AddCanvas
    (
        Canvas* canvas,
        string  name
    );

public:
    // Canvas用Update仮想関数
    virtual void Update() {}

public:
    // 解放する
    void Release();

public:
    // 表示する
    void Show();

    // 非表示にする
    void Hide();

    // 可視状態を切り替え
    void ToggleVisibility();

    // スケールを変更
    void SetScale(float scale);

    // 相対的に位置を設定
    void AddOffset(float x, float y);

    // オフセットをパーセントで適用
    void AddOffsetPercent(float percentX, float percentY);

    // Z-Indexを設定
    void SetZIndex(UINT z_index);

    void EnableInput();

    void DisableInput();

public:
    // 名前から子GUIを取得
    template <class GuiT> // 実装は下に記述あり
    GuiT* GetGuiChildByName(string name);

    // 名前から子Canvasを取得
    Canvas* GetCanvasChildByName(string name);

public:
    // GUI要素
    std::vector<std::pair<Gui*, string>> allChildGuiWithName;

    // Canvas要素
    std::vector<std::pair<Canvas*, string>> allChildCanvasWithName;

public:
    // 解放された
    bool released;

public:
    virtual void EventBeginPlay()             override {}
    virtual void EventTick(float deltaTime)   override;
    virtual void EventKeyDown(KeyEvent e)     override {}
    virtual void EventKeyUp(KeyEvent e)       override {}
    virtual void EventMouseDown(MouseEvent e) override {}
    virtual void EventMouseUp(MouseEvent e)   override {}
};

template<class GuiT>
inline GuiT* Canvas::GetGuiChildByName(string name)
{
    for (auto& pair : this->allChildGuiWithName)
    {
        if (pair.second == name)
        if (GuiT* child = dynamic_cast<GuiT*>(pair.first))
            return child;
    }

    return nullptr;
}
