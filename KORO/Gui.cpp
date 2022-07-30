#include "Gui.hpp"
#include "Window.hpp"
#include "Game.hpp"

std::vector<std::vector<HLSLObject*>> Gui::GetSortedByZIndexList()
{
    // リストinリスト
    std::vector<std::vector<HLSLObject*>> list;

    // 領域確保
    list.resize(MAX_Z_INDEX_i);

    for (auto& gameObject : GameObject::all)
    {
        if (auto gui = dynamic_cast<Gui*>(gameObject.get()))
        {
            // 可視と解放されていないものを追加
            if (!gui->visible)  continue;
            if ( gui->released) continue;

            // フォントGUIの場合
            if (auto fontGui = dynamic_cast<FontGui*>(gui))
            {
                if (fontGui->renderItems.empty()) continue;
            }

            // 対応するZ-Indexのリストに追加
            UINT i = (MAX_Z_INDEX_i - gui->z_index - 1);
            list[i].push_back(gui);
        }
    }

    return list;
}

const float Gui::GetScalingValue()
{
    return (Window::instance->width / SCREEN_SIZE_TO_FIT.x);
}

Gui::Gui(D3DXVECTOR2 position, ULONG width, ULONG height, D3DXVECTOR2 pivotOffset)
    : GameObject(ObjectType::GUI),
    rotationDeg(0.0f), z_index(4), color({ 1.0f, 1.0f, 1.0f, 1.0f }),
    visible(true), inputState({}), released(false)
{
    // %による位置とスケール設定
    this->position.x = Gui::GetPosXFromPercent(position.x);
    this->position.y = Gui::GetPosYFromPercent(position.y);

    // ウィンドウサイズによるスケーリング値
    float scaling = Gui::GetScalingValue();
    
    // スケーリングされた値を設定
    this->scale        = 1.0f        * scaling;
    this->width        = width       * scaling;
    this->height       = height      * scaling;
    this->pivotOffset  = pivotOffset * scaling;

    // 初期値を設定
    this->initialPos   = this->position;
    this->initialScale = this->scale;
}

void Gui::SetParamOnce(ID3DXEffect* effect)
{
    // 射影行列を設定
    this->GetProjMatrix(&this->hlslParam.mat_Proj);
    effect->SetMatrix("mat_Proj", &this->hlslParam.mat_Proj);

    // Mainテクニックを使用
    effect->SetTechnique("Main");
}

void Gui::Release()
{
    if (this->released) return;

    this->visible  = false;
    this->released = true;
}

void Gui::GetWorldMatrix(D3DXMATRIX* pOut)
{
    D3DXMATRIX mat_Position, mat_RotationZ, mat_Scale;

    // 原点（スクリーン中央）を基準にした位置を設定
    float x = this->position.x + this->pivotOffset.x;
    float y = this->position.y + this->pivotOffset.y;

    // 行列作成
    D3DXMatrixTranslation(&mat_Position, x, y, 0.0f);
    D3DXMatrixRotationZ(&mat_RotationZ, D3DXToRadian(this->rotationDeg));
    D3DXMatrixScaling(&mat_Scale, this->scale, this->scale, 1.0f);

    *pOut = (mat_Scale * mat_RotationZ * mat_Position);
}

void Gui::GetProjMatrix(D3DXMATRIX* pOut)
{
    // 2D描画用の射影行列を作成
    // デフォルトでは画面の中心が(0, 0)なので左上が0になるように作成
    // 参考 : http://marupeke296.com/NGDV_No3_Sprite.html
    D3DXMatrixIdentity(pOut);
    pOut->_11 = ( 2.0f / Window::instance->width);
    pOut->_22 = (-2.0f / Window::instance->height);

    pOut->_41 = -1.0f;
    pOut->_42 =  1.0f;
}

float Gui::GetPosXFromPercent(float percent)
{
    return Window::instance->width * (percent / 100.0f);
}

float Gui::GetPosYFromPercent(float percent)
{
    return Window::instance->height * (percent / 100.0f);
}

D3DXVECTOR2 Gui::GetPosFromPercent(float parcentX, float parcentY)
{
    return D3DXVECTOR2(Gui::GetPosXFromPercent(parcentX),
                       Gui::GetPosYFromPercent(parcentY));
}

D3DXVECTOR2 Gui::GetPosPercentFromPosition(float posX, float posY)
{
    return D3DXVECTOR2(posX / Window::instance->width  * 100.0f,
                       posY / Window::instance->height * 100.0f);
}

