using UnityEngine;
using UnityEngine.UI;

public class FadeIn : MonoBehaviour
{
    private float alfa = 255f;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if(alfa <= 0)
        {
            Destroy(this.gameObject);
        }
        GetComponent<Image>().color = new Color(0, 0, 0, alfa);
        alfa -= 200;
    }
}
