#include "Camera.hpp"

Camera* Camera::instance;


/************************************
コンストラクタ・デストラクタ
*************************************/
Camera::Camera(D3DXVECTOR3 look,float angle,float distance) : GameObject(ObjectType::CAMERA)
{

    Camera::instance = this;

    this->initLook = look;
    this->initAngle = angle;
    this->initDistance = distance;


    // 数値の初期化
    this->InitState(look, angle,distance);

    // 行列の初期化
    this->InitMatrix();

}

Camera::~Camera()
{

}

/************************************
変数の初期化
************************************/

bool Camera::InitState(D3DXVECTOR3 look, float angle,float distance)
{

    // 各値の初期化
    this->camera_look = look;
    this->target_angle = angle;
    this->target_distance = distance;


    this->camera_position = D3DXVECTOR3(-20.0f, 6.0f, -10.0f);
    this->camera_upvec = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    this->initCameraPosition = D3DXVECTOR3(0.0f, this->target_distance * sinf(this->target_angle), (this->target_distance * cosf(this->target_angle)) * -1);

    D3DXMatrixIdentity(&this->view_matrix);


    this->camera_rotationYAW = 1.57f;
    this->lerp_count = 0.0f;


    this->camera_mode = CameraMode::Title_camera;
    this->camera_zoom = false;
    this->camera_out  = false;

    this->debugFloat = 0;


    this->lerp_flg = false;

    this->titleCameraMoveFlg = false;
    this->resetCamera_Ingame = false;
    this->zoom_Koro_Flag	 = false;
    this->zoom_Goal_flag     = false;
    this->goalCameraFlg      = false;
    this->rotation_GoalFlag  = false;

    return true;
}

bool Camera::InitMatrix()
{
    D3DXMatrixIdentity(&this->world_matrix);
    D3DXMatrixIdentity(&this->projection);

    // 行列の処理
    D3DXMatrixLookAtLH(&this->view_matrix, &this->camera_position, &this->camera_look, &this->camera_upvec);
    D3DXMatrixPerspectiveFovLH(&this->projection, D3DXToRadian(45.0f), (float)Window::instance->width / (float)Window::instance->height, 0.01f, 1000.0f);
    DX9::instance->device3d->SetTransform(D3DTS_WORLD,      &this->world_matrix);
    DX9::instance->device3d->SetTransform(D3DTS_VIEW,       &this->view_matrix);
    DX9::instance->device3d->SetTransform(D3DTS_PROJECTION, &this->projection);

    return false;
}

/************************************
Set関数
************************************/

bool Camera::SetLookVector(D3DXVECTOR3 lookAt)
{
    this->camera_look = lookAt;
    return true;
}

bool Camera::SetCameraMode(CameraMode mode)
{

    this->camera_mode = mode;

    return true;
}

bool Camera::SetLerpCamera(D3DXVECTOR3 position, D3DXVECTOR3 look, D3DXVECTOR3 upvector, float speed)
{
    // 現在の位置
    this->tmpCameraPosition = this->camera_position;
    this->tmpCameraUpvector = this->camera_upvec;
    this->tmpLookPosition = this->camera_look;

    // 移動先の位置
    this->lerpPosition = position;
    this->lerpUpvector = upvector;
    this->lerpLook = look;
    this->lerp_speed = speed;

    // Lerp関数を切り替え
    this->lerp_flg = true;

    return true;
}

/************************************
Get関数
************************************/

D3DXVECTOR3 Camera::GetCameraLocation()
{
    return this->camera_position;
}

D3DXVECTOR3 Camera::GetCameraLookAtLocation()
{
    return this->camera_look;
}

D3DXVECTOR3 Camera::GetCameraUpVector()
{
    return this->camera_upvec;
}

/************************************
カメラ挙動
************************************/

bool Camera::CameraUpdate(CameraMode mode)
{
    switch (mode)
    {
    case CameraMode::Title_camera:

        this->LerpCamera();
        break;

    case CameraMode::ResetCamera:

        this->LerpCamera();
        break;

    case CameraMode::Tracking_Koro:

        // ゆっくり移動
        InterpToV3(&this->camera_look, this->camera_look, Koro::instance->GetWorldLocation(), 8.0f);

        this->TrackingCamera(mode);

        break;

    case CameraMode::Tracking_Stage:

        // ゆっくり移動
        InterpToV3(&this->camera_look, this->camera_look, Stage::instance->GetWorldLocation(), 8.0f);

        this->TrackingCamera(mode);

        break;

    case CameraMode::Clear_camera:

        this->LerpCamera();
        this->RotateGoalFlag();
        break;
    }

    // 保存されたカメラ距離にInterpTo
    if (mode == CameraMode::Tracking_Stage ||
        mode == CameraMode::Tracking_Koro)
    {
        InterpTo(&this->target_distance, this->target_distance, CAMERAMODE_WITH_DISTANCE[mode], 8.0f);
    }

    return true;
}

