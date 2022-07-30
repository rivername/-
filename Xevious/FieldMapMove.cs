using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FieldMapMove : MonoBehaviour
{
    private Vector2 cameraUp;  //カメラの上座標
    private Vector2 cameraDown;  //カメラの下座標

    public float speed = 1.0f;  //スクロールスピード
    float MapHeight;  //マップの高さ
    float MapCountstr;//マップ分割の長さ
    double MapMath;//マップのEnemy出現管理座標
    float once;  //一度だけ用


    void Start()
    {
        
        //マップの座標上の高さ
        MapHeight = 81.9f;

        //出現用マップナンバーの1マス
        MapCountstr = MapHeight / 256;
        
        //一度用の初期化
        once = 0;
        
        //カメラの座標
        cameraUp = Camera.main.ViewportToWorldPoint(new Vector2(1, 1));
        cameraDown = Camera.main.ViewportToWorldPoint(new Vector2(0, 0));

        //地上敵の配置
        SetEnemy(Status.STAGE_NUMBER);

    }

    void Update()
    {
        //座標の更新
        transform.GetChild(0).position -= transform.GetChild(0).up * speed * Time.deltaTime;

        //マップの上座標がカメラの上座標より下に言ったら
        if (transform.GetChild(0).position.y + (MapHeight/2) <= cameraUp.y)
        {
            //一度だけ
            if (once == 0)
            {
                //ステージをプラス
                Status.STAGE_NUMBER++;

                //マップの描画状態の変更
                Map_controller.AppMap = false;

                once++;
            }
        }
        else
        {
            //マップポジション
            Status.MAP_POSITION = (int)Math.Ceiling((double)((transform.GetChild(0).transform.position.y - (MapHeight/2) - 5) / MapCountstr)) * -1;
            
            //0以下は0に
            if (Status.MAP_POSITION <= 0) Status.MAP_POSITION = 0;

        }

            //マップの全体が画面外に出たら
        if (transform.GetChild(0).transform.position.y + (MapHeight/2) <= cameraDown.y)
        {
            Destroy(this.gameObject);
        }
    }

    void SetEnemy(int stage)
    {
        //子オブジェクトの数(マップの分-1)
        int area_num;
        area_num = transform.childCount;

        //全てのAREAの非表示
        for (int i = 1; i < area_num; i++)
        {
            transform.GetChild(i).gameObject.SetActive(false);
        }

        //該当AREAの表示
        transform.GetChild(stage).gameObject.SetActive(true);


    }
}
