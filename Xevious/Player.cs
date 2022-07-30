using System.Collections;
using System.Linq;
using UnityEngine;

public class Xevious
{
    /// <summary>
    /// メインプレイヤー（自機）
    /// </summary>
    public static GameObject mainPlayer
    {
        get
        {
            if (Object.FindObjectOfType<Player>() != null)
            {
                return Object.FindObjectOfType<Player>().gameObject;
            }
            else
            {
                return null;
            }
        }
    }

    /// <summary>
    /// プレイヤーの照準が地上の敵にホバーしてるか
    /// </summary>
    public static bool player_HOVERED_ON_GroundEnemy
    {
        get
        {
            if (Object.FindObjectOfType<Player>() != null)
            {
                return Object.FindObjectOfType<Player>().HOVERED_ON_GroundEnemy;
            }
            else
            {
                return false;
            }
        }
    }
}

public class Player : MonoBehaviour
{
    public GameObject bulletZapper;
    public GameObject bulletBlaster;
    public GameObject playerExplosion;
    public GameObject SightChild0;

    public Sprite initSprite;

    public AudioClip audioExplosion;

    public float xSpeed;
    public float ySpeed;

    private bool ZAPPER;
    private bool BLASTER;
    public  bool HOVERED_ON_GroundEnemy;

    public string[] friendlyObjectTags;
    public string[] groundEnemies;
    public string[] itemTags;

    void Awake()
    {
        //DontDestroyOnLoad(this);
    }

    IEnumerator Start()
    {
        Status.DEAD = false;
        PlayerData.LoadAll();

        /* カメラのZ座標を-30にする */
        Camera.main.transform.position = new Vector3(0, 0, -30.0f);

        while (true)
        {
            if (ZAPPER)
            {
                /* 弾がゲーム画面に出るのは3つまで */
                if (GameObject.FindGameObjectsWithTag("Zapper" /* Tagをつける */).Length < 3)
                {
                    ZapperFire();
                }
            }

            if (BLASTER)
            {
                BlasterFire();
            }

            yield return new WaitForSeconds(0.1f);
        }
    }

