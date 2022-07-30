using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GaruDerota : MonoBehaviour
{
    public GameObject derota;

    float speed = 1f;

    // Start is called before the first frame update
    void Start()
    {
        //中央にデロータ生成、Zを-1、モード設定
        Instantiate(derota, transform.position+new Vector3(0,0,-1), transform.rotation).GetComponent<Derota>().GaruMode = true;
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