bool Camera::TrackingCamera(CameraMode mode)
{

    this->camera_position.x = this->camera_look.x;
    this->camera_position.y = this->camera_look.y + (this->target_distance * sinf(this->target_angle));
    this->camera_position.z = this->camera_look.z + -(this->target_distance * cosf(this->target_angle));


    if (this->camera_zoom == true)this->target_distance -= 0.5f;

    if (this->camera_out == true)this->target_distance += 0.5f;

    if (this->target_distance < 15.0f) this->target_distance = 15.0f;
    if (this->target_distance > 75.0f) this->target_distance = 75.0f;


    return false;
}

bool Camera::RotateGoalFlag()
{
    if (this->rotation_GoalFlag == false) return false;

    float radius = 10.0f;

    this->camera_position.x = this->camera_look.x + (radius * cosf(this->camera_rotationYAW));
    this->camera_position.z = this->camera_look.z + (radius * sinf(this->camera_rotationYAW));

    this->camera_rotationYAW += 0.005f;

    return true;
}

bool Camera::MoveRotateGoalFlag()
{
    this->rotation_GoalFlag = true;

    return false;
}



/************************************
CameraLerp関数
************************************/

bool Camera::MoveTitleCamera()
{
    if (this->titleCameraMoveFlg == true) return false;

    this->SetLerpCamera(
        this->initCameraPosition,
        Stage::instance->GetWorldLocation(),
        D3DXVECTOR3(0.0f, 1.0f, 0.0f),
        0.02f
    );

    this->titleCameraMoveFlg = true;

    return true;
}

bool Camera::MoveClearCamera()
{
    if (this->goalCameraFlg == true) return false;

    this->SetLerpCamera(
        GoalFlag::instance->GetWorldLocation() + (10.0f * (-GoalFlag::instance->GetRightVector() + -GoalFlag::instance->GetForwardVector() + GoalFlag::instance->GetUpVector())),
        GoalFlag::instance->GetWorldLocation(),
        Stage::instance->GetUpVector(),
        0.05f
    );

    this->camera_mode = CameraMode::Clear_camera;

    this->goalCameraFlg = true;

    return true;
}

bool Camera::CameraResetToTitle()
{

    this->InitState(initLook, initAngle, initDistance);
    this->InitMatrix();

    return false;
}

bool Camera::CameraZoomKoro()
{

    if (this->zoom_Koro_Flag == true) return false;

    this->SetLerpCamera(
        Koro::instance->GetWorldLocation() + (10.0f * -Koro::instance->GetForwardVector() + Koro::instance->GetUpVector()),
        Koro::instance->GetWorldLocation(),
        Stage::instance->GetUpVector(),
        0.01f
    );

    this->zoom_Koro_Flag = true;

    return true;
}

bool Camera::CameraZoomGoal()
{

    if (this->zoom_Goal_flag == true)return false;

    this->SetLerpCamera(
        GoalFlag::instance->GetWorldLocation() + (10.0f * -Koro::instance->GetForwardVector() + Koro::instance->GetUpVector()),
        GoalFlag::instance->GetWorldLocation(),
        Stage::instance->GetUpVector(),
        0.01f
    );

    this->zoom_Goal_flag = true;

    return false;
}

bool Camera::CameraResetToInGame()
{
    //if (this->resetCamera_Ingame == true)return false;

    this->SetLerpCamera(
        this->initCameraPosition,
        Stage::instance->GetWorldLocation(),
        D3DXVECTOR3(0.0f, 1.0f, 0.0f),
        0.05f
    );

    this->camera_mode = CameraMode::Tracking_Stage;

    this->camera_zoom = false;
    this->camera_out = false;
    this->resetCamera_Ingame = true;

    return true;
}

