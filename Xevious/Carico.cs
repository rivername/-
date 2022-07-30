#if UNITY_EDITOR
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class Carico : MonoBehaviour
{
    private Transform c_EnemyBullet;
    private Transform c_Zapper;
    private Transform c_Blaster;
    private Transform c_Misc;

    void Start()
    {
        c_EnemyBullet = transform.GetChild(0);
        c_Zapper      = transform.GetChild(2);
        c_Blaster     = transform.GetChild(3);
        c_Misc        = transform.GetChild(4);
    }

    void Update()
    {
        /* 敵の弾を収納 */
        foreach (var enemyBullet in FindObjectsOfType<EnemyBullet>())
        {
            var eb = enemyBullet.gameObject;

            if (eb.transform.parent == null)
            {
                eb.transform.parent = c_EnemyBullet;
            }
        }

        /* ザッパーを収納 */
        foreach (var zapper in GameObject.FindGameObjectsWithTag("Zapper"))
        {
            if (zapper.transform.parent == null)
            {
                zapper.transform.parent = c_Zapper;
            }
        }

        /* ブラスターを収納 */
        foreach (var zapper in GameObject.FindGameObjectsWithTag("Blaster"))
        {
            if (zapper.transform.parent == null)
            {
                zapper.transform.parent = c_Blaster;
            }
        }

        /* ブラスター着弾点を収納 */
        var blasterSightPoint = GameObject.FindGameObjectWithTag("BlasterSightPoint");
        if (blasterSightPoint != null )
        {
            if (blasterSightPoint.transform.parent == null) blasterSightPoint.transform.parent = c_Blaster;
        }

        /* その他のオブジェクトを収納 */
        foreach (var clone in GetAllObjectsOnlyInScene())
        {
            if (clone.name.Contains("Clone") || clone.name.ToLower().Contains("audio"))
            {
                if (clone.transform.parent == null) clone.transform.parent = c_Misc;
            }
        }
    }

    public List<GameObject> GetAllObjectsOnlyInScene()
    {
        List<GameObject> objectsInScene = new List<GameObject>();

        foreach (GameObject go in Resources.FindObjectsOfTypeAll(typeof(GameObject)) as GameObject[])
        {
            if (!EditorUtility.IsPersistent(go.transform.root.gameObject) && !(go.hideFlags == HideFlags.NotEditable || go.hideFlags == HideFlags.HideAndDontSave))
                objectsInScene.Add(go);
        }

        return objectsInScene;
    }
}
#endif
