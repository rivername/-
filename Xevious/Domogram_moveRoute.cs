using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Domogram_moveRoute : MonoBehaviour
{
    private Transform body;
    private Transform target;

    public float speed = 1.0f;

    private int targetIndex = 0;

    private bool moveFlag = false;  //本体からいじる
    public bool MoveFlag
    {
        set { moveFlag = value; }
    }

    // Start is called before the first frame update
    void Start()
    {
        body   = transform.GetChild(0);
        target = transform.GetChild(1);
    }

    void Update()
    {
        transform.position -= transform.up * speed * Time.deltaTime;
        if (moveFlag)
        {
            moveFlag = false;
            StartCoroutine(Move_toTarget());
        }
    }
    
    IEnumerator Move_toTarget()
    {
        while (true)
        {
            body.position = Vector2.MoveTowards(body.position, target.GetChild(targetIndex).position, speed * Time.deltaTime);
            if (Vector2.Distance(body.position, target.GetChild(targetIndex).position) == 0)
            {
                if (++targetIndex == target.childCount)
                {
                    yield break;
                }
            }

            yield return null;
        }
    }

}
