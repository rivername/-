using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Jara : MonoBehaviour
{
    public enum AnimStatus
    {
        APPEAR,   //出現
        ESCAPE    //逃走
    }
    private AnimStatus animStatus;

    public enum FireFlag
    {
        TRUE,
        FALSE
    }
    public FireFlag fireFlag;

    public GameObject enemyBullet;
    public GameObject explosion;

    private Animator animator; 
    private AnimationClip[] clips;

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
        //clips[0]  ...  ジアラ00_登場
        //clips[1]  ...  ジアラ01_逃走col1
        //clips[2]  ...  ジアラ02_逃走col2
        //clips[3]  ...  ジアラ03_逃走col3
        //clips[4]  ...  ジアラ04_逃走col4
        //clips[5]  ...  ジアラ05_逃走col5
        clips = animator.runtimeAnimatorController.animationClips;

        //自機のオブジェクト取得
        if (Xevious.mainPlayer == null)
        {
            Destroy(this.gameObject);
        }

        //自機との角度算出
        Vector3 thisPos = transform.position;
        targetPos = Xevious.mainPlayer.transform.position;
        float rad = Mathf.Atan2(targetPos.y - thisPos.y, targetPos.x - thisPos.x);
        targetVec = new Vector3(Mathf.Cos(rad), Mathf.Sin(rad), 0);
    }

    // Update is called once per frame
    void Update()
    {
        //自機の座標再取得
        if (Xevious.mainPlayer != null) targetPos = Xevious.mainPlayer.transform.position;

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
        if(Mathf.Abs(transform.position.x - targetPos.x) <= 1)
        {
            animStatus = AnimStatus.ESCAPE;

            if (fireFlag == FireFlag.TRUE)
            {
                Instantiate(enemyBullet, transform.position, transform.rotation);
            }
            StartCoroutine(RotateAnim());

            if (transform.position.x < targetPos.x)
                forceVec = new Vector3(-1.0f / 32.0f, 0.0f, 0.0f);
            else
                forceVec = new Vector3(1.0f / 32.0f, 0.0f, 0.0f);
        }
    }

    void Move_ESCAPE()
    {
        targetVec += forceVec;
        transform.position += speed * targetVec * Time.deltaTime;
    }

    //転回時のアニメーション
    IEnumerator RotateAnim()
    {
        int i = (int)Mathf.Floor(animator.GetCurrentAnimatorStateInfo(0).normalizedTime * 5.0f) % 5;
        float normalizedTime = 0;
        float animLength = clips[i].length;
        while (true)
        {
            if (++i > 5) i = 1;
            animator.Play(clips[i].name, -1, normalizedTime);

            normalizedTime += 1.0f / 5.0f;

            yield return new WaitForSeconds(animLength / 5.0f);
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.tag == "Zapper")
        {
            Instantiate(explosion, transform.position, transform.rotation);

            Destroy(collision.gameObject);

            Status.SCORE += 150;
            Status.KILLS_BY_ZAPPER++;

            Destroy(gameObject);
        }
    }

    private void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