    void Update()
    {
        Move();
        Shot();
        SightManager();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: Move
    //	タイプ: void
    //	引数: なし
    //	説明: プレイヤーの動きの制御
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void Move()
    {
        /* 移動のキー入力 */
        float x = Input.GetAxisRaw("Horizontal") * xSpeed;
        float y = Input.GetAxisRaw("Vertical") * ySpeed;
        Vector3 direction = new Vector2(x, y).normalized;

        /* 画面の端の座標を取得 */
        Vector2 min = Camera.main.ViewportToWorldPoint(new Vector2(0, 0));
        Vector2 max = Camera.main.ViewportToWorldPoint(new Vector2(1, 1));
        Vector3 pos = transform.position;

        /* 画面外に行かないようにする */
        pos.x = Mathf.Clamp(pos.x, min.x + transform.localScale.x / 2, max.x - transform.localScale.y / 2);
        pos.y = Mathf.Clamp(pos.y, min.y + transform.localScale.y / 2, max.y - transform.localScale.y * 3);
        pos.z = -10.0f; /* Z=-10で固定 */

        pos += direction * 4 * Time.deltaTime;
        transform.position = pos;
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: Zapper
    //	タイプ: void
    //	引数: なし
    //	説明: 空中の敵を攻撃する
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void ZapperInput()
    {
        if (/* ------------------------------------------------ */
            /* キーが押されたら */
            Input.GetMouseButtonDown(0) || /* マウス左クリック */
            Input.GetKey(KeyCode.Z)        /* Zキー */
           )
        {
            ZAPPER = true;
        }   /* ------------------------------------------------ */

        if (/* ------------------------------------------------ */
            /* キーが離されたら */
            Input.GetMouseButtonUp(0) || /* マウス左クリック */
            Input.GetKeyUp(KeyCode.Z)    /* Zキー */
           )
        {
            ZAPPER = false;
        }   /* ------------------------------------------------ */
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: Blaster
    //	タイプ: void
    //	引数: なし
    //	説明: 地上の敵を攻撃する
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void BlasterInput()
    {
        if (/* ------------------------------------------------ */
            /* キーが押されたら */
            Input.GetMouseButtonDown(1) || /* マウス右クリック */
            Input.GetKey(KeyCode.X)        /* Zキー */
           )
        {
            BLASTER = true;
        }   /* ------------------------------------------------ */

        if (/* ------------------------------------------------ */
            /* キーが離されたら */
            Input.GetMouseButtonUp(1) || /* マウス右クリック */
            Input.GetKeyUp(KeyCode.X)    /* Zキー */
           )
        {
            BLASTER = false;
        }   /* ------------------------------------------------ */
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: ZapperFire
    //	タイプ: void
    //	引数: なし
    //	説明: ザッパーの発射
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void ZapperFire()
    {
        Instantiate
        (
            bulletZapper,

            /* 自機の上 */
            new Vector3(transform.position.x, transform.position.y + transform.localScale.y / 3, transform.position.z + 1),
            transform.rotation
        );
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: BlasterFire
    //	タイプ: void
    //	引数: なし
    //	説明: ブラスターの発射
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void BlasterFire()
    {
        if (GameObject.FindGameObjectWithTag("Blaster") == null)
        {
            Instantiate
            (
                bulletBlaster,

                /* 自機の上 */
                new Vector3(transform.position.x, transform.position.y + transform.localScale.y / 3, transform.position.z + 1),
                transform.rotation
            );
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: Shot
    //	タイプ: void
    //	引数: なし
    //	説明: 弾の発射関連
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void Shot()
    {
        ZapperInput();
        BlasterInput();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //	名前: SightManager
    //	タイプ: void
    //	引数: なし
    //	説明: 照準の制御
    //	返り値: なし
    //	備考: 
    //-+-+-+-+-+-+-+-+-+-+-+-+
    void SightManager()
    {
        /* ブラスターの弾がSceneに存在し、ザッパーを発射してないとき */
        if (GameObject.FindGameObjectWithTag("Blaster") != null && !ZAPPER)
        {
            Change(false, true);
        }
        else
        {
            Change(true, false);
        }

        /* ブラスター時 */
        if (BLASTER)
        {
            if (HOVERED_ON_GroundEnemy) return;
            Change(false, true);
        }

        /* ザッパー時 */
        if (ZAPPER)
        {
            Change(true, false);
        }

        /* サイト切り替え */
        void Change(bool zapper_idle, bool blaster)
        {
            transform.GetChild(0 /* Crosshair - アイドル & 空中の敵攻撃 */).gameObject.SetActive(zapper_idle);
            transform.GetChild(1 /* Crosshair - 地上の敵を攻撃 */        ).gameObject.SetActive(blaster);

            if (HOVERED_ON_GroundEnemy)
            {
                transform.GetChild(0 /* Crosshair - アイドル & 空中の敵攻撃 */).gameObject.GetComponent<Animator>(/* ホバーアニメーション */).enabled = true;
            }
            else
            {
                transform.GetChild(0 /* Crosshair - アイドル & 空中の敵攻撃 */).gameObject.GetComponent<SpriteRenderer>().sprite = initSprite;
                transform.GetChild(0 /* Crosshair - アイドル & 空中の敵攻撃 */).gameObject.GetComponent<Animator>(/* ホバーアニメーション */).enabled = false;
            }
        }
    }

    void OnTriggerEnter2D(Collider2D collision)
    {
        /* friendlyオブジェクト以外 */
        if (friendlyObjectTags.Contains(collision.tag)) return;

        /* 地上の敵以外 */
        if (groundEnemies.Contains(collision.tag)) return;

        /* SpecialFlagだったら */
        if (itemTags.Contains(collision.tag))
        {
            Status.SHIP_STOCKS++;
            Status.SCORE += 1000;
            Status.PICKED_UP_ITEM++;
            Destroy(collision.gameObject);
            PlayAtPoint2D(GetComponent<AudioSource>().clip);

            return;
        }

        /* 空中の敵と弾だったら */
        Dead();
    }

    public static void PlayAtPoint2D(AudioClip audioClip)
    {
        var gameObject = new GameObject($"#2D One Shot Audio: {audioClip.name}");
        var audioSource = gameObject.AddComponent<AudioSource>();
        audioSource.clip = audioClip;
        audioSource.Play();
        Destroy(gameObject, audioClip.length);
    }

    public void Dead()
    {
        if (Status.DEAD) return;

        Instantiate(playerExplosion, transform.position, transform.rotation);
        PlayAtPoint2D(audioExplosion);
        Status.SHIP_STOCKS--;
        Status.DEATHES++;
        _ = (Status.DIFFICULTY <= 24) ? (Status.DIFFICULTY = 0) : (Status.DIFFICULTY -= 24);

        if (Status.MAP_POSITION >= 179 /* 70% */)
        {
            Status.STAGE_NUMBER++;
        }

        Status.DEAD = true;
        Destroy(gameObject);
    }

    public void HoveredOnGroundEnemy(bool hovered)
    {
        HOVERED_ON_GroundEnemy = hovered;
    }

    public static void ExitGame()
    {
    #if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
    #elif UNITY_STANDALONE
        UnityEngine.Application.Quit();
    #endif
    }
}
