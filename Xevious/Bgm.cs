using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Threading;


public class Bgm : MonoBehaviour
{
    //オーディオソースを配列で宣言
    private AudioSource[] sound;

    void Start()
    {
        //コンポーネントの取得
        sound = gameObject.GetComponents<AudioSource>();

        //startBGM再生
        sound[0].Play();

    }

    private void Update()
    {
        //再生状況
        float playTime = sound[0].time + Time.deltaTime;

        //再生が終わったら
        if (playTime >= sound[0].clip.length)
        {

            sound[1].Play();

        }
    }
}
