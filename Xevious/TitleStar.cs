using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TitleStar : MonoBehaviour
{
    Animator anim;
    public static bool Ded;
    // Start is called before the first frame update
    void Start()
    {
        anim = GetComponent<Animator>();
        Ded = false;
    }
    
    // Update is called once per frame
    void Update()
    {
        if (anim.GetCurrentAnimatorStateInfo(0).IsName("タイトル星_Rotation"))
        {
            transform.position += new Vector3(2, 0, 0) * Time.deltaTime;
            if(transform.position.x >= 3)
            {
                anim.Play("タイトル星_Out");
            }
        }
    }

    //アニメーションイベントで呼び出し
    public void AnimEnd()
    {
        Ded = true;
        Destroy(this.gameObject);
    }
}
