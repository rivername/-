#include "Canvas.hpp"
#include "Game.hpp"

Canvas::Canvas() : GameObject(ObjectType::CANVAS), released(false)
{
}

Canvas::~Canvas()
{
}

void Canvas::AddGui(Gui* gui, string name)
{
    this->allChildGuiWithName.push_back({ gui, name });
}

void Canvas::AddCanvas(Canvas* canvas, string name)
{
    this->allChildCanvasWithName.push_back({ canvas, name });
}

void Canvas::Release()
{
    if (this->released) return;

    // GUI解放
    for (auto& pair : this->allChildGuiWithName)
    {
        if (!pair.first->released)
        {
            pair.first->Release();
        }
    }

    // Canvas解放
    for (auto& pair : this->allChildCanvasWithName)
    {
        pair.first->Release();
    }

    this->released = true;
}

void Canvas::Show()
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->visible = true;
    for (auto& pair : this->allChildCanvasWithName) pair.first->Show();
}

void Canvas::Hide()
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->visible = false;
    for (auto& pair : this->allChildCanvasWithName) pair.first->Hide();
}

void Canvas::ToggleVisibility()
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->visible = !pair.first->visible;
    for (auto& pair : this->allChildCanvasWithName) pair.first->ToggleVisibility();
}

void Canvas::SetScale(float scale)
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->scale = scale;
    for (auto& pair : this->allChildCanvasWithName) pair.first->SetScale(scale);
}

void Canvas::AddOffset(float x, float y)
{
    for (auto& e : this->allChildGuiWithName)
    {
        e.first->position.x += x;
        e.first->position.y += y;
    }

    for (auto& pair : this->allChildCanvasWithName) pair.first->AddOffset(x, y);
}

void Canvas::AddOffsetPercent(float percentX, float percentY)
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->AddOffsetPercent(percentX, percentY);
    for (auto& pair : this->allChildCanvasWithName) pair.first->AddOffsetPercent(percentX, percentY);
}

void Canvas::SetZIndex(UINT z_index)
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->z_index = z_index;
    for (auto& pair : this->allChildCanvasWithName) pair.first->SetZIndex(z_index);
}

void Canvas::EnableInput()
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->EnableInput();
    for (auto& pair : this->allChildCanvasWithName) pair.first->EnableInput();
    GameObject::EnableInput();
}

void Canvas::DisableInput()
{
    for (auto& pair : this->allChildGuiWithName)    pair.first->DisableInput();
    for (auto& pair : this->allChildCanvasWithName) pair.first->DisableInput();
    GameObject::DisableInput();
}

Canvas* Canvas::GetCanvasChildByName(string name)
{
    for (auto& pair : this->allChildCanvasWithName)
    {
        if (pair.second == name) return pair.first;
    }

    return nullptr;
}

void Canvas::EventTick(float deltaTime)
{
    IF_PAUSE_RETURN;

    if (!this->released)
    {
        this->Update();
    }
}
