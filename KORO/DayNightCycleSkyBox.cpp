#include "DayNightCycleSkyBox.hpp"
#include "Canvases.hpp"
#include "Camera.hpp"
#include "resource.h"

DayNightCycleSkyBox::DayNightCycleSkyBox() : skyboxData({}), interpSpeed(10.0f), offsetMode(SkyBoxOffsetMode::IN_TITLE)
{
    DayNightCycleSkyBox::instance = this;

    // インスタンス化
    this->skybox_color     = new SkyBox(TEX__SKYBOX_COLOR);
    this->skybox_filter    = new SkyBox(TEX__SKYBOX_FILTER);
    this->skybox_star      = new SkyBox(TEX__SKYBOX_STAR);
    this->skybox_moon      = new SkyBox(TEX__SKYBOX_MOON);
    this->skybox_sun_color = new SkyBox(TEX__SKYBOX_SUN_COLOR);
    this->skybox_sun_flare = new SkyBox(TEX__SKYBOX_SUN_FLARE);

    // 階層順を設定
    this->skybox_color->sizeAddValue     =  100.0f;
    this->skybox_filter->sizeAddValue    =  100.0f;
    this->skybox_star->sizeAddValue      = -200.0f;
    this->skybox_moon->sizeAddValue      = -300.0f;
    this->skybox_sun_color->sizeAddValue = -310.0f;
    this->skybox_sun_flare->sizeAddValue = -311.0f;

    // 読み込んだセーブデータのSkyBoxDataを反映
    this->skyboxData = Game::instance->saveData.skyboxData;

    // SkyBoxコントローラーに設定を反映 // teleport = true
    Game::instance->InstanceCanvas<DayNightCycleSkyBoxController>()->SetControlSlide(this->skyboxData.slideValue, true);

    // サイクルモードを適用
    this->SetCycleMode(this->skyboxData.cycleMode);
}

DayNightCycleSkyBox::~DayNightCycleSkyBox()
{
}

