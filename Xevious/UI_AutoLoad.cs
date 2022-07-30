using UnityEngine;
using UnityEngine.SceneManagement;

public class UI_AutoLoad : MonoBehaviour
{
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
    private static void Load_UIScene()
    {
        string SceneName = "UI";

        //ManagerSceneが有効でないときに追加ロード
        if (!SceneManager.GetSceneByName(SceneName).IsValid())
        {   

            SceneManager.LoadScene(SceneName, LoadSceneMode.Additive);

        }
    }
}
