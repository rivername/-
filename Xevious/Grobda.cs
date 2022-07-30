using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Grobda : MonoBehaviour
{
    public enum Mode
    {
        [InspectorName("200点(不動・エリア12以外)")]STOP,                                                        //200点(不動・エリア12以外)
        [InspectorName("400点(常に前進)")] PRODRESS,                                                             //400点(常に前進)
        [InspectorName("600点(停止・照準を感知して前進)")] STOP_SCOPENOTINCING_PROGRESS,                         //600点(停止・照準を感知して前進)
        [InspectorName("1000点(前進・照準を感知して停止・前進)")] PROGRESS_SCOPENOTINCING_STOP,                  //1000点(前進・照準を感知して停止・前進)
        [InspectorName("1500点(停止・ブラスターを感知して後退・停止)")] STOP_BRASTERNOTINCING_BACK,              //1500点(停止・ブラスターを感知して後退・停止)
        [InspectorName("1500点(不動・エリア12のみ)")] STOP_AREA12,                                               //1500点(不動・エリア12のみ)
        [InspectorName("2000点(前進・照準を感知して高速前進・前進)")] PROGRESS_SCOPENOTINCING_HIGHSPEED,         //2000点(前進・照準を感知して高速前進・前進)
        [InspectorName("2500点(前進・ブラスターを感知して後退・前進)")] PROGRESS_BRASTERNOTINCING_BACK,          //2500点(前進・ブラスターを感知して後退・前進)
        [InspectorName("10000点(停止・ブラスターを感知して高速前進・停止)")] STOP_BRASTERNOTINCING_HIGHSPEED     //10000点(停止・ブラスターを感知して高速前進・停止)
    }

    public enum Move
    {
        STOP,       //停止
        PROGRESS,   //前進
        BACK,       //後退
        HISPEED     //加速
    }

    
    public Mode mode;                   // 列挙型の列挙値を格納する変数
    public GameObject expPrefab;        //爆発アニメーション用プレファブ

    private Animator anim;              //アニメーター
    private GameObject blaster;         //ブラスター
    private Vector2 cameMin;            //カメラ下座標
    private Vector3 groMax;             //ブラスター感知座標MAX
    private Vector3 groMin;             //ブラスター感知座標MIN
    private Vector3 range;              //ブラスター感知範囲
    private float speed;                // 移動スピード
    private bool Snotincing;            //スコープ感知フラグ
    private bool Bnotincing;            //ブラスター感知フラグ
    private bool inDisplay;             //カメラに入ったフラグ

    void Start()
    {
        //カメラ下座標
        cameMin = Camera.main.ViewportToWorldPoint(new Vector2(0, 0));

        //アニメ―ター
        anim = gameObject.GetComponent<Animator>();

        //ブラスター初期化
        blaster = null;

        //サイト判定
        Snotincing = false;

        //ブラスター判定
        Bnotincing = false;

        //カメラにはいっているかの判定
        inDisplay = false;

        //ブラスター判定範囲
        range = new Vector3(2, 2, 0);
    }

    void Update()
    {
        //ディスプレイに入ってない場合は、マップと同じ速度
        if (inDisplay == false)
        {
            Stop();
        }

        //マップの入ったらmodeごとの処理
        else
        {
            //ブラスター判定範囲
            groMax = transform.position + range;
            groMin = transform.position - range;

            //ブラスターのオブジェクト取得
            if (FindObjectOfType<Blaster>() != null)
            {
                blaster = FindObjectOfType<Blaster>().gameObject;
                //Debug.Log("座標とったよ！");
            }
            else//ない場合の処理
            {
                blaster = null;
            }

            //enemyのy座標がカメラ下部を超えたら
            if (transform.position.y < cameMin.y)
            {
                //enemyを削除
                Destroy(this.gameObject);
            }

            //モードごとの処理
            switch (mode)
            {
                case Mode.STOP:
                    Stop();
                    break;

                case Mode.PRODRESS:
                    Progress();
                    break;

                case Mode.STOP_SCOPENOTINCING_PROGRESS:
                    ScopeNotincing(Move.STOP, Move.PROGRESS);
                    break;

                case Mode.PROGRESS_SCOPENOTINCING_STOP:
                    ScopeNotincing(Move.PROGRESS, Move.STOP);
                    break;

                case Mode.STOP_BRASTERNOTINCING_BACK:
                    BrasterNotincing(Move.STOP, Move.BACK);
                    break;

                case Mode.STOP_AREA12:
                    Stop();
                    break;

                case Mode.PROGRESS_SCOPENOTINCING_HIGHSPEED:
                    ScopeNotincing(Move.PROGRESS, Move.HISPEED);
                    break;

                case Mode.PROGRESS_BRASTERNOTINCING_BACK:
                    BrasterNotincing(Move.PROGRESS, Move.BACK);
                    break;

                case Mode.STOP_BRASTERNOTINCING_HIGHSPEED:
                    BrasterNotincing(Move.STOP, Move.HISPEED);
                    break;
            }
        }

        
    }

    //enemyが弾に当たった時
    void OnTriggerEnter2D(Collider2D c)
    {
        if (c.tag == "BlasterSightPoint")
        {
            //exePrefab(爆破エフェクト)を再生
            Instantiate(expPrefab, transform.position, transform.rotation);

            switch (mode)
            {
                case Mode.STOP:
                    Status.SCORE += 200;
                    break;

                case Mode.PRODRESS:
                    Status.SCORE += 400;
                    break;

                case Mode.STOP_SCOPENOTINCING_PROGRESS:
                    Status.SCORE += 600;
                    break;

                case Mode.PROGRESS_SCOPENOTINCING_STOP:
                    Status.SCORE += 1000;
                    break;

                case Mode.STOP_BRASTERNOTINCING_BACK:
                    Status.SCORE += 1500;
                    break;

                case Mode.STOP_AREA12:
                    Status.SCORE += 1500;
                    break;

                case Mode.PROGRESS_SCOPENOTINCING_HIGHSPEED:
                    Status.SCORE += 2000;
                    break;

                case Mode.PROGRESS_BRASTERNOTINCING_BACK:
                    Status.SCORE += 2500;
                    break;

                case Mode.STOP_BRASTERNOTINCING_HIGHSPEED:
                    Status.SCORE += 10000;
                    break;
            }

            //enemyを削除
            Destroy(this.gameObject);

            Status.KILLS_BY_BLASTER++;
        }
    }

    /// <summary>
    /// 停止
    /// </summary>
    void Stop()
    {
        speed = 1.0f;

        anim.Play("グロブダー停");

        //enemy座標の更新
        transform.position += transform.up * -speed * Time.deltaTime;
    }

    /// <summary>
    /// 前進
    /// </summary>
    void Progress()
    {
        speed = 1.5f;

        anim.Play("グロブダー進");

        //enemy座標の更新
        transform.position += transform.up * -speed * Time.deltaTime;
    }

    /// <summary>
    /// 後退
    /// </summary>
    void Back()
    {
        speed = 0.5f;

        anim.Play("グロブダー進");

        //enemy座標の更新
        transform.position += transform.up * speed * Time.deltaTime;
    }

    /// <summary>
    /// 高速前進
    /// </summary>
    void HighSpeed()
    {
        speed = 3.0f;

        anim.Play("グロブダー進");

        //enemy座標の更新
        transform.position += transform.up * -speed * Time.deltaTime;
    }

    /// <summary>
    /// 照準感知
    /// </summary>
    /// <param name="move">正常時の動き、引数はenum Move</param>
    /// <param name="aftmove">感知時の動き、引数はenum Move</param>
    void ScopeNotincing(Move move ,Move aftmove)
    {

        if (Xevious.player_HOVERED_ON_GroundEnemy)
        {
            Snotincing = true;
        }
        else
        {
            if (Snotincing)
            {
                //サイトが外れてから前進する
                StartCoroutine("StaySecond_sc", (int)2);
            }
        }

        if (!Snotincing)//非感知時
        {
            switch (move)
            {
                case Move.STOP:
                    Stop();
                    break;

                case Move.PROGRESS:
                    Progress();
                    break;

                case Move.BACK:
                    Back();
                    break;

                case Move.HISPEED:
                    HighSpeed();
                    break;
            }
        }
        else//感知時
        {
            switch (aftmove)
            {
                case Move.STOP:
                    Stop();
                    break;

                case Move.PROGRESS:
                    Progress();
                    break;

                case Move.BACK:
                    Back();
                    break;

                case Move.HISPEED:
                    HighSpeed();
                    break;
            }
        }
    }

    /// <summary>
    /// ブラスター感知
    /// </summary>
    /// <param name="move">正常時の動き、引数はenum Move</param>
    /// <param name="aftmove">感知時の動き、引数はenum Move</param>
    void BrasterNotincing(Move move ,Move aftmove)
    {

        if(blaster != null)
        {
             if(blaster.transform.position.y >= groMin.y && blaster.transform.position.x >= groMin.x &&
                blaster.transform.position.y <= groMax.y && blaster.transform.position.x <= groMax.x){

                Bnotincing = true;
                //Debug.Log("弾が近いよ！");
            }
        }
        else
        {
            if (Bnotincing)
            {
                //サイトが外れてから前進する
                StartCoroutine("StaySecond_bla", (int)2);
            }
        }

        if (!Bnotincing)//非感知時
        {
            switch (move)
            {
                case Move.STOP:
                    Stop();
                    break;

                case Move.PROGRESS:
                    Progress();
                    break;

                case Move.BACK:
                    Back();
                    break;

                case Move.HISPEED:
                    HighSpeed();
                    break;
            }
        }
        else//感知時
        {
            switch (aftmove)
            {
                case Move.STOP:
                    Stop();
                    break;

                case Move.PROGRESS:
                    Progress();
                    break;

                case Move.BACK:
                    Back();
                    break;

                case Move.HISPEED:
                    HighSpeed();
                    break;
            }
        }
    }

    /// <summary>
    /// ブラスター判定用　引数(int)秒待機する。
    /// </summary>
    /// <param name="sec">待機する秒数</param>
    /// <returns></returns>
    IEnumerator StaySecond_bla(int sec)
    {
        yield return new WaitForSeconds(sec);

        if (blaster == null)
        {
            Bnotincing = false;
        }

    }

    /// <summary>
    /// スコープ判定用　引数(int)秒待機する。
    /// </summary>
    /// <param name="sec">待機する秒数</param>
    /// <returns></returns>
    IEnumerator StaySecond_sc(int sec)
    {
        yield return new WaitForSeconds(sec);

        if (!Xevious.player_HOVERED_ON_GroundEnemy)
        {
            Snotincing = false;
        }
    }

    //ディスプレイに入ったら呼び出される
    private void OnBecameVisible()
    {
        //ディスプレイ入った
        inDisplay = true;
    }
}

