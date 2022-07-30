using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Logram : MonoBehaviour
{
    private Animator animator;
    private AnimationClip[] clips;

    public GameObject enemyBullet;        //弾
    public GameObject enemyExplosion;     //爆発

    public float  speed = 1f;
    private int fireCnt;            //発射回数カウント   ... Fire()内で加算
    private bool onFire = false;    //発射フラグ

    void Start()
    {
        animator = gameObject.GetComponent<Animator>();

        //全てのアニメーションクリップを配列に取得
        //clip[0]  ...  ログラムcol0.anim
        //clip[1]  ...  ログラムcol1.anim
        //clip[2]  ...  ログラムcol2.anim
        //clip[3]  ...  ログラムcol3.anim
        //clip[4]  ...  ログラムcol4.anim
        clips = animator.runtimeAnimatorController.animationClips;

        fireCnt = 0;
    }

    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;

    }

    /// <summary>
    /// 発射アニメーション開始制御
    /// </summary>
    IEnumerator Start_fire()
    {
        float coolTime = Random.Range(0.0f, 1.0f);
        while (true)
        {
            yield return new WaitForSeconds(coolTime);
            if (GameObject.FindGameObjectWithTag("Player") != null && fireCnt < 2)
            {
                onFire = true;
                StartCoroutine(Fire_animation());
                coolTime = Random.Range(2.0f, 3.0f);
            }
            else
                yield break;
        }
    }

    /// <summary>
    /// 発射アニメーション
    /// </summary>
    IEnumerator Fire_animation()
    {
        int col_index = Random.Range(0, 4);
        float normalizedTime = 0;
        float animLength = clips[col_index].length;
        while (true)
        {
            if (normalizedTime < 1.0f)
            {
                animator.Play(clips[col_index++].name, -1, normalizedTime);
                if (col_index > 4) col_index = 0;

                normalizedTime += 1.0f / 5.0f;

                yield return new WaitForSeconds(animLength / 5.0f);
                continue;
            }
            animator.Play("ログラム_待機");
            yield break;
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
            fireCnt++;
            Instantiate(enemyBullet, this.transform.position, this.transform.rotation);
        }
    }

    void OnTriggerEnter2D(Collider2D c)
    {
        if (c.tag == "BlasterSightPoint")
        {
            //爆発エフェクト生成
            Instantiate(enemyExplosion, transform.position, transform.rotation);

            //削除
            Destroy(gameObject);

            Status.KILLS_BY_BLASTER++;

            //スコア加算
            Status.SCORE += 300;
        }
    }

    private void OnBecameVisible()
    {
        StartCoroutine(Start_fire());
    }

    void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
