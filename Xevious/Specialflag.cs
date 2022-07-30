using System.Collections;
using System.Collections.Generic;
using JetBrains.Annotations;
using UnityEngine;

public class Specialflag : MonoBehaviour
{
    //移動速度
    public float speed = 1f;

    private bool spawned;

    private void Start()
    {
        GetComponent<SpriteRenderer>().enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        //enemy座標の更新
        transform.position -= transform.up * speed * Time.deltaTime;

        //カメラ左下の座標を取得
        Vector2 cameraMin = Camera.main.ViewportToWorldPoint(new Vector2(0, 0));
        Vector2 cameraMax = Camera.main.ViewportToWorldPoint(new Vector2(1, 1));

        //enemyのy座標がカメラ下部を超えたら
        if (transform.position.y < cameraMin.y)
        {
            //enemyを削除
            Destroy(this.gameObject);
        }

        /* カメラの上部に入ったら */
        if (transform.position.y >= cameraMax.y)
        {
            if (!spawned)
            {
                spawned = true;

                /* プレイヤーが右にいたら */
                if (Xevious.mainPlayer.transform.position.x > 0)
                {
                    /* 左端から中央のランダム */
                    float randomX = Random.Range(-6.0f, 0.0f);
                    Vector2 position = new Vector2(randomX, transform.position.y);
                    transform.position = position;
                }
                /*  プレイヤーが左にいたら*/
                else
                {
                    /* 右端から中央のランダム */
                    float randomX = Random.Range(0.0f, 6.0f);
                    Vector2 position = new Vector2(randomX, transform.position.y);
                    transform.position = position;
                }
            }
        }
    }

    void OnTriggerEnter2D(Collider2D c)
    {
        if (c.tag == "BlasterSightPoint")
        {
            if (!GetComponent<SpriteRenderer>().enabled)
            {
                Status.SCORE += 1000;
            }
            else 
            {
                GetComponent<PolygonCollider2D>().enabled = false;
            }

            GetComponent<SpriteRenderer>().enabled = true;
        }
    }
}
