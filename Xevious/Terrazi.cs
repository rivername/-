using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Terrazi : MonoBehaviour
{

    public enum AnimStatus
    {
        APPEAR,
        ESCAPE
    }

    private AnimStatus animStatus;

    public GameObject enemyBullet;
    public GameObject explosion;

    private Animator animator;
    private AnimationClip[] clips;

    private GameObject targetObj;
    private Vector3 targetPos;
    private Vector3 targetVec;
    public float speed = 6.0f;


    private Vector3 forceVec;

    // Start is called before the first frame update
    void Start()
    {
        //最初は出現モード
        animStatus = AnimStatus.APPEAR;
        animator = GetComponent<Animator>();

        //全てのアニメーションクリップを配列に取得
        //clips[0]  ...  テラジ00_登場
        //clips[1]  ...  テラジ01_転回col1
        //clips[2]  ...  テラジ02_転回col2
        //clips[3]  ...  テラジ03_転回col3
        //clips[4]  ...  テラジ04_転回col4
        //clips[5]  ...  テラジ05_転回col5
        //clips[6]  ...  テラジ06_逃走
        clips = animator.runtimeAnimatorController.animationClips;

        //自機のオブジェクト取得
        if ((targetObj = GameObject.FindGameObjectWithTag("Player")) == null)
        {
            Destroy(this.gameObject);
        }

        //自機との角度算出
        Vector3 thisPos = transform.position;
        targetPos = targetObj.transform.position;
        float rad = Mathf.Atan2(targetPos.y - thisPos.y, targetPos.x - thisPos.x);
        targetVec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);
    }

    // Update is called once per frame
    void Update()
    {
        //自機の座標再取得
        if (targetObj != null) targetPos = targetObj.transform.position;

        switch (animStatus)
        {
            case AnimStatus.APPEAR:
                transform.position += speed * targetVec * Time.deltaTime;
                break;
            case AnimStatus.ESCAPE:
                Move_ESCAPE();
                break;
        }
    }

    //出現移動の終了
    IEnumerator End_APPEAR()
    {
        while (true)
        {
            if (Mathf.Abs(transform.position.x - targetPos.x) <= 1)
            {
                animStatus = AnimStatus.ESCAPE;

                if (transform.position.x < targetPos.x)
                    forceVec = new Vector3(-1.0f / 4.0f, 1.0f / 4.0f);
                else
                    forceVec = new Vector3(1.0f / 4.0f, 1.0f / 4.0f);

                yield break;
            }
            //1フレームごと
            yield return new WaitForSeconds(Time.deltaTime);
        }
    }

    //逃走時の移動
    bool onceTrigger_ESCAPE = false;
    void Move_ESCAPE()
    {
        targetVec += forceVec;
        transform.position += targetVec * Time.deltaTime;

        if (!onceTrigger_ESCAPE)
        {
            onceTrigger_ESCAPE = true;
            StartCoroutine(RotateAnim());
        }
    }

    //転回時のアニメーション
    IEnumerator RotateAnim()
    {
        int i = (int)Mathf.Floor(animator.GetCurrentAnimatorStateInfo(0).normalizedTime * 5.0f) % 5;
        float normalizedTime = 0;
        float animLength = clips[i].length;
        while (true)
        {
            if (normalizedTime < 1.0f)
            {
                if (++i > 5) i = 1;
                animator.Play(clips[i].name, -1, normalizedTime);

                normalizedTime += 1.0f / 5.0f;

                yield return new WaitForSeconds(animLength / 5.0f);
                continue;
            }
            //逃走用アニメーション
            animator.Play(clips[6].name, -1, (float)i / 5.0f);
            yield break;
        }
    }

    //発射
    IEnumerator Fire()
    {
        while (true)
        {
            Instantiate(enemyBullet, transform.position, transform.rotation);
            yield return new WaitForSeconds(0.3f);
        }
    }

    private void OnBecameVisible()
    {
        StartCoroutine(End_APPEAR());
        StartCoroutine(Fire());
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.tag == "Zapper")
        {
            Instantiate(explosion, transform.position, transform.rotation);

            Destroy(collision.gameObject);

            Status.SCORE += 700;
            Status.KILLS_BY_ZAPPER++;

            Destroy(gameObject);
        }
    }


    private void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
