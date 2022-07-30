#include "const.hpp"
#include "utils.hpp"
#include "DX9.hpp"
#include "Window.hpp"
#include "GameObject.hpp"
#include "Koro.hpp"
#include "Debug.hpp"
#include "Sound.hpp"
#include "Stage.hpp"
#include "Game.hpp"
#include "MathLib.hpp"
#include <format>
#include <string>
#pragma once

// カメラモード
enum class CameraMode
{
    Title_camera,     // タイトルのカメラ
    Tracking_Stage,   // ステージ中心　固定
    Tracking_Koro,    // koro中心　　 追尾
    ResetCamera,
    Clear_camera,      // クリア時のカメラ
};

// カメラモードとカメラ距離保存マップ
inline std::map<CameraMode, float> CAMERAMODE_WITH_DISTANCE =
{
    { CameraMode::Tracking_Stage, 50.0f },
    { CameraMode::Tracking_Koro,  15.0f }
};

class Camera : public GameObject
{
public:
    static Camera* instance;

// コンストラクタ・デストラクタ
public:
    /// <summary>
    /// カメラのコンストラクタ
    /// </summary>
    /// <param name="look">ステージの中心地点　注視点</param>
    /// <param name="angle">カメラの角度 </param>
    /// <param name="distance">基点からの距離 </param>
    Camera(D3DXVECTOR3 look, float angle, float distance);

    /// <summary>
    /// カメラのデストラクタ
    /// </summary>
    ~Camera();

// 関数
public:

// Get関数

    // カメラの座標の取得
    D3DXVECTOR3 GetCameraLocation();
    // カメラの向いてる場所の取得
    D3DXVECTOR3 GetCameraLookAtLocation();
    // カメラのUpvectorの取得
    D3DXVECTOR3 GetCameraUpVector();


// Set関数

    /// <summary>
    /// 視線の変更
    /// </summary>
    /// <param name="lookAt">注視点</param>
    bool SetLookVector(D3DXVECTOR3);
    /// <summary>
    /// カメラモードの指定
    /// </summary>
    /// <param name="mode">カメラのモード</param>
    /// <returns></returns>
    bool SetCameraMode(CameraMode mode);
    /// <summary>
    /// カメラを指定の位置、向きにlerpさせる
    /// </summary>
    /// <param name="position">位置</param>
    /// <param name="look">注視点</param>
    /// <param name="upvector">カメラの上方向(向き)</param>
    /// <param name="speed">lerpの速さ()</param>
    /// <returns></returns>
    bool SetLerpCamera(D3DXVECTOR3 position, D3DXVECTOR3 look, D3DXVECTOR3 upvector, float speed);


// CameraLerp関数

    // 初期位置へ
    bool MoveTitleCamera();
    //　ゴールフラッグによる
    bool MoveClearCamera();
    // 初期位置に戻る
    bool CameraResetToInGame();
    //　タイトルの位置に戻る
    bool CameraResetToTitle();
    // koroによる
    bool CameraZoomKoro();
    // ゴールフラッグによる
    bool CameraZoomGoal();
    

    // ゴールフラッグの周りを回す
    bool MoveRotateGoalFlag();

private:

    /// <summary>
    /// カメラの初期化
    /// </summary>
    /// <param name="look">カメラの注視点</param>
    /// <param name="angle">見下ろす角度</param>
    /// <param name="distance">注視点からの距離</param>
    /// <returns></returns>
    bool InitState(D3DXVECTOR3 look, float angle, float distance);
    /// <summary>
    /// 行列の初期化
    /// </summary>
    /// <returns></returns>
    bool InitMatrix();
    /// <summary>
    /// カメラの更新
    /// </summary>
    /// <returns>移動有=true　移動無=false</returns>
    bool CameraUpdate(CameraMode);
    /// <summary>
    /// 追尾カメラの計算
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool TrackingCamera(CameraMode); 
    /// <summary>
    /// Lerpした座標の計算
    /// </summary>
    /// <param name="first">基点</param>
    /// <param name="end">終点</param>
    /// <param name="position">動かすVector3野ポインタ</param>
    /// <param name="speed">動かす速さ</param>
    /// <returns>false=移動中　true=移動終了</returns>
    bool LerpCamera();
    /// <summary>
    /// Lerp終了時に呼び出される
    /// </summary>
    /// <returns></returns>
    bool EndLerp();

    bool RotateGoalFlag();

    // デバック用
    void DebugState();

//オーバーライド関数
public:

    virtual void EventBeginPlay() override;
    virtual void EventTick(float deltaTime) override;
    virtual void EventKeyDown(KeyEvent e) override;
    virtual void EventKeyUp(KeyEvent e) override;
    virtual void EventMouseDown(MouseEvent e) override;
    virtual void EventMouseUp(MouseEvent e) override;
    virtual void EventMouseWheelMove(int delta) override;

// 変数
public:


public:
    CameraMode  camera_mode;		// カメラモード

    D3DXVECTOR3 camera_look,		// カメラの注視点
                camera_position,	// カメラの座標
                camera_upvec,       // カメラのUPVECTOR
                
                initLook,
                initCameraPosition, // カメラの初期位置


                lerpPosition,       // Lerp先位置
                lerpLook,           // Lerp先注視点
                lerpUpvector,       // Lerp先Upvector

                tmpCameraPosition,  // Lerp前位置
                tmpLookPosition,    // Lerp前注視点
                tmpCameraUpvector,  // Lerp前Upvector
        
                debug_vector;	    // デバック用座標

    D3DXMATRIX  view_matrix,		// ビューの行列
                world_matrix,		// ワールドの行列
                projection;         // プロジェクション行列

    float		target_distance,	// ステージまでの距離
                target_angle,       // カメラの見下ろす角度
                camera_rotationYAW, // カメラのYAWの回転

                initDistance,       // カメラの初期の距離
                initAngle,

                lerp_speed,         // Lerpの速さ
                lerp_count,         // Lerpのカウント
                debugFloat;

    bool		mouse_Button_Right,	// マウスの左クリック 押true/離false
                camera_zoom,        // カメラのzoom状態
                camera_out,         // カメラのzoomout状態

                lerp_flg,           // Lerp中判定

                // 各lerpのフラグ
                goalCameraFlg,      //
                resetCamera_Ingame, //
                titleCameraMoveFlg, //
                zoom_Koro_Flag,     //
                zoom_Goal_flag,     //
                
                rotation_GoalFlag;  //
};
