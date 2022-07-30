using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bacura : MonoBehaviour
{
    private float speed = 2f;

    // Start is called before the first frame update
    void Start()
    {
        //debug
        //speed = 0;
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.tag == "Zapper")
        {
            //SE再生
            GetComponent<AudioSource>().Play();
            Destroy(collision.gameObject);

            //debug
            //Status.SCORE += 1500;
        }
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
