using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Sight /* プレイヤーのサイト */ : MonoBehaviour
{
    /* 地上の敵にホバーしたとき */
    void OnTriggerStay2D(Collider2D collision)
    {
        foreach (var ge in FindObjectOfType<Player>().groundEnemies)
        {
            if (collision.tag.Contains(ge))
            {
                FindObjectOfType<Player>().HoveredOnGroundEnemy(true);
            }
        }
    }

    /* 地上の敵のホバーをやめたとき */
    private void OnTriggerExit2D(Collider2D collision)
    {
        foreach (var ge in FindObjectOfType<Player>().groundEnemies)
        {
            if (collision.tag.Contains(ge))
            {
                FindObjectOfType<Player>().HoveredOnGroundEnemy(false);
            }
        }
    }
}
