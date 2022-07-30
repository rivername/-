using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Derota : MonoBehaviour
{
    public GameObject enemyBullet;
    public GameObject enemyExplosion;

    private bool garuMode = false;
    public bool GaruMode           //GaruDerotaで設定
    {
        get { return garuMode; }
        set { garuMode = value; }
    }

    Coroutine tmp_coroutine;

    float speed = 1f;                //速度

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;
        if(!garuMode && transform.position.y <= 0)  //中央より下で攻撃停止
        {
            StopCoroutine(tmp_coroutine);
        }
    }

    IEnumerator RandomWait_StartFire()
    {
        yield return new WaitForSeconds(Random.Range(0.0f, 1.0f));
        tmp_coroutine = StartCoroutine(StartFire());
    }

    IEnumerator StartFire()
    {
        while (true)
        {
            Fire();
            yield return new WaitForSeconds(0.25f);
        }
    }

    /*********************************************************************
     * 変数名   Fire
     * 型      void
     * 処理内容 スパリオ発射
     * 引き数   無し
     * 戻り値   無し
     * 備考   　
     *********************************************************************/
    void Fire()
    {
        if (GameObject.FindGameObjectWithTag("Player") != null)
        {
            //弾生成
            Instantiate(enemyBullet, transform.position, transform.rotation);
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.tag == "BlasterSightPoint")
        {
            //ガルデロータは爆破跡つくらない
            Instantiate(enemyExplosion, transform.position, transform.rotation).GetComponent<EnemyExplosion>().BrokenEffectFlag = !garuMode;
            
            //スコア
            switch (garuMode)
            {
                case true:
                    Status.SCORE += 2000;
                    break;
                case false:
                    Status.SCORE += 1000;
                    break;
            }

            Status.KILLS_BY_BLASTER++;
            //削除
            Destroy(this.gameObject);
        }
    }

    private void OnBecameVisible()
    {
        StartCoroutine(RandomWait_StartFire());
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
