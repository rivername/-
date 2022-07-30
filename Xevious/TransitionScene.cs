using UnityEngine;
using UnityEngine.SceneManagement;

public class TransitionScene : MonoBehaviour
{
    bool calledOnce = false;
    AsyncOperation asyncLoad = null;
    // Start is called before the first frame update
    void Start()
    {
    }


    // Update is called once per frame
    void Update()
    {
        if (!calledOnce)
        {
            switch (SceneManager.GetActiveScene().name)
            {
                case "Title":
                    //次シーンを読み込んで待機させておく
                    asyncLoad = SceneManager.LoadSceneAsync("Title2");
                    asyncLoad.allowSceneActivation = false; //切り替えさせない
                    break;
                case "Title2":
                    asyncLoad = SceneManager.LoadSceneAsync("GameMain");
                    asyncLoad.allowSceneActivation = false;
                    break;
                default:
                    break;
            }
            calledOnce = true;
        }

        if (Input.anyKey)
        {
            //シーン切り替え
            asyncLoad.allowSceneActivation = true;
            calledOnce = false;
        }
    }
}
