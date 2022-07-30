using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;


public class ExtendText : MonoBehaviour
{
    private Text text;
    private bool calledOnce = false;

    // Start is called before the first frame update
    void Start()
    {
        text = GetComponent<Text>();
    }

    // Update is called once per frame
    void Update()
    {
        if (SceneManager.GetActiveScene().name == "GameMain")
        {
            if (!calledOnce)
            {
                //0.4秒ごとに呼ぶ
                InvokeRepeating("Blinking", 0, 0.4f);

                calledOnce = true;
            }
        }
    }

    void Blinking()
    {
        //α値を変えて点滅させる
        text.color = (text.color.a == 0) ? Color.white : Color.clear;
    }
}
