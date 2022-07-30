#include "GameObject.hpp"
#include "HLSL_Param.hpp"
#pragma once

using std::string;

class HLSL;

/*==================================
             HLSLをセット
==================================*/
#define SET_HLSL(arg, tag) \
    static auto hlsl = new HLSL(arg, tag); \
    this->SetHLSL(hlsl)

/*==================================
            HLSLオブジェクト
==================================*/
class HLSLObject
{
public:
    // 使用するHLSLシェーダークラスを設定
    void SetHLSL(HLSL* hlsl);

public:
    // 一度のみパラメーターを設定
    // ゲームの中で固定されている値を設定するための関数
    virtual void SetParamOnce(ID3DXEffect* effect) {}

    // パラメーターを設定
    virtual void SetParam(ID3DXEffect* effect) = 0;

    // 描画
    virtual bool Render(ID3DXEffect* effect) = 0;

public:
    // シェーダー基本パラメーター
    HLSL_BasicParam hlslParam = {};

private:
    friend class HLSL;
};

/*==================================
          HLSLシェーダークラス
・GameObjectを継承しているが自動描画はなし
==================================*/
class HLSL : public GameObject
{
public:
    // タグからHLSLシェーダークラスを取得
    static HLSL* GetHLSLByTag(string tag);

    // すべてのHLSLシェーダークラスリスト
    inline static std::vector<HLSL*> all;

public:
    // HLSL : ファイル名から
    // @param filename : ファイル名
    // @param tag      : タグ
    HLSL(string filename, string tag);

    // HLSL : リソースIDから
    // @param resourceID : リソースID
    // @param tag        : タグ
    HLSL(int resourceID, string tag);

    ~HLSL();

private:
    // 初期化
    bool Init(string filename);
    bool Init(int resourceID);

    // 描画開始
    bool BeginRender();

    // 描画
    // @param target : 描画するHLSLObject*
    bool Render(HLSLObject* target);

    // 描画
    // @param targetList : 描画するHLSLObject*リスト
    bool Render(std::vector<HLSLObject*>& targetList);

    // 描画終了
    bool EndRender();

public:
    // 描画一連の流れ実行
    // @param target : 特定のオブジェクトのみを描画する場合のそのポインター
    bool RenderProcess(HLSLObject* target = nullptr);

    // 描画一連の流れ実行
    // @param targetList : 特定のオブジェクトのみを描画する場合のそのポインターリスト
    bool RenderProcess(std::vector<HLSLObject*>& targetList);

public:
    // HLSLObjectをリストから削除
    void RemoveHLSLObjectFromList(HLSLObject* hlslObject);

public:
    // シェーダーエフェクト
    ID3DXEffect* effect;

    // このシェーダーを使うオブジェクト
    std::vector<HLSLObject*> hlslObjects;

public:
    // タグ
    string tag;

    // 自動描画
    bool enableAutoRenderProcess;

private:
    // 一度のみ判定用
    bool setParamOnce;

public:
    virtual void EventBeginPlay()             override;
    virtual void EventRender()                override;
    virtual void EventTick(float deltaTime)   override;
    virtual void EventKeyDown(KeyEvent e)     override {}
    virtual void EventKeyUp(KeyEvent e)       override {}
    virtual void EventMouseDown(MouseEvent e) override {}
    virtual void EventMouseUp(MouseEvent e)   override {}

private:
    friend class HLSLObject;
};