void DayNightCycleSkyBox::SetSkyBoxColorByCurrentDateTimeVar()
{
    // 時間の範囲から現在の時刻を正規化
    // @param fromHour   : from時間
    // @param toHour     : to時間
    // @param current    : 現在のDateTime
    // @return { float } : 正規化された値
    static auto NormalizeInHourRange = [](int fromHour, int toHour, DateTime current)
    {
        if (fromHour == toHour) return 0.0f;

        // 既に進んだ時間分の秒数を足す
        current.sec += ((current.hour - fromHour) * 3600);

        int range = toHour - fromHour;

        if (toHour < fromHour)
        {
            range = 24 - fromHour;
        }

        // fromから進んだ秒 / Range全体の秒数
        return ((current.min * 60.0f) + current.sec) / (3600.0f * range);
    };

    // SkyBoxパラメーター現在時刻
    DateTime current = SkyBoxParam::currentDateTime;

    // 時間（H）によるSkyBoxColorSceneのペア配列
    std::pair<int, SkyBoxColorScene> timeToLerpWithColorScene[] =
    {
        {  0, SkyBoxColorScene::MIDNIGHT },
        {  3, SkyBoxColorScene::MIDNIGHT },
        {  4, SkyBoxColorScene::EARLY_MORNING },
        {  6, SkyBoxColorScene::SUNRISE },
        {  9, SkyBoxColorScene::MORNING },
        { 12, SkyBoxColorScene::DAY },
        { 15, SkyBoxColorScene::DAY },
        { 18, SkyBoxColorScene::SUNSET },
        { 19, SkyBoxColorScene::NIGHT },
    };

    auto from = timeToLerpWithColorScene[0]; // Fromペア
    auto to   = timeToLerpWithColorScene[0]; // Toペア

    // fromとtoに該当するペアを現在時刻から求める
    for (size_t i = 0; i < ARRAYSIZE(timeToLerpWithColorScene); i++)
    {
        auto& pair = timeToLerpWithColorScene[i];

        // 超えてしまったら
        if (pair.first > current.hour)
        {
            if (i > 0)
            {
                // 一個前が該当するやつ
                i--;
            }

            from = timeToLerpWithColorScene[i];

            // それ以外はその次の番号
            to = timeToLerpWithColorScene[i + 1];

            break;
        }

        // 超えなかった場合はfromを配列の最後にし、toを配列の先頭に設定
        if (i == ARRAYSIZE(timeToLerpWithColorScene) - 1)
        {
            from = timeToLerpWithColorScene[i];
            to   = timeToLerpWithColorScene[0];

            break;
        }
    }

#pragma region Alpha計算

    // 色用のAlphaを計算
    float colorLerpAlpha = NormalizeInHourRange(from.first, to.first, current);

    // 回転用のAlphaを計算
    float rotateLerpAlpha = NormalizeInHourRange(0, 1, DateTime(0, current.min, current.sec));

#pragma endregion

#pragma region 色設定 : SkyBoxColor, SkyBoxFilter

    // alphaでの結果の色にゆっくり遷移
    bool color  = this->ColorInterpTo(this->skybox_color,  this->skybox_color->hlslParam.mulColor,  this->LerpColor(SKYBOX_COLOR_COLOR[from.second],  SKYBOX_COLOR_COLOR[to.second],  colorLerpAlpha), this->interpSpeed);
    bool filter = this->ColorInterpTo(this->skybox_filter, this->skybox_filter->hlslParam.mulColor, this->LerpColor(SKYBOX_FILTER_COLOR[from.second], SKYBOX_FILTER_COLOR[to.second], colorLerpAlpha), this->interpSpeed);

    // SkyBoxColorSceneから目標の星と月の色αを取得
    auto GetTargetStarAndMoonColorAlphaByColorScene = [](SkyBoxColorScene colorScene)
    {
        switch (colorScene)
        {
        case SkyBoxColorScene::NIGHT:
        case SkyBoxColorScene::MIDNIGHT:
            return 1.0f;

        case SkyBoxColorScene::EARLY_MORNING:
        case SkyBoxColorScene::SUNSET:
            return 0.25f;

        default:
            return 0.0f;
        }
    };

    // SkyBoxColorSceneから環境光（方向性ライト）の色を取得
    auto GetTargetAmbientLightColor = [](SkyBoxColorScene colorScene)
    {
        switch (colorScene)
        {
        case SkyBoxColorScene::EARLY_MORNING: // 早朝
        case SkyBoxColorScene::SUNSET:        // 夕方

            return D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

        case SkyBoxColorScene::NIGHT:    // 夜
        case SkyBoxColorScene::MIDNIGHT: // 深夜

            return D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);

        default: // 夜明け、朝、昼
            
            return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
        }
    };

    // alphaでの結果の色にゆっくり遷移

    // 星の色α
    bool star = InterpTo(&this->skybox_star->hlslParam.mulColor.a, this->skybox_star->hlslParam.mulColor.a, std::lerp(GetTargetStarAndMoonColorAlphaByColorScene(from.second), GetTargetStarAndMoonColorAlphaByColorScene(to.second), colorLerpAlpha), this->interpSpeed);

    // 月の色α
    bool moon = InterpTo(&this->skybox_moon->hlslParam.mulColor.a, this->skybox_moon->hlslParam.mulColor.a, std::lerp(GetTargetStarAndMoonColorAlphaByColorScene(from.second), GetTargetStarAndMoonColorAlphaByColorScene(to.second), colorLerpAlpha), this->interpSpeed);

    // 太陽の色と太陽フレアの色α
    bool sun[2] =
    {
        // alphaでの結果の色にゆっくり遷移

        // 太陽の色
        this->ColorInterpTo(this->skybox_sun_color, this->skybox_sun_color->hlslParam.mulColor, this->LerpColor(SKYBOX_SUN_COLOR[from.second], SKYBOX_SUN_COLOR[to.second], colorLerpAlpha), this->interpSpeed),

        // 太陽フレアの色α
        InterpTo(&this->skybox_sun_flare->hlslParam.mulColor.a, this->skybox_sun_flare->hlslParam.mulColor.a, std::lerp(SKYBOX_SUN_COLOR[from.second].a, SKYBOX_SUN_COLOR[to.second].a, colorLerpAlpha), this->interpSpeed)
    };

    // ステージライト
    bool ambientLight = this->ColorInterpTo(&SkyBoxParam::ambientLightColor, SkyBoxParam::ambientLightColor, this->LerpColor(GetTargetAmbientLightColor(from.second), GetTargetAmbientLightColor(to.second), colorLerpAlpha), this->interpSpeed);

    // 色補間完了
    bool colorInterpDone = (color && filter && star && moon && sun[0] && sun[1] && ambientLight);

    // Interpが完了したらデフォルトのスピードにする（起動時のみ）
    if (colorInterpDone)
    {
        this->interpSpeed = SkyBoxParam::defaultInterpSpeed;
    }

    // 太陽フレアと星と月の色RGBは常に255(1.0f)

    this->skybox_sun_flare->hlslParam.mulColor.r = 1.0f;
    this->skybox_sun_flare->hlslParam.mulColor.g = 1.0f;
    this->skybox_sun_flare->hlslParam.mulColor.b = 1.0f;

    this->skybox_star->hlslParam.mulColor.r = 1.0f;
    this->skybox_star->hlslParam.mulColor.g = 1.0f;
    this->skybox_star->hlslParam.mulColor.b = 1.0f;

    this->skybox_moon->hlslParam.mulColor.r = 1.0f;
    this->skybox_moon->hlslParam.mulColor.g = 1.0f;
    this->skybox_moon->hlslParam.mulColor.b = 1.0f;

    Debug::AddLine("\n\nColor Interp Done: {}", colorInterpDone);

