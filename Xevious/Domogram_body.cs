using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Domogram_body : MonoBehaviour
{
    public GameObject enemyBullet;
    public GameObject explosion;

    private Animator animator;
    private AnimationClip[] clips;

    public float speed = 1.0f;

    private bool onFire = false;


    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();

        //全てのアニメーションクリップを配列に取得
        //clip[0]  ...  ドモグラム_00_待機.anim
        //clip[1]  ...  ドモグラム_01_col1.anim
        //clip[2]  ...  ドモグラム_02_col2.anim
        //clip[3]  ...  ドモグラム_03_col3.anim
        //clip[4]  ...  ドモグラム_04_col4.anim
        //clip[5]  ...  ドモグラム_05_col5.anim
        clips = animator.runtimeAnimatorController.animationClips;
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
            if (GameObject.FindGameObjectWithTag("Player") != null)
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
    /// 終わったら待機アニメーション開始させる
    /// </summary>
    IEnumerator Fire_animation()
    {
        int col_index = (int)Mathf.Floor(animator.GetCurrentAnimatorStateInfo(0).normalizedTime * 5.0f) % 5; //5色
        float normalizedTime = 0;
        float animLength = clips[col_index].length;
        while (true)
        {
            if (normalizedTime < 1.0f)
            {
                if (++col_index > 5) col_index = 1;
                animator.Play(clips[col_index].name, -1, normalizedTime);

                normalizedTime += 1.0f / 5.0f;
                yield return new WaitForSeconds(animLength / 5.0f);
                continue;
            }
            //発射が終わって、待機アニメーションを再生
            animator.Play(clips[0].name, -1, (float)col_index / 5.0f);
            yield break;
        }
    }

    /// <summary>
    /// アニメーションイベント側で呼び出す　弾発射
    /// </summary>
    public void fire()
    {
        if (onFire)
        {
            Instantiate(enemyBullet, transform.position, transform.rotation);
            onFire = false;
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.tag == "BlasterSightPoint")
        {
            Instantiate(explosion, transform.position, transform.rotation);

            Status.SCORE += 800;
            Status.KILLS_BY_BLASTER++;

            Destroy(transform.parent.gameObject);
        }
    }


    private void OnBecameVisible()
    {
        StartCoroutine(Start_fire());
        transform.parent.GetComponent<Domogram_moveRoute>().MoveFlag = true;
    }

    private void OnBecameInvisible()
    {
        Destroy(transform.parent.gameObject);
    }
}
