using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

/*
 * 
 * ゲームオーバー
 * -> 150フレーム後にタイトルに戻る
 * 
 */

public class GameOver : MonoBehaviour
{
    private GameObject bgm;
    private GameObject skyEnemyManager;
    private GameObject uiGameOvertext;

    private int counter = 0;
    private bool show;

    void Start()
    {
        bgm = GameObject.Find("BGM");
        skyEnemyManager = GameObject.Find("SkyEnemyManager");
        uiGameOvertext  = transform.GetChild(0).gameObject;
    }

    void Update()
    {
        if (show)
        {
            counter++;

            if (counter == 150)
            {
                GoBackToTitle();
                show    = false;
                counter = 0;
            }
        }
    }

    void _Show()
    {
        Time.timeScale = 0.0f;

        AudioListener.pause = true;

        Destroy(bgm);
        Destroy(skyEnemyManager);

        show = true;

        uiGameOvertext.SetActive(true);
    }

    void _Hide()
    {
        Time.timeScale = 1.0f;

        AudioListener.pause = false;

        uiGameOvertext.SetActive(false);
    }

    public static void Show()
    {
        FindObjectOfType<GameOver>()._Show();
    }

    public static void Hide()
    {
        if (FindObjectOfType<GameOver>() == null) return;
        FindObjectOfType<GameOver>()._Hide();
    }

    void GoBackToTitle()
    {
        Hide();

        Status.STAGE_NUMBER = 1;
        Status.SCORE = 0;
        Status.SHIP_STOCKS = 2;

        SceneManager.LoadScene(1);
    }
}
