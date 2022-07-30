using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Zoshi : MonoBehaviour
{
    /* 弾 */
    public GameObject enemyBullet;

    public GameObject explosion;

    /* 移動速度 */
    public float speed;

    /* プレイヤーの位置 */
    private Vector2 playerPos;

    /* 状態 */
    private bool enable;

    void Update()
    {
        if (enable)
        {
            /* プレイヤーの方向に移動 */
            transform.position = Vector2.MoveTowards(transform.position, playerPos, speed * Time.deltaTime);
        }
        else
        {
            /* 下に移動 */
            transform.position -= transform.up * 1.0f * Time.deltaTime;
        }
    }

    void OnBecameVisible()
    {
        /* enableにする */
        enable = true;

        StartCoroutine(UpdatePlayerPositionAndShot());
    }

    private IEnumerator UpdatePlayerPositionAndShot()
    {
        while (true)
        {
            if (Xevious.mainPlayer == null) yield break;

            /* プレイヤーの位置を更新 */
            playerPos = Xevious.mainPlayer.transform.position;

            /* 弾を発射 */
            Instantiate(enemyBullet, transform.position, Quaternion.identity);

            /* 0.5f ~ 1.5f待つ */
            yield return new WaitForSeconds(Random.Range(0.25f, 1.75f));
        }
    }

    void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Zapper")
        {
            Instantiate(explosion, transform.position, Quaternion.identity);
            Destroy(gameObject);
        }
    }
}
