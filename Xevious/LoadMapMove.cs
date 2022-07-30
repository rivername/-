using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LoadMapMove : MonoBehaviour
{
    public float speed = 1.0f;
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        Vector3 direction = transform.up * speed;

        transform.position += direction * -speed * Time.deltaTime;
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }
}
