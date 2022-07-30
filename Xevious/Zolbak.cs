using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Zolbak : MonoBehaviour
{
    // 移動スピード
    public float speed = 0.5f;
    //爆発アニメーション用プレファブ
    public GameObject expPrefab;

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        //移動方向を決める
        Vector3 direction = transform.up * speed;

        //enemy座標の更新
        transform.position += direction * -speed * Time.deltaTime;

        //カメラ左下の座標を取得
        Vector2 min = Camera.main.ViewportToWorldPoint(new Vector2(0, 0));
        
        //enemyのy座標がカメラ下部を超えたら
        if (transform.position.y < min.y)
        {
            //enemyを削除
            Destroy(this.gameObject);
        }
    }


    //enemyが弾に当たった時
    void OnTriggerEnter2D(Collider2D c)
    {

        if (c.tag == "BlasterSightPoint")
        {
            //exePrefab(爆破エフェクト)を再生
            Instantiate(expPrefab, transform.position, transform.rotation);

            Status.SCORE += 200;

            _ = (Status.DIFFICULTY <= 2) ? (Status.DIFFICULTY = 0) : (Status.DIFFICULTY -= 2);

            Destroy(gameObject);

            Status.KILLS_BY_BLASTER++;
        }
    }
}
