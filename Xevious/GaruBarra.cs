using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GaruBarra : MonoBehaviour
{
    public GameObject barra;

    float speed = 1f;

    // Start is called before the first frame update
    void Start()
    {
        //中央にバーラ生成、Zを-1、モード設定
        Instantiate(barra, transform.position + new Vector3(0, 0, -1), transform.rotation).GetComponent<Barra>().BarraMode = true;
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
