﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyBrokenEffect : MonoBehaviour
{
    public float speed = 1f;

    // Start is called before the first frame update
    void Start()
    {
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
