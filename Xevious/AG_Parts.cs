using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AG_Parts : MonoBehaviour
{
    public GameObject bullet;
    public GameObject explosion;

    public Sprite deadSprite;

    public AudioClip ExplosionAudioClip;

    public static bool alive;

    void Update()
    {
        /* Defeatモードだったら */
        if (FindObjectOfType<AG_Body>().mode == AG_Body.Mode.DEFEAT)
        {
            alive = false;

            /* 爆破オブジェクト生成 */
            Instantiate(explosion, transform.position, transform.rotation);

            /* 破壊 */
            Destroy(gameObject);
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        /* 撃たれた時 */
        if (collision.tag == "BlasterSightPoint")
        {
            /* 爆破オブジェクト生成 */
            Instantiate(explosion, transform.position, transform.rotation);

            /* 爆破音再生 */
            Player.PlayAtPoint2D(ExplosionAudioClip);

            /* 1000ポイント加点 */
            Status.SCORE += 1000;

            /* 破壊 */
            Destroy(gameObject);
        }
    }

    /* プレイヤーに向かって弾を打つ */
    private IEnumerator ShotToPlayer()
    {
        while (alive)
        {
            GameObject obj = Instantiate(bullet, transform.position, transform.rotation);
            yield return new WaitForSeconds(Random.Range(0.5f, 1.0f));
        }

        /* 死んだ画像にする */
        GetComponent<SpriteRenderer>().sprite = deadSprite;
    }

    /* 画面に入ってきたら */
    private void OnBecameVisible()
    {
        alive = true;
        StartCoroutine(ShotToPlayer());
    }

    /* 画面外に行ったら */
    private void OnBecameInvisible()
    {
        alive = false;
    }
}
