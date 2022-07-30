using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Kapi : MonoBehaviour
{
    public enum AnimStatus
    {
        APPEAR,   //出現
        ESCAPE    //逃走
    }
    private AnimStatus animStatus;

    public GameObject enemyBullet;
    public GameObject explosion;

    private Animator animator;
    private AnimationClip[] clips;

    private GameObject targetObj;
    private Vector3 targetPos;
    private Vector3 targetVec;
    public float speed = 4.0f;

    private Vector3 forceVec;


    // Start is called before the first frame update
    void Start()
    {
        //最初は出現モード
        animStatus = AnimStatus.APPEAR;
        animator = GetComponent<Animator>();

        //全てのアニメーションクリップを配列に取得
        //clips[0]  ...  カピ00_登場
        //clips[1]  ...  カピ01_転回col0
        //clips[2]  ...  カピ02_転回col1
        //clips[3]  ...  カピ03_転回col2
        //clips[4]  ...  カピ04_転回col3
        //clips[5]  ...  カピ05_転回col4
        //clips[6]  ...  カピ06_逃走
        clips = animator.runtimeAnimatorController.animationClips;

        //自機のオブジェクト取得
        if((targetObj = GameObject.FindGameObjectWithTag("Player")) == null)
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
        if(targetObj != null) targetPos = targetObj.transform.position;

        //状態で場合分け
        switch (animStatus)
        {
            case AnimStatus.APPEAR:
                Move_APPEAR();
                break;
            
            case AnimStatus.ESCAPE:
                Move_ESCAPE();
                break;
        }
    }

    //出現時の移動
    void Move_APPEAR()
    {
        transform.position += speed * targetVec * Time.deltaTime;
    }

    //出現移動の終了
    IEnumerator End_APPEAR()
    {
        float flameCount = 0.0f;
        float endTime = Random.Range(30.0f, 55.0f); //適当
        while (true)
        {
            if(flameCount++ >= endTime)
            {
                animStatus = AnimStatus.ESCAPE;

                if (transform.position.x < targetPos.x)
                    forceVec = new Vector3(-1.0f / 8.0f, 1.0f / 4.0f);
                else
                    forceVec = new Vector3( 1.0f / 8.0f, 1.0f / 4.0f);

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
            StartCoroutine(Fire());
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
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.tag == "Zapper")
        {
            Instantiate(explosion, transform.position, transform.rotation);

            Destroy(collision.gameObject);

            Status.SCORE += 300;
            Status.KILLS_BY_ZAPPER++;

            Destroy(gameObject);
        }
    }


    private void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
