using UnityEngine;

public class Zapper : MonoBehaviour
{
    public float speed;

    private Animator anim;

    void Start()
    {
        Anim();
    }

    void Update()
    {
        Move();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: Move
    //	タイプ: void
    //	引数: なし
    //	説明: 弾の動き
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void Move()
    {
        transform.position += new Vector3(0, speed, 0) * Time.deltaTime;

        Vector2 max = Camera.main.ViewportToWorldPoint(new Vector2(1, 1));

        if (max.y < transform.position.y)
        {
            Destroy(this.gameObject);
        }
    }
    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: Anim
    //	タイプ: void
    //	引数: なし
    //	説明: 弾のアニメーションを制御する（縦軸で揃える）
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void Anim()
    {
        GameObject zapper = GameObject.FindGameObjectWithTag("Zapper");

        /* ザッパーの弾がゲーム内にあったら */
        if (zapper != null)
        {
            /* Animatorコンポーネント */
            Animator lBullet = zapper.transform.GetChild(0 /* 左 */).GetComponent<Animator>();
            Animator rBullet = zapper.transform.GetChild(1 /* 右 */).GetComponent<Animator>();

            /* Animator State Info */
            AnimatorStateInfo lInfo = lBullet.GetCurrentAnimatorStateInfo(0);
            AnimatorStateInfo rInfo = rBullet.GetCurrentAnimatorStateInfo(0);

            /* Normalized Time */
            float lStateTime = lInfo.normalizedTime;
            float rStateTime = rInfo.normalizedTime;

            /* 左右の弾のAnimatorコンポーネント */
            Animator meL = transform.GetChild(0 /* 左 */).GetComponent<Animator>();
            Animator meR = transform.GetChild(1 /* 右 */).GetComponent<Animator>();

            /* アニメーションを揃える */
            meL.Play(0, 0, lStateTime);
            meR.Play(0, 0, rStateTime);
        }
    }
}
