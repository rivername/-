using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GaruZakato_Spario : MonoBehaviour
{
    public float speed = 6f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        transform.position -= transform.up * speed * Time.deltaTime;
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
