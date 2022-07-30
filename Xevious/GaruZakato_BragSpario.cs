using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GaruZakato_BragSpario : MonoBehaviour
{
    private Vector3 move;
    public Vector3 Move                   //GaruZakato.csで設定
    {
        get { return move; }

        set { move = value; }
    }

    private GameObject target;            //目標
    private AudioClip  clip;

    private float acceleration = 1 / 4f;  //追尾の加速度

    // Start is called before the first frame update
    void Start()
    {
        target = GameObject.FindGameObjectWithTag("Player");
        clip = GetComponent<AudioSource>().clip;
    }

    // Update is called once per frame
    void Update()
    {
        if (target != null)
        {
            //自機の位置によって加速させる
            Vector3 thisPos = this.transform.position;
            Vector3 targetPos = target.transform.position;

            if (thisPos.x > targetPos.x) //右側にいるとき
            {
                move.x -= acceleration;
            }
            if(thisPos.x < targetPos.x)//左側
            {
                move.x += acceleration;
            }

            if (thisPos.y > targetPos.y)  //上側にいるとき
            {
                move.y -= acceleration;
            }
            if (thisPos.y < targetPos.y)//下側
            {
                move.y += acceleration;
            }
        }

        //加速
        GetComponent<Rigidbody2D>().velocity = move;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.tag == "Zapper")
        {
            //スコア
            Status.SCORE += 500;

            //hit音
            PlayAtPoint2D(clip, transform.position, 1f, "BragSpario"); 
            //ザッパー削除
            Destroy(collision.gameObject);
        }
    }

    private void OnBecameInvisible()
    {
        Destroy(this.gameObject);
    }

    /*********************************************************************
     * 変数名   PlayAtPoint2D
     * 型      void
     * 引き数   AudioClip  clip         .. 再生するクリップ
     *         Vector3    position     .. 再生する座標
     *         float      volume       .. 再生ボリューム                　省略可
     *         string     name         .. 作るオブジェクトの名前(デバッグ用) 省略可
     * 戻り値   無し
     * 備考     
     *********************************************************************/
    public void PlayAtPoint2D(AudioClip clip, Vector3 position, float volume = 1f, string name = "2D One shot")
    {
        // name+"audio" という名前のゲームオブジェクト作成
        GameObject tmpGameObject = new GameObject(name + " audio");

        //tmpGameObject に AudioSource を追加
        AudioSource tmpAudioSource = new AudioSource();
        tmpAudioSource = tmpGameObject.AddComponent<AudioSource>();

        //AudioSource の設定
        tmpAudioSource.clip = clip;
        tmpAudioSource.transform.position = position;
        tmpAudioSource.volume = volume;
        tmpAudioSource.spatialBlend = 0;
        tmpAudioSource.loop = false;
        tmpAudioSource.playOnAwake = false;

        //再生
        tmpAudioSource.Play();

        //削除
        Destroy(tmpGameObject, clip.length);
    }
}
