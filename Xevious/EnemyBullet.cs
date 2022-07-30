using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyBullet : MonoBehaviour
{
    private GameObject target;  //自機
    public float speed = 4f;
    private Vector3 bulletVec;  //自機に向かうベクトル

    // Start is called before the first frame update
    void Start()
    {
        float rad;

        //自機のオブジェクト所得
        target = GameObject.FindGameObjectWithTag("Player");

        //自機がいなかったら発射前の弾は削除
        if(target == null)
        {
            Destroy(this.gameObject);
            return;
        }

        //自機との角度計算
        rad = Mathf.Atan2(target.transform.position.y - this.transform.position.y, target.transform.position.x - this.transform.position.x);

        //ベクトルにまとめてノーマライズ
        bulletVec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);
        bulletVec.Normalize();
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position += bulletVec * speed * Time.deltaTime;
    }

    void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
