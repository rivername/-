using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Boza_Logram : MonoBehaviour
{
    const int NUM_CHILDREN = 4;

    public float speed = 1.0f;
    public GameObject logram;
    public GameObject enemyExplosion;

    int i;

    // Start is called before the first frame update
    void Start()
    {
        Instantiate_4Logram();
    }

    // Update is called once per frame
    void Update()
    {
        transform.position -= transform.up * speed * Time.deltaTime;
    }

    void Instantiate_4Logram()
    {
        float x, y;
        float dist = 0.52f;
        for (i = 0; i < NUM_CHILDREN; i++)
        {
            //四方向
            x = Mathf.Cos(i * Mathf.PI / 2f);
            y = Mathf.Sin(i * Mathf.PI / 2f);

            x *= (Mathf.Floor(x) == Mathf.Ceil(x)) ? dist : 0;
            y *= (Mathf.Floor(y) == Mathf.Ceil(y)) ? dist : 0;

            //生成
            Instantiate
                (
                logram, 
                this.transform.position + new Vector3(x, y, 1.0f),
                this.transform.rotation,
                this.transform                       //このオブジェクトを親とする
                ).GetComponent<Logram>().speed = 0;  //子供の速度を0に
        }
    }

    int DestroyAllChildren()
    {
        int destroyCount = 0;
        foreach (Transform childTransform in this.transform)
        {
            //子ログラムの場所に爆発エフェクト生成
            Instantiate(enemyExplosion, childTransform.position, childTransform.rotation).GetComponent<AudioSource>().enabled = false;

            //子ログラム削除
            Destroy(childTransform.gameObject);

            destroyCount++;
        }

        if(destroyCount == NUM_CHILDREN)
        {
            Status.SCORE += 2000;
        }
        else
        {
            Status.SCORE += 300 * destroyCount;
        }
        Debug.Log(destroyCount);
        return destroyCount;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.tag == "BlasterSightPoint")
        {
            if( DestroyAllChildren() != NUM_CHILDREN)
            {
                Status.SCORE += 600;
            }

            //爆発エフェクト生成
            Instantiate(enemyExplosion, transform.position, transform.rotation);

            Status.KILLS_BY_BLASTER++;

            //削除
            Destroy(this.gameObject);
        }
    }
}
