using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InsertText : MonoBehaviour
{
    private Text text;
    private float time = 0;
    private float speed = 5f;

    // Start is called before the first frame update
    void Start()
    {
        text = GetComponent<Text>();
    }

    // Update is called once per frame
    void Update()
    {
        //α値を変えて点滅させる
        text.color = ChangeAlpha(text.color);
    }

    Color ChangeAlpha(Color col)
    {
        time += Time.deltaTime * speed;
        if (time % 2 < 1)
        {
            col.a = 1;
        }
        else
        {
            col.a = 0;
        }

        return col;
    }
}
