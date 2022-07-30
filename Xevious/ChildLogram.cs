using System.Collections;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;


public class ChildLogram : MonoBehaviour
{
    public GameObject enemyBullet;       //弾
    public GameObject enemyExplosion;    //爆発
    private Animator anim;

    private bool explosionFlag;   
    public  bool ExplosionFlag           //ボザログラムからもアクセスする
    {
        get { return explosionFlag; }

        set { explosionFlag = value; }
    }

    private bool onFire;
    public bool OnFire
    {
        get { return onFire; }
        set { onFire = value; }
    }
    

    // Start is called before the first frame update
    void Start()
    {
        anim = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
    }

    /*********************************************************************
     * 処理内容 アニメーション切り替え(色)
     * 引き数   無し
     * 戻り値   無し
     * 備考   　アニメーションイベントで呼び出し
     *********************************************************************/
    public void ExchangeAnim()
    {
        int nextNumber;
        float normalizedTime;
        string name = null;

        //再生中クリップの名前取得

        try
        {
            name = anim.GetCurrentAnimatorClipInfo(0)[0].clip.name;
        }
        catch (System.IndexOutOfRangeException)
        {
        }


        if (name == null) return;

        //名前から番号を取得    例 => "ログラムcol3" の '3'
        nextNumber = int.Parse(name.Substring(name.Length - 1, 1)) + 1;
        if (nextNumber > 4)
        {
            nextNumber = 0;
        }

        //現在の再生時間取得
        normalizedTime = anim.GetCurrentAnimatorStateInfo(0).normalizedTime;
        if (normalizedTime >= 1) // 再生終了
        {
            anim.Play("New State");
        }
        else
        {
            anim.Play("ログラムcol" + nextNumber, -1, normalizedTime + 0.01f);
        }
    }

    /*********************************************************************
     * 処理内容 スパリオ発射
     * 引き数   無し
     * 戻り値   無し
     * 備考   　アニメーションイベントで呼び出し
     *********************************************************************/
    public void Fire()
    {
        if (onFire)
        {
            onFire = false;
            Instantiate(enemyBullet, this.transform.position, this.transform.rotation);
        }
    }

    void OnTriggerEnter2D(Collider2D c)
    {
        if (c.tag == "BlasterSightPoint")
        {
            //爆発・Destroyは親側で
            explosionFlag = true;
        }
    }

    void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
