using System.Collections;
using System.Collections.Generic;
using System.Security.Cryptography;
using UnityEngine;

public class BozaLogram : MonoBehaviour
{
    // 子ログラムの攻撃開始用
    private struct CHILDANIMATION
    {
        public Animator anim;  //アニメーター
        public int  coolTime;  //クールタイム
        public int  fireCount; //攻撃回数カウント
        public bool onFire;    //攻撃フラグ
    }
    private CHILDANIMATION[] childAnimations;  //子ログラムの攻撃開始用
    private const int NUM_CHILDREN = 4;        //子ログラムの数(定数)
    private int   i;
    private bool  onFire = false;              //ボザ・ログラム全体の攻撃フラグ
    private bool  awakeFlag = false;           //ボザ・ログラムが画面に入ったかどうか
    private bool  attackEnd = false;           //ボザ・ログラム全体の攻撃終了フラグ
    private bool  bozaExplosionFlag = false;   //ボザ・ログラムのコアの爆発フラグ

    private ChildLogram[] childScript;         //子ログラムのスクリプト

    public GameObject enemyExplosion;    //爆発
    public float  speed = 1f;

    // Start is called before the first frame update
    void Start()
    {
        childAnimations = new CHILDANIMATION[NUM_CHILDREN];
        // 初期化
        for (i = 0; i < NUM_CHILDREN; i++)
        {
            childAnimations[i].coolTime = Random.Range(75, 125);
            childAnimations[i].fireCount = 0;
            childAnimations[i].onFire = false;
            childAnimations[i].anim = transform.GetChild(i).gameObject.GetComponent<Animator>();
        }


        childScript = new ChildLogram[NUM_CHILDREN];
        for (i = 0; i < NUM_CHILDREN; i++)
        {
            childScript[i] = transform.Find("子ログラム" + i).gameObject.GetComponent<ChildLogram>();
        }
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;

        //子ログラムの爆発処理
        ChildlogramExplosion();

        //攻撃開始条件
        if (awakeFlag && !attackEnd && (transform.position.y < Random.Range(3f, 4f))) 
        {
            onFire = true;
        }


        // 各 子ログラムがランダムに攻撃開始する
        if (onFire)
        {
            if (GameObject.FindGameObjectWithTag("Player") != null) //Player生存時
            {
                for (i = 0; i < NUM_CHILDREN; i++)
                {
                    if (transform.Find("子ログラム" + i) == null)
                    {
                        //子ログラムが破壊されていたら次ループへ
                        continue;
                    }

                    //クールタイム終わった、攻撃回数2回未満 のとき
                    if ((childAnimations[i].coolTime-- <= 0) && (childAnimations[i].fireCount < 2))
                    {
                        //アニメーションスタート
                        childAnimations[i].anim.Play("ログラムcol"+ Random.Range(0, 5));
                        //クールタイム再設定
                        childAnimations[i].coolTime = 125;
                        //攻撃回数加算
                        childAnimations[i].fireCount++;
                        //子ログラムのスクリプトの攻撃フラグオン
                        childScript[i].OnFire = true;
                    }
                }

                for(i = 0; i < NUM_CHILDREN; i++)
                {
                    if (childAnimations[i].fireCount < 2)
                    {
                        break;
                    }
                }
                if(i >= NUM_CHILDREN)  // 攻撃終了
                {
                    onFire = false;
                    attackEnd = true;
                }
            }
        }
    }

    /*********************************************************************
     * 処理内容 子ログラムの爆発・削除処理
     * 引き数   無し
     * 戻り値   無し
     *********************************************************************/
    void ChildlogramExplosion()
    {
        Transform tmpChildTransform;   //子ログラムのトランスフォーム

        for (i = 0; i < NUM_CHILDREN; i++)
        {
            if (childScript[i].ExplosionFlag || bozaExplosionFlag)
            {
                //子ログラムのTransformを取得
                //子ログラムが破壊されていたら次ループへ
                if ((tmpChildTransform = transform.Find("子ログラム" + i)) == null)
                {
                    //子ログラムが破壊されていたら次ループへ
                    continue;
                }

                //子ログラムの場所に爆発エフェクト生成
                Instantiate(enemyExplosion, tmpChildTransform.position, tmpChildTransform.rotation).GetComponent<AudioSource>().enabled = !bozaExplosionFlag;

                //スコア
                Status.SCORE += 300;

                //子ログラム削除
                Destroy(tmpChildTransform.gameObject);
            }
        }
    }

    void OnTriggerEnter2D(Collider2D c)
    {
        if (c.tag == "BlasterSightPoint")
        {
            bozaExplosionFlag = true;
            ChildlogramExplosion();

            //コアの爆発エフェクト生成
            Instantiate(enemyExplosion, transform.position, transform.rotation);

            //スコア
            switch (transform.childCount)
            {
                case NUM_CHILDREN:       //子が4つ生存時
                    Status.SCORE += 2000;
                    break;
                default:                 //子が1つでも死亡時
                    Status.SCORE += 600;
                    break;
            }

            //コアを削除
            Destroy(this.gameObject);
        }
    }

    private void OnBecameVisible()
    {
        awakeFlag = true;
    }

    void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
