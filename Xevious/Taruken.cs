using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Taruken : MonoBehaviour
{
    //アニメーション
    private Animator anm;
 
    //敵のモード　　false = 逃げる　true = 向かってくる
    bool enemyAttack = true;

    //敵の弾
    public GameObject eneBullet;

    public GameObject player;

    //プレイヤーの座標
    private Vector3 playerPos;

    //移動速度
    public float speed = 1.0f;

    //停止位置のランダム
    private float rand;

    //敵の動きベクトル
    Vector3 enemyMove;

    //爆破アニメーション
    public GameObject expPrefab;

    //移動の角度θ
    float theta;


    void Start()
    {

        //プレイヤーの座標
        player = GameObject.FindGameObjectWithTag("Player");

        if(player == null )
        {
            Destroy(this.gameObject);
            return;
        }

        playerPos = player.transform.position;


        //停止位置(64~127の間)
        rand = Random.Range(64.0f,127.0f);


        //rand = 127f;
        //だいたい2倍
        //rand = ((int)rand) * 2;

        //自機とタルケンの角度
        theta = Mathf.Atan2((playerPos.y - transform.position.y), (playerPos.x - transform.position.x));

        //タルケンの動きのベクトル
        enemyMove = new Vector3(Mathf.Cos(theta), Mathf.Sin(theta), 0f);

        //アニメ―ションの取得
        anm = gameObject.GetComponent<Animator>();

    }

    //一度だけ用
    bool once  = false;
    bool once2 = false;

    //フレームカウント
    private float frame = 0.0f;

    //ディスプレイ入ったか
    bool inDisplay = false;


    void Update()
    {
        //ディスプレイに入ったとき   
        if (inDisplay ==  true) {

            //フレームカウント
            frame ++;
        }

        //ランダムフレーム分動いたら
        if (frame >= rand)
        {
            //一回だけ
            if (!once)
            {
                //敵停止
                enemyMove = new Vector3(0, 0, 0);

                //攻撃アニメーション呼び出し
                GetComponent<Animator>().SetBool("attack_trigger",true);

                //弾の発射
                Instantiate(eneBullet, transform.position, transform.rotation);

                //一度だけ
                once = true;
            }

        }

        //敵の攻撃前
        if (enemyAttack == true)
        {
            //自機の初期座標に向かう
            transform.position += enemyMove * speed * Time.deltaTime;
        }
        //敵の攻撃後
        else
        {
            //一度だけ
            if (!once2)
            {
                //プレイヤーのオブジェクト更新
                player = GameObject.FindGameObjectWithTag("Player");

                //プレイヤーがいない場合
                if (player == null)
                {
                    return;
                    
                }

                //プレイヤーの座標更新
                playerPos = player.transform.position;


                //自機とタルケンの角度の更新
                theta = Mathf.Atan2((playerPos.y - transform.position.y), (playerPos.x - transform.position.x));

                //タルケンの動きベクトルの更新
                enemyMove = new Vector3(Mathf.Cos(theta), Mathf.Sin(theta), 0f);

                //一度だけ
                once2 = true;
            }

            //プレイヤーから離れる
            transform.position += enemyMove * -speed * Time.deltaTime;
        }
        //Debug.Log(frame);


    }

    //ディスプレイに入ったら呼び出される
    private void OnBecameVisible()
    {
        //ディスプレイ入った
        inDisplay = true;
    }

    //画面外に出たら
    private void OnBecameInvisible()
    {
        //画面に一度入った後
        if (inDisplay)
        {
            Destroy(this.gameObject);
        }
    }

    //当たり判定
    void OnTriggerEnter2D(Collider2D c)
    {
        //当たった敵のタグがザッパーの場合
        if (c.tag == "Zapper")
        {
            //爆破アニメーションを再生
            Instantiate(expPrefab, transform.position, transform.rotation);

            //エネミーを削除
            Destroy(gameObject);

            //当たった弾を削除
            Destroy(c.gameObject);

            //スコア
            Status.SCORE += 50;

            Status.KILLS_BY_ZAPPER++;
        }
    }

    //攻撃アニメーションの再生の最後に呼び出される
    void attack_END()
    {
        //エネミーを、戻る状態にする。
        enemyAttack = false;
    }
    
}
