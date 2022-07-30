using UnityEngine;

public class Bragza : MonoBehaviour
{
    public int speed;

    void Update()
    {
        /* 上に移動する */
        transform.position += transform.up * speed * Time.deltaTime;
    }

    /* 画面外に行ったら */
    private void OnBecameInvisible()
    {
        Destroy(gameObject);
    }
}
