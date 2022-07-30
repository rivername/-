using UnityEngine;

public class Blaster : MonoBehaviour
{
    public float firstSpeed;  /* 初速 */
    public float secondSpeed; /* 加速後の速度 */

    private float _speed;     /* 弾の速度 */

    private int aliveCounter = 0; /* Sceneにいる時間フレームをカウントする */

    /* ブラスター着弾点サイト */
    public GameObject blasterPointSight;
    
    /* ブラスター着弾点の位置 */
    private Vector3 blasterSightPos;

    private GameObject _GENERATED;

    void Start()
    {
        _speed = firstSpeed;

        /* プレイヤーの子オブジェクト3つ目（ブラスター発射時のサイト） */
        blasterSightPos = FindObjectOfType<Player>().transform.GetChild(0).transform.position;

        /* 着弾点サイトを生成 */
        _GENERATED = Instantiate
        (
            blasterPointSight,

            /* プレイヤーのサイトの位置に生成 (Z = 1(プレイヤーの一つ下)) */
            new Vector3
            (
                blasterSightPos.x,
                blasterSightPos.y,
                GameObject.FindGameObjectWithTag("Player").transform.position.z + 1
            ),

            /* ブラスター発射時のサイトと同じ向き */
            Quaternion.Euler(new Vector3(blasterSightPos.x, blasterSightPos.y))
        );
    }

    void Update()
    {
        Move();
        SightMove();
        Fell();
    }

    void Move()
    {
        transform.position += transform.up * _speed * Time.deltaTime;

        /* 10フレーム経過したら */
        if (aliveCounter >= 10)
        {
            _speed = secondSpeed;
        }

        aliveCounter++;
    }

    /* 背景スクロールに合わせる */
    void SightMove()
    {
        GameObject.FindGameObjectWithTag("BlasterSightPoint").transform.position -= transform.up * 1.0f * Time.deltaTime;
    }

    /* 着弾判定 */
    void Fell()
    {
        float distance = Vector2.Distance(transform.position, _GENERATED.transform.position);

        if (distance <= 0.2f)
        {
            /* BlasterSightPointの当たり判定をオン */
            _GENERATED.GetComponent<BoxCollider2D>().enabled = true;
        }

        if (distance <= 0.1f)
        {
            Destroy(_GENERATED);
            Destroy(gameObject);
        }
    }
}
