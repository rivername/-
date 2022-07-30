using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyExplosion : MonoBehaviour
{

    private bool brokenEffectFlag = true;
    public bool BrokenEffectFlag
    {
        get { return brokenEffectFlag; }
        set { brokenEffectFlag = value; }
    }

    public GameObject enemyBrokenEffect;

    public float speed = 1f;

    // Start is called before the first frame update
    void Start()
    {
        //SE再生
        if (GetComponent<AudioSource>().enabled)
        {
            PlayAtPoint2D(GetComponent<AudioSource>().clip, transform.position, 1f, "Ground Explosion");
        }
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;

        //アニメーション
        AnimatorStateInfo animInfo = GetComponent<Animator>().GetCurrentAnimatorStateInfo(0);
        //アニメーション終了時、削除
        if (animInfo.normalizedTime > 1.0f)
        {
            if (brokenEffectFlag)
            {
                Instantiate(enemyBrokenEffect, transform.position, transform.rotation);
            }
            Destroy(gameObject);
        }
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