#pragma endregion

#pragma region 回転設定

    // From回転
    float fromRot = SkyBoxParam::rotationAmoInAHour * (current.hour);

    // To回転
    float toRot = SkyBoxParam::rotationAmoInAHour * (current.hour + 1);

    // 星と月の回転（目標値）
    float targetStarMoonRot = std::lerp(fromRot, toRot, rotateLerpAlpha);

    // 前回のSkyBoxCycleMode
    static SkyBoxCycleMode prevMode = this->skyboxData.cycleMode;

    // モードが切り替わったら
    if (prevMode != this->skyboxData.cycleMode)
    {
        // 常に東から西に天体が回るようにする
        if (targetStarMoonRot < this->skybox_star->rotation.roll)
        {
            this->skybox_star->rotation.roll      -= 360.0f;
            this->skybox_moon->rotation.roll      -= 360.0f;
            this->skybox_sun_color->rotation.roll -= 360.0f;
            this->skybox_sun_flare->rotation.roll -= 360.0f;
        }

        prevMode = this->skyboxData.cycleMode;
    }

    // 太陽の回転（目標値）= (月の回転 + 180)
    float targetSunRot = targetStarMoonRot + 180.0f;

    // 星と月の回転（Lerp版）
    float ROLL_STAR_MOON =
        // LerpでEase-Out
        std::lerp(this->skybox_moon->rotation.roll, targetStarMoonRot, 0.5f * SkyBoxParam::rotateInterpSpeed);

    // 太陽の回転（Lerp版）
    float ROLL_SUN =
        // LerpでEase-Out
        std::lerp(this->skybox_sun_color->rotation.roll, targetSunRot, 0.5f * SkyBoxParam::rotateInterpSpeed);

    bool lerpDone =
        FLOAT_EQ(targetStarMoonRot - ROLL_STAR_MOON, 0.0f) && FLOAT_EQ(targetSunRot - ROLL_SUN, 0.0f);

    // 0時になったとき（回転が一周して0に戻るとき）
    if (targetStarMoonRot == 0.0f) // ぴったりでOK
    {
        // 強制的に値を変更
        ROLL_STAR_MOON = 0.0f;
        ROLL_SUN       = ROLL_STAR_MOON + 180.0f;
    }

    // Lerpが完了したらデフォルトのスピードにする
    if (lerpDone)
    {
        SkyBoxParam::rotateInterpSpeed = SkyBoxParam::defaultInterpSpeed;
    }

    Debug::AddLine("\n\nRotation Lerp Done: {}", lerpDone);

    // 回転を適用
    this->skybox_star->rotation.roll      = ROLL_STAR_MOON;
    this->skybox_moon->rotation.roll      = ROLL_STAR_MOON;
    this->skybox_sun_color->rotation.roll = ROLL_SUN;
    this->skybox_sun_flare->rotation.roll = ROLL_SUN;

