using UnityEngine;

public class AG_Body : MonoBehaviour
{
    public int speed = 1;

    private float timer = 0.0f;

    public Sprite deadSprite;

    public Mode mode;

    public enum Mode
    {
        ENTRY,
        EXIT,
        DEFEAT
    }

    private void Start()
    {
        /* エントリーモードにする */
        mode = Mode.ENTRY;

        /* 画面中央にする */
        transform.position = new Vector3(0.0f, transform.position.y, transform.position.z);
    }

    private void Update()
    {
        EntryMove();
        ExitMove();
        DefeatMode();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Name   : EntryMove()
    //  Type   : void
    //  Desc   : 登場の動き
    //  Return : なし
    //  P.S.   :
    //-+-+-+-+-+-+-+-+-+-+-+-+
    private void EntryMove()
    {
        if (mode != Mode.ENTRY) return;

        /* 画面上部 */
        if (transform.position.y >= 2.5f)
        {
            transform.position -= transform.up * speed * Time.deltaTime;
            timer = 0.0f;
        }
        else
        {
            Stay20sec();
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Name   : ExitMove()
    //  Type   : void
    //  Desc   : 退去の動き
    //  Return : なし
    //  P.S.   :
    //-+-+-+-+-+-+-+-+-+-+-+-+
    private void ExitMove()
    {
        if (mode != Mode.EXIT) return;

        /* 画面外上 */
        if (transform.position.y <= 7.0f)
        {
            transform.position += transform.up * speed * Time.deltaTime;
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Name   : DefeatMode()
    //  Type   : void
    //  Desc   : やられたときの動き
    //  Return : なし
    //  P.S.   :
    //-+-+-+-+-+-+-+-+-+-+-+-+
    private void DefeatMode()
    {
        if (mode != Mode.DEFEAT) return;

        /* 画面外下 */
        if (transform.position.y >= -7.0f)
        {
            /* アニメーションを停止 */
            GetComponent<Animator>().enabled = false;

            /* 死んだ画像にする */
            GetComponent<SpriteRenderer>().sprite = deadSprite;

            /* 移動する */
            transform.position -= transform.up * speed * Time.deltaTime;

            /* 音を止める */
            GetComponent<AudioSource>().Stop();
        }
        else
        {
            Destroy(gameObject);
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Name   : Stay20sec()
    //  Type   : void
    //  Desc   : 20秒間とどまる
    //  Return : なし
    //  P.S.   :
    //-+-+-+-+-+-+-+-+-+-+-+-+
    private void Stay20sec()
    {
        timer += Time.deltaTime;

        if (timer >= 20.0f)
        {
            SetMode(Mode.EXIT);
        }
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Name   : SetMode(Mode _mode)
    //  Type   : void
    //  Desc   : 外部からModeを変更する
    //  Return : なし
    //  P.S.   :
    //-+-+-+-+-+-+-+-+-+-+-+-+
    public void SetMode(Mode _mode)
    {
        mode = _mode;
    }

    private void OnBecameVisible()
    {
        GetComponent<AudioSource>().Play();
    }

    private void OnBecameInvisible()
    {
        GetComponent<AudioSource>().Stop();
    }
}
