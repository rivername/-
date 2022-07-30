using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Title_grad : MonoBehaviour
{
    Animator anim;

    float c;

    void Start()
    {
        anim = GetComponent<Animator>();
        anim.SetBool("grd_start", false);
    }

    // Update is called once per frame
    void Update()
    {
        if(TitleStar.Ded == true)
        {
           anim.SetBool("grd_start", true);
        }

    }

}
