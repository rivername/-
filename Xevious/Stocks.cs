using UnityEngine;
using UnityEngine.SceneManagement;

public class Stocks : MonoBehaviour
{
    public GameObject stockObject;
    private float imgSize = 32;

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        if (Status.SHIP_STOCKS >= 0)
        {
            DrawStock();
        }
    }

    void DrawStock()
    {
        int shiftStock; 
        int childCnt;

        childCnt = transform.childCount;
        if ((shiftStock = Status.SHIP_STOCKS - childCnt) != 0)
        {
            for (; shiftStock > 0; shiftStock--)
            {
                //右端に追加
                Instantiate
                    (
                    stockObject,
                    transform.position + new Vector3(childCnt++ * imgSize, 0, 0),
                    transform.rotation,
                    this.transform           //親をこのオブジェクトに設定
                    );
            }

            for (; shiftStock < 0; shiftStock++)
            {
                //右端から削除していく
                Destroy(this.transform.GetChild(--childCnt).gameObject);
            }
        }
    }
}
