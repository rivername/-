using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GaruZakato : MonoBehaviour
{
    public GameObject spario_garuZakato;
    public GameObject bragSpario;
    public GameObject explosionPrefab;

    public float speed = 6f;
    private float i;
    private bool suicide = true;

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        //移動
        transform.position -= transform.up * speed * Time.deltaTime;

        //自爆するタイミング
        if (transform.position.y < Random.Range(2f, 3f))
        {
            Destroy(this.gameObject);
        }
    }

    /*********************************************************************
     * 変数名   FireSpario
     * 処理   　スパリオ(16発)生成
     * 型      void
     * 引き数   無し
     * 戻り値   無し
     * 備考     
     *********************************************************************/
    void FireSpario()
    {
        Vector3 pos = transform.position;    //発射時の座標
        Quaternion rot = transform.rotation; //発射時の回転
        float sparioNum = 16.0f;
        // 16方向にスパリオ発射
        for (i = 0.0f; i < sparioNum; i += 1.0f)
        {
            Instantiate(spario_garuZakato, pos, Quaternion.Euler(new Vector3(0, 0, 360.0f / sparioNum * i)));
        }
    }

    /*********************************************************************
     * 変数名   FireBragSpario
     * 処理   　ブラグスパリオ(4発)生成
     * 型      void
     * 引き数   無し
     * 戻り値   無し
     * 備考     
     *********************************************************************/
    void FireBragSpario()
    {
        float x, y;
        float vx, vy;
        // 4方向にブラグスパリオ発射
        for (i = 0; i < 2*Mathf.PI; i += Mathf.PI / 2f)
        {
            x  = Mathf.Cos(i);
            y  = Mathf.Sin(i);
            vx = (Mathf.Floor(x) == Mathf.Ceil(x)) ? x : 0;
            vy = (Mathf.Floor(y) == Mathf.Ceil(y)) ? y : 0;

            //ブラグスパリオ生成、ブラグスパリオのスクリプト内の Move を設定
            Instantiate(bragSpario, transform.position, transform.rotation).GetComponent<GaruZakato_BragSpario>().Move = new Vector3(vx, vy, 0).normalized * 4;
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.tag == "Zapper")
        {
            suicide = false; // 自爆オフ
            Instantiate(explosionPrefab, transform.position, transform.rotation);
            Status.SCORE += 1000;
            Destroy(this.gameObject);
            Destroy(collision.gameObject);
        }
    }

    private void OnDestroy()
    {
        if (suicide) //自爆の時
        {
            //発射
            FireSpario();
            FireBragSpario();
            //SE
            PlayAtPoint2D(GetComponent<AudioSource>().clip, transform.position, 1f, "GaruZakato");
        }
    }


    /*********************************************************************
     * 変数名   PlayAtPoint2D
     * 処理     サウンド専用のオブジェクトを生成、再生終了時にデストロイ
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
        AudioSource tmpAudioSource = tmpGameObject.AddComponent<AudioSource>();

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
