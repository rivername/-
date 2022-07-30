#include <vector>

#include "Window.hpp"
#include "Cube.hpp"
#include "XModel.hpp"
#include "TriggerBox.hpp"
#include "Game.hpp"

#pragma once

class Koro;

namespace stage
{
    // 左右の傾きの最大値
    const float MAX_ANGLE_X = D3DX_PI / 4;

    // 前後の傾きの最大値
    const float MAX_ANGLE_Y = D3DX_PI / 4;
}

class Stage : public GameObject, virtual public Transform
{
public:
    inline static Stage* instance;

public:
    Stage();
    ~Stage();

    bool Init();

    // ステージが動いているか
    bool IsMoving();

    // ステージ番号からステージを読み込み
    // @param num : ステージ番号
    bool LoadStageByNum(UINT num);

    // タイトル画面のために読み込んだステージのメモリを解放する
    bool MinimalizeData();

    // スタート地点を取得
    D3DXVECTOR3 GetStartPoint();

public:
    // タイトル画面での回転
    void TitleStageRotation();

    // ステージの回転をゆっくりリセット
    void ReturnStageRotationLerpToInit();

public:
    // マウスの入力を取得
    void GetMouseInput();

    // ステージを回転させる
    void RotateStage();

    // ステージの操作をリセット
    void ResetHandle();

    // 初期回転にゆっくりと戻す
    bool InterpToInitialRotation();

public:
    // KOROを基準に手前にいるか奥にいるかどうかで並べ替えられたリストペア
    // @param pOutFront : KOROより手前のオブジェクトを格納するアドレス
    // @param pOutBack  : KOROより後ろのオブジェクトを格納するアドレス
    void GetSortedHLSLObjects(std::vector<HLSLObject*>* pOutFront, std::vector<HLSLObject*>* pOutBack);

private:
    // 前フレームのステージの状態
    D3DXMATRIX prevLocalMatrix;

public:
    // マウス操作用
    float yawAngle;
    float l_x_axis;
    float l_y_axis;

    POINT beginDragPos;
    POINT endDragPos;

    // ドラッグ状態
    bool isMouseDragging;

    D3DXVECTOR2 dragDirection;
    float       dragLength;

public:
    // 各ステージで管理するActorリスト配列
    std::vector<std::vector<Actor*>> actors;

private:
    // 各ステージのセーブデータ配列
    std::vector<std::vector<ActorSaveData>> levelDatas;

public:
    // wrapping GameObject::DisableInput()
    void DisableInput();

private:
    // GameObject

    virtual void EventBeginPlay() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;
    virtual void EventGamepadAxisInput(GamepadAxisEvent e) override;

    // Transform

    virtual void EventTransformUpdated() override;
};