#pragma endregion

    Debug::AddLine("\nTarget ROLL_STAR_MOON: {} / Lerp ROLL_STAR_MOON: {}", targetStarMoonRot, ROLL_STAR_MOON);
    Debug::AddLine("ROLL_STAR_MOON Diff: {}", targetStarMoonRot - ROLL_STAR_MOON);

    Debug::AddLine("\nTarget ROLL_SUN: {} / Lerp ROLL_STAR_MOON: {}", targetSunRot, ROLL_SUN);
    Debug::AddLine("ROLL_SUN Diff: {}", targetSunRot - ROLL_SUN);

    Debug::AddLine("\nPair | From Hour: {} / To Hour: {}", from.first, to.first);
    Debug::AddLine("Current Date Time: {}/{} - {}:{}:{}", current.month, current.day, current.hour, current.min, current.sec);
    Debug::AddLine("\nColor Alpha: {}", colorLerpAlpha);
    Debug::AddLine("Rotation Alpha: {}\n\n\n\n\n", rotateLerpAlpha);
}

bool DayNightCycleSkyBox::ColorInterpTo(SkyBox* target, D3DXCOLOR current, D3DXCOLOR to, float interpSpeed)
{
    return this->ColorInterpTo(&target->hlslParam.mulColor, current, to, interpSpeed);
}

bool DayNightCycleSkyBox::ColorInterpTo(D3DXCOLOR* out, D3DXCOLOR current, D3DXCOLOR to, float interpSpeed)
{
    D3DXVECTOR4 color = { out->r, out->g, out->b, out->a };

    bool done = InterpToV4(&color, color, { to.r, to.g, to.b, to.a }, interpSpeed);

    out->r = color.x;
    out->g = color.y;
    out->b = color.z;
    out->a = color.w;

    Debug::AddLine("Interp Color: {} / {} / {} / {}", color.x, color.y, color.z, color.w);

    return done;
}

D3DXCOLOR DayNightCycleSkyBox::LerpColor(D3DXCOLOR from, D3DXCOLOR to, float alpha)
{
    return
    {
        std::lerp(from.r, to.r, alpha),
        std::lerp(from.g, to.g, alpha),
        std::lerp(from.b, to.b, alpha),
        std::lerp(from.a, to.a, alpha)
    };
}