bool Camera::LerpCamera()
{
    if (this->lerp_flg == false) return false;

    if (this->lerp_count >= 1.0f)
    {
        this->lerp_count = 0.0f;
        this->lerp_flg = false;
        this->EndLerp();
        return true;
    }
    else
    {

        D3DXVec3Lerp(&this->camera_upvec, &this->tmpCameraUpvector, &this->lerpUpvector, this->lerp_count);
        D3DXVec3Lerp(&this->camera_position, &this->tmpCameraPosition, &this->lerpPosition, this->lerp_count);
        D3DXVec3Lerp(&this->camera_look, &this->tmpLookPosition, &this->lerpLook, this->lerp_count);

        this->lerp_count += this->lerp_speed;
        return false;
    }
}

bool Camera::EndLerp()
{
    // Lerpが終わったら呼ばれる
    if (this->titleCameraMoveFlg) {

        Game::instance->CustomEvent__TitleCameraMoveEnded();
        this->SetCameraMode(CameraMode::Tracking_Stage);
        this->titleCameraMoveFlg = false;
    }

    else if (this->zoom_Koro_Flag) {


        this->zoom_Koro_Flag = false;
    }

    else if (zoom_Goal_flag) {


        this->zoom_Goal_flag = false;
    }

    else if (this->goalCameraFlg) {

        this->goalCameraFlg = false;
    }

    else if (this->resetCamera_Ingame) {

        if (this->camera_mode == CameraMode::Title_camera) {

        }

        this->resetCamera_Ingame = false;
    }

    return false;
}

/************************************
Event関数
************************************/

void Camera::EventBeginPlay()
{

}

void Camera::EventTick(float deltaTime)
{
    /*Debug::AddLine("Camera Distance: {}", this->target_distance);
    Debug::AddLine("CameraMode: {}", nameof::nameof_enum(this->camera_mode));*/

    IF_PAUSE_RETURN;

    this->CameraUpdate(this->camera_mode);

    this->DebugState();

    D3DXMatrixLookAtLH(&this->view_matrix, &this->camera_position, &this->camera_look, &this->camera_upvec);
    DX9::instance->device3d->SetTransform(D3DTS_VIEW, &this->view_matrix);

}

void Camera::EventKeyDown(KeyEvent e)
{
    if (e.keyCode == 'T')
    {
        if (this->camera_mode == CameraMode::Tracking_Stage)
        {
            this->SetCameraMode(CameraMode::Tracking_Koro);
        } else

        if (this->camera_mode == CameraMode::Tracking_Koro)
        {
            this->SetCameraMode(CameraMode::Tracking_Stage);
        }
    }

    if (e.keyCode == 'B') {


        Sound::instanse->UpDownVolumeAll(true);
    }

    if (e.keyCode == 'N') {
        Sound::instanse->UpDownVolumeAll(false);
    }

    if (e.keyCode == 'G') {
        Sound::instanse->SetVolumeBuffer(0.7f);
    }

    if (e.keyCode == 'H') {
        Sound::instanse->SetVolumeBuffer(0.0f);
    }

}

void Camera::EventKeyUp(KeyEvent e)
{
}

void Camera::EventMouseDown(MouseEvent e)
{
    if (e.button == MouseEvent::Button::RIGHT)
    {
        this->mouse_Button_Right = true;
    }
}

void Camera::EventMouseUp(MouseEvent e)
{
    if (e.button == MouseEvent::Button::RIGHT)
    {
        this->mouse_Button_Right = false;
    }
}

void Camera::EventMouseWheelMove(int delta)
{
    if (Game::instance->appMode == AppMode::LEVEL_EDITOR) return;

    // カメラ距離を設定
    if (this->camera_mode == CameraMode::Tracking_Stage ||
        this->camera_mode == CameraMode::Tracking_Koro)
    {
        CAMERAMODE_WITH_DISTANCE[this->camera_mode] -= delta * 10.0f;
        CAMERAMODE_WITH_DISTANCE[this->camera_mode]  = std::clamp(CAMERAMODE_WITH_DISTANCE[this->camera_mode], 15.0f, 50.0f);
    }
}

/************************************
Debug関数
************************************/

void Camera::DebugState()
{

    //Debug::AddLine("distance {}", this->target_distance);

    Debug::AddLine("volume {}", Sound::instanse->volumeBGM);


    /*
    this->debug_vector =
    Debug::AddLine("x = {} y = {} z {}", this->debug_vector.x, this->debug_vector.y, this->debug_vector.z);
    */
}
