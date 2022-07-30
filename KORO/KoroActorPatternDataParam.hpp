#include <string>
#pragma once

using std::string;

inline const byte CHAR_SIZE_DEFAULT = 32;
inline const byte CHAR_SIZE_ID      = 32;

inline const string RESOURCES_PATH     = "resources/";
inline const string RESOURCES_TEX_PATH = "textures/";
inline const string RESOURCES_X_PATH   = "x/";

inline const LPCSTR COLORED_CUBE_TEXTURE_PATH = "BaseWood.png";

/*==================================
             Actorタイプ
==================================*/
enum class KoroActorType : byte
{
    CUBE,        // Cube
    X_MODEL,     // X Model
    START_POINT, // スタートポイント
    GOAL_POINT,  // ゴールポイント
    TRIGGER_BOX, // Trigger Box

    /// ----:
    __num
};

/*==================================
         Cube情報 : ファイルIO用
==================================*/
struct KoroCubeInfoByte
{
    // Cubeタイプ
    enum class Type : byte
    {
        COLORED,  // 着色積み木Cube
        TEXTURED, // テクスチャ貼り付けCube

        /// ----:
        __num
    } type;

    // 6面対応テクスチャ配列
    char texPaths[6][CHAR_SIZE_DEFAULT];
};
/*==================================
               Cube情報
==================================*/
struct KoroCubeInfo
{
    KoroCubeInfo() {}

    // KoroCubeInfo = KoroCubeInfoByte
    // 変換コンストラクタ
    KoroCubeInfo(KoroCubeInfoByte _Right) : type(_Right.type)
    {
        // 6枚分変換
        for (int i = 0; i < ARRAYSIZE(this->texPaths); i++)
        {
            // テクスチャが登録されているかどうか
            if (_Right.texPaths[0][0] != '\0')
            {
                // 6面分のテクスチャが登録されているかどうか
                if (_Right.texPaths[i][0] != '\0')
                {
                    // パスが登録されている場合フォルダパスをつけて変換
                    this->texPaths[i] = (RESOURCES_PATH + RESOURCES_TEX_PATH + _Right.texPaths[i]);
                    continue;
                }

                // 面が登録されてない場合や変な文字列の時はテクスチャは0番のテクスチャを適用
                this->texPaths[i] = this->texPaths[0];
            }
            // テクスチャが何も登録されていない
            else
            {
                // タイプがCOLOREDの場合はCOLOREDのテクスチャを設定
                if (this->type == Type::COLORED)
                {
                    this->texPaths[0] = (RESOURCES_PATH + RESOURCES_TEX_PATH + COLORED_CUBE_TEXTURE_PATH);
                    break;
                }
            }
        }
    }

    // Cubeタイプ
    using Type = KoroCubeInfoByte::Type;

    // Cubeタイプ
    Type type;

    // 6面対応テクスチャ配列
    string texPaths[6];
};

/*==================================
       X-Model情報 : ファイルIO用
==================================*/
struct KoroXModelInfoByte
{
    char x_path[CHAR_SIZE_DEFAULT];
    char tex_path[CHAR_SIZE_DEFAULT];
};
/*==================================
              X-Model情報
==================================*/
struct KoroXModelInfo
{
    KoroXModelInfo() {}

    // KoroXModelInfo = KoroXModelInfoByte
    // 変換コンストラクタ
    KoroXModelInfo(KoroXModelInfoByte _Right)
    {
        // パスが登録されている場合フォルダパスをつけて変換
        if (_Right.x_path[0]   != '\0') this->x_path   = RESOURCES_PATH + RESOURCES_X_PATH + _Right.x_path;
        if (_Right.tex_path[0] != '\0') this->tex_path = RESOURCES_PATH + RESOURCES_TEX_PATH + _Right.tex_path;
    }

    string x_path;
    string tex_path;
};

#define COLOR_FIX_RGB_IF_BROKEN(rgb) if (rgb < 0.0f || rgb > 1.0f) rgb = 1.0f
#define COLOR_FIX_IF_BROKEN(color)    \
    COLOR_FIX_RGB_IF_BROKEN(color.r); \
    COLOR_FIX_RGB_IF_BROKEN(color.g); \
    COLOR_FIX_RGB_IF_BROKEN(color.b); \
    COLOR_FIX_RGB_IF_BROKEN(color.a)

/*==================================
    KOROパターンデータ : ファイルIO用
==================================*/
struct KoroActorPatternDataByte
{
    static const byte CHAR_SIZE = 32;

    char id[CHAR_SIZE_ID];         // 固有ID

    KoroActorType      actorType;  // Actorタイプ
    KoroCubeInfoByte   cubeInfo;   // Cube情報
    KoroXModelInfoByte xModelInfo; // X-Model情報

    D3DXCOLOR color;               // 色
};
/*==================================
           KOROパターンデータ
==================================*/
struct KoroActorPatternData
{
    KoroActorPatternData() {}

    // KoroActorPatternData = KoroActorPatternDataByte
    // 変換コンストラクタ
    KoroActorPatternData(KoroActorPatternDataByte _Right)
    {
        // IDをコピー
        strcpy_s(this->id, CHAR_SIZE_ID, _Right.id);

        // 変換呼び出し
        this->actorType  = _Right.actorType;
        this->cubeInfo   = _Right.cubeInfo;
        this->xModelInfo = _Right.xModelInfo;

        // 色設定
        this->color = _Right.color;
        
        // 初期化されてない場合は色修正
        COLOR_FIX_IF_BROKEN(color);
    }

    char id[CHAR_SIZE_ID];     // 固有ID

    KoroActorType  actorType;  // Actorタイプ
    KoroCubeInfo   cubeInfo;   // Cube情報
    KoroXModelInfo xModelInfo; // X-Model情報

    D3DXCOLOR color; // 色
};

inline void a()
{
    sizeof(KoroActorPatternDataByte);
    sizeof(KoroActorPatternData);
    sizeof(KoroActorType);
    sizeof(KoroCubeInfo);
    sizeof(KoroCubeInfo::Type);
}
