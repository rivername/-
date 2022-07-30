using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class toloid : MonoBehaviour
{
    private Rigidbody2D toloidkasoku;           //加速逃走用座標
    private Transform target = null;    //自機２
    private Transform targetposition = null;    //自機２
    private Vector3 toloidVec;                  //自機に向かうベクトル
    public GameObject expPrefab;                //爆発アニメーション用プレファブ
    public GameObject enemyBullet; 	            //toloid弾プレファブ
    public float speed = 2.5f;                  //スピード
    public float rad;
    bool kaitenflag = false;                    //回転→逃走用フラグ
    bool toloidbullet = false;                  //弾発射用フラグ
    Animator animator;                          //回転アニメーション
    Vector2 force = new Vector2(0.0f,0.0f);     //加速力
    GameObject p;

    // Start is called before the first frame update
    void Start()
    {
        //アニメーション用
        this.animator = GetComponent<Animator>();

        //加速力用
        toloidkasoku = GetComponent<Rigidbody2D>();

        //自機が配置されていなかった時は削除する
        p = GameObject.Find("自機");
        if (p == null)
        {
            Destroy(gameObject);
            return;
        }

        //出現→自機の方へ移動
        //自機の座標取得
        target = GameObject.Find("自機").transform;
        //自機との角度計算
        rad = Mathf.Atan2(target.position.y - transform.position.y, target.position.x - transform.position.x);
        //ベクトルにまとめてノーマライズ
        toloidVec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);
        toloidVec.Normalize();

        //うごく
        GetComponent<Rigidbody2D>().velocity = toloidVec * speed;

    }

    // Update is called once per frame
    void Update()
    {
        p = GameObject.Find("自機");
        if (p == null)
        {
            toloidkasoku.AddForce(force);   //加速力を加える
            return;
        }
        else
        {
            //自機の座標取り
            targetposition = GameObject.Find("自機").transform;
        }


        //自分と自機のｘ軸の差が0.45f以下になったとき
        if (Mathf.Abs(targetposition.position.x - transform.position.x) <= 0.45f)
            {
                kaitenflag = true;   //逃走フラグオン

                //一回だけ処理
                if (toloidbullet == false)
                {
                    toloidbullet = true;

                    animator.SetTrigger("KaitenFlag");  //回転アニメーション開始

                    //攻撃
                    //弾のプレハブを作成
                    var t = Instantiate(enemyBullet) as GameObject;
                    //弾のプレハブの位置をトーロイドの位置にする
                    t.transform.position = this.gameObject.transform.position;

                    //左右どちらに逃げるか（自機と自分の位置で判断）
                    if (targetposition.position.x >= transform.position.x)
                    {
                        force = new Vector2(-9f, -1f);    //左へ行く加速力
                    }
                    else
                    {
                        force = new Vector2(9f, -1f);    //右へ行く加速力
                    }
                }
            }

        //逃走
        if (kaitenflag == true && p != null)
        {
                toloidkasoku.AddForce(force);   //加速力を加える
        }

    }



    //被撃破処理
    void OnTriggerEnter2D(Collider2D c)
    {

        if (c.tag == "Zapper")
        {
            Instantiate(expPrefab, transform.position, transform.rotation);
            Status.SCORE += 30;     //スコアに＋３０
            Status.KILLS_BY_ZAPPER++;
            Destroy(c.gameObject);  //弾の削除
            Destroy(gameObject);    //トーロイドの削除
        }

    }

    //画面外に出たら消去
    void OnBecameInvisible()
    {
        Destroy(gameObject);
    }

}
