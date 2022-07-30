using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class UI : MonoBehaviour
{
    public Text scoreText;            //スコアの<Text>
    public Text highScoreText;        //ハイスコアの<Text>
    public Text startText;            //開始時文章<Text>

    private AudioSource extendSound;   //1UP SE

    private int every20000score_count = 0; //20000点毎にカウント
    private int scoreCount = 0;

    private bool gameMain = false;

    // Start is called before the first frame update
    void Start()
    {
        Set_ActiveChild();

        //1UP SE 取得
        extendSound = GetComponent<AudioSource>();

        //イベントに設定
        SceneManager.activeSceneChanged += SceneChanged;
    }

    // Update is called once per frame
    void Update()
    {
        if (gameMain)
        {
            ScoreControl();

            Extend();
        }
    }


    /****************************** start用 ******************************/
    void Set_ActiveChild()
    {
        //一度すべての子オブジェクトを非アクティブにする
        foreach(Transform childTransform in this.transform.Find("Canvas"))
        {
            childTransform.gameObject.SetActive(false);
        }
        gameMain = false;
        transform.Find("Canvas/ALWAYS").gameObject.SetActive(true);


        switch (SceneManager.GetActiveScene().name)
        {
            case "Intro":
                transform.Find("Canvas/ALWAYS").gameObject.SetActive(false);
                break;
            case "Title":
                transform.Find("Canvas/TITLE").gameObject.SetActive(true);
                break;
            case "Title2":
                transform.Find("Canvas/TITLE2").gameObject.SetActive(true);
                break;
            case "GameMain":
                Transform gameMain_transform = transform.Find("Canvas/GAME_MAIN");
                gameMain_transform.gameObject.SetActive(true);
                Instantiate(startText, gameMain_transform);
                gameMain = true;
                break;
            default:
                break;
        }
    }


    /****************************** update用 ******************************/
    void ScoreControl()
    {
        //スコア カウントストップ
        if (Status.SCORE >= 9999990)
        {
            Status.SCORE = 9999990;
        }
        //ハイスコア随時更新
        if (Status.HIGH_SCORE <= Status.SCORE)
        {
            Status.HIGH_SCORE = Status.SCORE;
        }

        //textオブジェクトにスコア、ハイスコアを渡す
        scoreText.text = Status.SCORE.ToString("00");
        highScoreText.text = Status.HIGH_SCORE.ToString("00");
    }

    void Extend()
    {
        scoreCount = Status.SCORE - (every20000score_count * 20000);
        if(scoreCount >= 20000)
        {
            scoreCount -= 20000;
            every20000score_count++;
            Status.SHIP_STOCKS++;

            extendSound.Play();
        }
    }


    //シーンが切り替わった時に呼ばれる (UI.cs:30)
    void SceneChanged(Scene thisScene, Scene nextScene)
    {
        //Debug.Log("change!!!!");
        Set_ActiveChild();  
    }

}
