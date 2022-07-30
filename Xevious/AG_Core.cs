using UnityEngine;

public class AG_Core : MonoBehaviour
{
    public GameObject explosion;
    public GameObject bragza;

    public AudioClip ExplosionAudioClip;

    private void OnTriggerEnter2D(Collider2D collision)
    {
        /* 撃たれた時 */
        if (collision.tag == "BlasterSightPoint")
        {
            /* 爆破オブジェクト生成 */
            Instantiate(explosion, transform.position, transform.rotation);

            /* ブラグザ生成 */
            Instantiate(bragza, transform.position, transform.rotation);

            /* 爆破音再生 */
            Player.PlayAtPoint2D(ExplosionAudioClip);
            
            /* 1000ポイント加点 */
            Status.SCORE += 4000;
            
            /* Defeatモードにする */
            FindObjectOfType<AG_Body>().SetMode(AG_Body.Mode.DEFEAT);

            /* 破壊 */
            Destroy(gameObject);
        }
    }
}
