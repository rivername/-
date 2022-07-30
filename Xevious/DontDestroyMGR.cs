using UnityEngine;

public class DontDestroyMGR : MonoBehaviour
{
    public GameObject UIObject;

    // Start is called before the first frame update
    void Start()
    {
        DontDestroyOnLoad(UIObject);
    }
}
