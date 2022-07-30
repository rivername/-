using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GiddoSpario : MonoBehaviour
{
    public GameObject enemyExplosion;
    public float speed = 8f;
    GameObject target;
    float rad;
    Vector3 giddoVec;

    // Start is called before the first frame update
    void Start()
    {
        //自機のオブジェクト取得
        target = GameObject.FindGameObjectWithTag("Player");
        //自機との角度計算
        rad = Mathf.Atan2(target.transform.position.y - this.transform.position.y, target.transform.position.x - this.transform.position.x);

        giddoVec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);
        giddoVec.Normalize();
    }


    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position += giddoVec * speed * Time.deltaTime;
    }


    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.tag == "Zapper")
        {
            Instantiate(enemyExplosion, transform.position, transform.rotation);

            //スコア
            Status.SCORE += 10;

            Destroy(collision.gameObject);
            Destroy(this.gameObject);
        }
    }


    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
