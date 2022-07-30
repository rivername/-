using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Map_controller : MonoBehaviour
{
    public GameObject Load_map;    //マップオブジェクト
    public GameObject Field_Map;   //マップオブジェクト
    public GameObject Player_obj;  //プレイヤー

    Vector2 CameraUp;   //カメラの上座標

    public static bool AppMap; //マップ描画状態

    private bool one;



    // Start is called before the first frame update
    void Start()
    {

        //マップ描画状態
        AppMap = false;

        one = false;

        //カメラの座標
        CameraUp = Camera.main.ViewportToWorldPoint(new Vector2(1, 1));

        //最初のロードマップを表示
        Instantiate(Load_map, new Vector3(0, 5, 11), new Quaternion(0,0,0,0));

        //Status.STAGE_NUMBER = 5;

    }

    // Update is called once per frame
    void Update()
    {
        //マップがない場合ステージ数を増やして

       
        if (Status.DEAD)
        {
            if (!one)
            {
                StartCoroutine("ResetMap");
                one = true;
            }
        }
        else
        {
            if (AppMap == false)
            {
                //マップ描画状態を変更
                AppMap = true;

                //マップ配置関数
                SetMap(Status.STAGE_NUMBER);

            }
        }
    }


    public void SetMap(int stage)
    {
        //ステージごとに描画
        switch (stage)
        {
            case 1:
                Instantiate(Field_Map, new Vector3(-25.0f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 2:
                Instantiate(Field_Map, new Vector3(-34.29f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));  
                break;

            case 3:
                Instantiate(Field_Map, new Vector3(-11.5f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 4:
                Instantiate(Field_Map, new Vector3(-32f, CameraUp.y - 40, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 5:
                Instantiate(Field_Map, new Vector3(-16.1f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

                /*
            case 6:
                Instantiate(Field_Map, new Vector3(-27.93f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 7:
                Instantiate(Field_Map, new Vector3(-6.74f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 8:
                Instantiate(Field_Map, new Vector3(-34.1f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 9:
                Instantiate(Field_Map, new Vector3(-23f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 10:
                Instantiate(Field_Map, new Vector3(-8.4f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 11:
                Instantiate(Field_Map, new Vector3(-32.5f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 12:
                Instantiate(Field_Map, new Vector3(-34.2f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 13:
                Instantiate(Field_Map, new Vector3(-18.3f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 14:
                Instantiate(Field_Map, new Vector3(-10.8f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 15:
                Instantiate(Field_Map, new Vector3(-27.8f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;

            case 16:
                Instantiate(Field_Map, new Vector3(-34.2f, CameraUp.y, 10), new Quaternion(0, 0, 0, 0));
                break;
                */

            default:
                Status.STAGE_NUMBER = 1;
                AppMap = false;
                break;
        }   
    }

    IEnumerator ResetMap()
    {
        yield return new WaitForSeconds(2.0f);

        if (Status.SHIP_STOCKS < 0)
        {
            GameOver.Show();
            yield break;
        }

        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
    }
}
