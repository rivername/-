using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Explosion : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        // animatorコンポーネントを取得
        Animator anim = GetComponent<Animator>();
        // animatorの現在のアニメーションの状態を取得
        AnimatorStateInfo animInfo = anim.GetCurrentAnimatorStateInfo(0);
        // アニメーションの再生が終わったら(再生時間が1.0=100%を超えたら）
        if (animInfo.normalizedTime > 0.9f)
        {
            Destroy(gameObject);    //自分自身を消去する
        }

    }
}
