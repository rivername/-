using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Soru : MonoBehaviour
{
    //移動速度
    public float speed = 1f;

    /* ヒットカウント */
    private int hitCount = 0;

    //爆発アニメーション用プレファブ
    public GameObject expPrefab;

    void Update()
    {
        //enemy座標の更新
        transform.position -= transform.up * speed * Time.deltaTime;

        //カメラ左下の座標を取得
        Vector2 min = Camera.main.ViewportToWorldPoint(new Vector2(0, 0));

        //enemyのy座標がカメラ下部を超えたら
        if (transform.position.y < min.y)
        {
            //enemyを削除
            Destroy(this.gameObject);
        }

        /* 1回目のヒットの時 */
        if (hitCount == 1)
        {
            Animator animator = GetComponent<Animator>();
            AnimatorStateInfo info = animator.GetCurrentAnimatorStateInfo(0);
            float time = info.normalizedTime;

            if (time >= 1.0f)
            {
                /* コライダーを有効化 */
                GetComponent<BoxCollider2D>().enabled = true;
            }
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        //Debug.Log("hit");
        /* ブラスターと当たった時 */
        if (collision.tag == "BlasterSightPoint")
        {
            //Debug.Log("hit");

            /* hitCountをプラス */
            hitCount++;

            if (hitCount == 1)
            {
                GetComponent<BoxCollider2D>().enabled = false;
                GetComponent<SpriteRenderer>().enabled = true;
                GetComponent<Animator>().enabled = true;

                Status.SCORE += 2000;
            }

            if (hitCount == 2)
            {

                Vector2 cPos = GetComponent<BoxCollider2D>().bounds.center;

                //exePrefab(爆破エフェクト)を再生
                Instantiate(expPrefab, cPos, transform.rotation);

                Status.SCORE += 2000;
                Status.KILLS_BY_BLASTER++;

                Destroy(gameObject);
                
            }
        }
    }
}