D3DXVECTOR2 Gui::GetPercentPos()
{
    return D3DXVECTOR2(this->position.x / Window::instance->width  * 100.0f,
                       this->position.y / Window::instance->height * 100.0f);
}

void Gui::SetPositionByPercent(float percentX, float percentY)
{
    this->position.x = Gui::GetPosXFromPercent(percentX);
    this->position.y = Gui::GetPosYFromPercent(percentY);
}

void Gui::AddOffsetPercent(float percentX, float percentY)
{
    if (percentX >= 0)
    {
        this->position.x += this->GetPosXFromPercent(percentX);
    }
    else
    {
        this->position.x -= this->GetPosXFromPercent(fabs(percentX));
    }
    
    if (percentY >= 0)
    {
        this->position.y += this->GetPosYFromPercent(percentY);
    }
    else
    {
        this->position.y -= this->GetPosYFromPercent(fabs(percentY));
    }
}

void Gui::AddOffsetPercentFromInitialPosition(float percentX, float percentY)
{
    this->position.x = this->initialPos.x + Gui::GetPosXFromPercent(percentX);
    this->position.y = this->initialPos.y + Gui::GetPosYFromPercent(percentY);
}

void Gui::AddScaleValueFromInitialScale(float value)
{
    this->scale = this->initialScale + value;
}

void Gui::SetScaleValueFromInitialScale(float value)
{
    this->scale = this->initialScale * value;
}

void Gui::EventListener()
{
    if (!Game::LISTEN_EVENT_ROOT) return;
    if (!this->visible)           return;

    RECT rc =
    {
        0,
        0,
        static_cast<LONG>(Window::instance->width),
        static_cast<LONG>(Window::instance->height)
    };

    // 画面外にいるときはイベント取得しない
    if (this->position.x < rc.left || this->position.x >= rc.right)  return;
    if (this->position.y < rc.top  || this->position.y >= rc.bottom) return;

    // FontGuiの場合無効なテキストボタンはイベント取得しない
    if (auto fontGui = dynamic_cast<FontGui*>(this))
    {
        if ( fontGui->isTextButton)
        if (!fontGui->isValidTextButton)
            return;
    }

    // マウス座標
    auto cursor = Window::instance->GetMouseCursorPos();

    // マウス座標をディスプレイのサイズでスケーリング
    float mouseX = cursor.x * (SCREEN_SIZE_TO_FIT.x / GetSystemMetrics(SM_CXSCREEN));
    float mouseY = cursor.y * (SCREEN_SIZE_TO_FIT.y / GetSystemMetrics(SM_CYSCREEN));

    // マウスホバー状態
    this->inputState.mouseOver =
        (mouseX >= this->position.x - this->width  * 0.5f && mouseX <= this->position.x + this->width  * 0.5f &&
         mouseY >= this->position.y - this->height * 0.5f && mouseY <= this->position.y + this->height * 0.5f);

    // マウスホバー状態に変化があったら
    if (this->inputState.mouseOverPrev != this->inputState.mouseOver)
    {
        // イベント発生
        if (this->inputState.mouseOver)
        {
            this->OnMouseOver();
        }
        else
        {
            this->OnMouseOut();
        }

        this->inputState.mouseOverPrev = this->inputState.mouseOver;
    }
}

void Gui::OnMouseOver()
{
    Game::instance->cursorMode = CursorMode::MOUSE_HAND;
}

void Gui::OnMouseOut()
{
    Game::instance->cursorMode = CursorMode::MOUSE_ARROW;
}

void Gui::OnMouseDown()
{
}

void Gui::OnMouseUp()
{
    Game::instance->cursorMode = CursorMode::MOUSE_ARROW;
}

void Gui::EventBeginPlay()
{
}

void Gui::EventTick(float deltaTime)
{
    if (this->isEnableInput)
    {
        this->EventListener();
    }
}

void Gui::EventKeyDown(KeyEvent e)
{
}

void Gui::EventKeyUp(KeyEvent e)
{
}

void Gui::EventMouseDown(MouseEvent e)
{
    if (this->inputState.mouseOver) this->OnMouseDown();

    if (e.button == MouseEvent::Button::LEFT)
    if (this->inputState.mouseOver)
        this->inputState.dragging = true;
}

void Gui::EventMouseUp(MouseEvent e)
{
    if (this->inputState.mouseOver)
    {
        this->OnMouseUp();
        this->inputState.mouseOver = false;
    }

    if (e.button == MouseEvent::Button::LEFT) this->inputState.dragging = false;
}