void DayNightCycleSkyBox::SetTransform()
{
    // すべてのSkyBox
    static SkyBox* all_skybox[] =
    {
        this->skybox_color,
        this->skybox_filter,
        this->skybox_star,
        this->skybox_moon,
        this->skybox_sun_color,
        this->skybox_sun_flare
    };

    // 宇宙系のSkyBox
    static SkyBox* universe_skybox[] =
    {
        this->skybox_star,
        this->skybox_moon,
        this->skybox_sun_color,
        this->skybox_sun_flare
    };

    // カメラの方向
    D3DXVECTOR3 direction = Camera::instance->GetCameraLookAtLocation() - Camera::instance->GetCameraLocation();

    // カメラのYaw角度を取得
    float yaw = D3DXToDegree(atan2f(direction.x, direction.z));

    // すべてのSkyBoxのYaw回転をカメラのYaw回転と同じ値に
    for (auto skybox : all_skybox)
    {
        skybox->rotation.yaw = yaw;
    }

    switch (this->offsetMode)
    {
    // タイトル画面での見え方の設定
    case SkyBoxOffsetMode::IN_TITLE:

        // オフセットの設定
        for (auto skybox : all_skybox) skybox->offset.y = -64.0f;

        // 星・月・太陽の回転の設定
        for (auto skybox : universe_skybox) skybox->rotation.pitch = 45.0f;

        break;

    case SkyBoxOffsetMode::CLEARED:

        // オフセットを下げる
        for (size_t i = 0; i < ARRAYSIZE(all_skybox); i++)
        {
            switch (i)
            {
            case 0: // skybox_color
            case 1: // skybox_filter
                InterpTo(&all_skybox[i]->offset.y, all_skybox[i]->offset.y, -128.0f + 16.0f, 10.0f);
                break;

            default: // それ以外

                InterpTo(&all_skybox[i]->offset.y, all_skybox[i]->offset.y, -48.0f + 16.0f, 10.0f);
                break;
            }
        }

        break;

    // それ以外での見え方の設定
    default:

        // オフセットを下げる
        for (size_t i = 0; i < ARRAYSIZE(all_skybox); i++)
        {
            switch (i)
            {
            case 0: // skybox_color
            case 1: // skybox_filter
                InterpTo(&all_skybox[i]->offset.y, all_skybox[i]->offset.y, -128.0f, 10.0f);
                break;

            default: // それ以外

                InterpTo(&all_skybox[i]->offset.y, all_skybox[i]->offset.y, -48.0f, 10.0f);
                break;
            }
        }

        // 星・月・太陽の回転の設定
        for (auto skybox : universe_skybox) InterpTo(&skybox->rotation.pitch, skybox->rotation.pitch, 90.0f, 10.0f);

        break;
    }
}

void DayNightCycleSkyBox::SetCycleMode(SkyBoxCycleMode cycleMode)
{
    // データに適用（外部呼出し用）
    this->skyboxData.cycleMode = cycleMode;

    // ゲームのセーブデータ更新
    Game::instance->saveData.skyboxData = this->skyboxData;

    // DayNightCycleSkyBoxControllerのコントロールモードを設定
    Game::instance->InstanceCanvas<DayNightCycleSkyBoxController>()->SetControlMode(cycleMode);
}

void DayNightCycleSkyBox::AddSkyBoxControllerSlide(UINT slideValue)
{
    this->skyboxData.slideValue += (byte)slideValue;
}

void DayNightCycleSkyBox::SetOffsetMode(SkyBoxOffsetMode offsetMode)
{
    this->offsetMode = offsetMode;
}

void DayNightCycleSkyBox::EventBeginPlay()
{
    // ユーザー自由設定モード
    if (this->skyboxData.cycleMode == SkyBoxCycleMode::USER_PREFERENCE)
    {
        SkyBoxParam::currentDateTime = USER_PREFERENCE_DATETIME_BY_COLOR_SCENE[(SkyBoxColorScene)this->skyboxData.slideValue];
    }

    this->SetTransform();
}

void DayNightCycleSkyBox::EventTick(float deltaTime)
{
    // 現在時刻と同期モード
    if (this->skyboxData.cycleMode == SkyBoxCycleMode::SYNC_LOCAL_DATETIME)
    {
        // 現在の日付・時刻を取得
        time_t t = time(nullptr);
        tm dt;
        localtime_s(&dt, &t);

        // 現在時刻を適用
        SkyBoxParam::currentDateTime = dt;
    } else

    // ユーザー自由設定モード
    if (this->skyboxData.cycleMode == SkyBoxCycleMode::USER_PREFERENCE)
    {
        SkyBoxParam::currentDateTime = USER_PREFERENCE_DATETIME_BY_COLOR_SCENE[(SkyBoxColorScene)this->skyboxData.slideValue];
    }

    // 現在時刻変数でSkyBoxの状態を設定
    this->SetSkyBoxColorByCurrentDateTimeVar();

    // 位置・回転設定
    this->SetTransform();

    // オフセットモードの設定
    switch (Game::instance->gameStatus)
    {
    case GameStatus::IN_TITLE:
        
        this->offsetMode = SkyBoxOffsetMode::IN_TITLE;
        break;

    case GameStatus::CLEARED:
    case GameStatus::ALL_CLEARED:
        break;

    default:

        this->offsetMode = SkyBoxOffsetMode::__default;
        break;
    }
}
