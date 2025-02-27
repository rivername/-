﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BragZakato : MonoBehaviour
{
    //2種のモード
    public enum Mode
    {
        AUTO_DETHOUT,
        APPROACH_DETHOUT
    }
    public Mode mode;

    //アニメーションの状態
    public enum AnimStatus
    {
        APPEAR,    //出現中
        MOVE,      //移動中
        DISAPPEAR  //消滅中
    }
    private AnimStatus animStatus;

    private Animator anim;
    private BoxCollider2D boxCollider2D;
    public GameObject enemyBullet;     //スパリオ
    public GameObject enemyExplosion;  //空中敵爆発
    private GameObject target;         //自機
    float rad;                         //自機との角度
    Vector3 ApproachVec;               //自機へのベクトル

    float speed;                 //速度
    float appearDist;            //出現時間
    Vector3 appearPos;           //出現時座標

    bool endAppear = false;   //出現アニメーション終了
    bool endMove = false;   //移動アニメーション終了
    bool endDisappear = false;   //消滅アニメーション終了

    const float DISAPPEAR_DIST = 4.0f;  //消滅までの自機との距離


    // Start is called before the first frame update
    void Start()
    {
        SetMode();

        //最初の状態は "出現中"
        animStatus = AnimStatus.APPEAR;
        //出現時の座標保存
        appearPos = transform.position;

        //アニメーション
        anim = GetComponent<Animator>();

        //コライダー
        boxCollider2D = GetComponent<BoxCollider2D>();
        //最初は判定なし
        boxCollider2D.enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        //状態別
        if (animStatus == AnimStatus.APPEAR)
        {
            Appear();
        }
        if (animStatus == AnimStatus.MOVE)
        {
            Move();
        }
        else
        {
            Disappear();
        }
    }

    /*********************************************************************
     * 処理内容 移動&消滅モード設定、各値設定
     * 引き数   無し
     * 戻り値   無し
     /*********************************************************************/
    void SetMode()
    {
        switch (mode)
        {
            case Mode.AUTO_DETHOUT:
                appearDist = Random.Range(2, 3);
                speed = 4.0f;
                break;

            case Mode.APPROACH_DETHOUT:
                speed = 4.0f;
                break;
        }
    }

    /*********************************************************************
     * 処理内容　出現中アニメーション時の処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void Appear()
    {
        if (endAppear) //出現アニメーションが終わった
        {
            anim.Play("ブラグザカート");
            //移動中は当たり判定あり
            boxCollider2D.enabled = true;
            animStatus = AnimStatus.MOVE;


            GetTarget();
        }
    }

    /*********************************************************************
     * 処理内容 自機のオブジェクト取得、自機へのベクトル計算
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void GetTarget()
    {
        //自機のオブジェクト取得
        target = GameObject.FindGameObjectWithTag("Player");
        //自機との角度計算
        rad = Mathf.Atan2(target.transform.position.y - this.transform.position.y, target.transform.position.x - this.transform.position.x);

        //まとめてノーマライズ
        ApproachVec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);
        ApproachVec.Normalize();
    }

    /*********************************************************************
     * 処理内容　移動中アニメーション時の処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void Move()
    {
        if (endMove) //移動アニメーションが終わった
        {
            Fire_FiveDirections();
            //SE再生
            GetComponent<AudioSource>().Play();
            //アニメーション切り替え
            anim.Play("ザカート_消滅");
            //消滅アニメーション時は当たり判定なし
            boxCollider2D.enabled = false;
            //状態切り替え
            animStatus = AnimStatus.DISAPPEAR;

            return;
        }

        //モード別移動処理
        switch (mode)
        {
            case Mode.AUTO_DETHOUT:
                Move_AutoDethout();
                break;
            case Mode.APPROACH_DETHOUT:
                Move_ApproachDethout();
                break;
        }
    }


    void Fire_FiveDirections()
    {
        rad = Mathf.Atan2(target.transform.position.y - this.transform.position.y, target.transform.position.x - this.transform.position.x);
        for (int i = -2; i <= 2; i++)
        {
            //弾生成
            Instantiate(enemyBullet, transform.position, Quaternion.Euler(new Vector3(0, 0, rad * 180.0f / Mathf.PI + 90f + i*20)));
        }
    }

    /*********************************************************************
     * 処理内容　"自動で消滅" モードの処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void Move_AutoDethout()
    {
        //移動
        transform.position += ApproachVec * speed * Time.deltaTime;

        //移動終了条件
        if (Vector3.Distance(appearPos, transform.position) >= appearDist)
        {
            endMove = true;
        }
    }

    /*********************************************************************
     * 処理内容　"自機に接近で消滅" モードの処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void Move_ApproachDethout()
    {
        //移動
        transform.position += ApproachVec * speed * Time.deltaTime;

        //移動終了条件
        Vector2 thisVec2 = new Vector2(transform.position.x, transform.position.y);
        Vector2 targetVec2 = new Vector2(target.transform.position.x, target.transform.position.y);
        if (Vector2.Distance(thisVec2, targetVec2) <= DISAPPEAR_DIST)
        {
            endMove = true;
        }
    }

    /*********************************************************************
     * 処理内容　消滅アニメーション時の処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void Disappear()
    {
        if (endDisappear)  //消滅アニメーションが終わった
        {
            Destroy(this.gameObject);
        }
    }

    /*********************************************************************
     * 処理内容　出現アニメーション終了のフラグ設定
     * 引き数   無し
     * 戻り値   無し
     * 備考   　アニメーションイベントで呼び出し
     *********************************************************************/
    public void GetEndAppear()
    {
        endAppear = true;
    }

    /*********************************************************************
     * 処理内容　消滅アニメーション終了のフラグ設定
     * 引き数   無し
     * 戻り値   無し
     * 備考   　アニメーションイベントで呼び出し
     *********************************************************************/
    public void GetEndDisappear()
    {
        endDisappear = true;
    }

    /*********************************************************************
     * 処理内容　モード別のスコア加算処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    private void ScoreManager()
    {
        switch (mode)
        {
            case Mode.AUTO_DETHOUT:
                Status.SCORE += 600;
                break;
            case Mode.APPROACH_DETHOUT:
                Status.SCORE += 1500;
                break;
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.tag == "Zapper")
        {
            //爆発エフェクト生成
            Instantiate(enemyExplosion, transform.position, transform.rotation);

            //モード別スコア加算
            ScoreManager();

            //自機弾、ザカート削除
            Destroy(collision.gameObject);
            Destroy(this.gameObject);
        }
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
