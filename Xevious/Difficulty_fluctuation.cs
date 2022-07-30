using System.Collections;
using UnityEngine;

public class Difficulty_fluctuation : MonoBehaviour
{
    bool[] called_once = new bool[16]; // new で 初期値はすべて false

    // Update is called once per frame
    void Update()
    {
        
        switch (Status.STAGE_NUMBER)
        {
            case 1:
                if (!called_once[Status.STAGE_NUMBER - 1])
                {
                    called_once[Status.STAGE_NUMBER - 1] = true;            // ↓        ここの数字を変える     ↓
                    StartCoroutine(Diff_fluctuation(Status.STAGE_NUMBER ,Area_01_Sky.spawn_start_end, Area_01_Sky.difficulty_update));
                }
                break;
            case 2:
                if (!called_once[Status.STAGE_NUMBER - 1])
                {
                    called_once[Status.STAGE_NUMBER - 1] = true;
                    StartCoroutine(Diff_fluctuation(Status.STAGE_NUMBER, Area_02_Sky.spawn_start_end, Area_02_Sky.difficulty_update));
                }
                break;
            case 3:
                if (!called_once[Status.STAGE_NUMBER - 1])
                {
                    called_once[Status.STAGE_NUMBER - 1] = true;
                    StartCoroutine(Diff_fluctuation(Status.STAGE_NUMBER, Area_03_Sky.spawn_start_end, Area_03_Sky.difficulty_update));
                }
                break;
            default:
                break;
        }
    }

    /// <summary>
    /// '!'の場所になったら難易度を加算、'?'の場所になったら難易度が 128 以上か判定する
    /// </summary>
    /// <param name="stageNum"> コルーチン開始時のステージ番号 </param>
    /// <param name="spawn_start_end"> 各 Area_xx_Sky の配列 spawn_start_end </param>
    /// <param name="difficulty_update"> 各 Area_xx_Sky の配列 difficulty_update </param>
    /// <returns></returns>
    IEnumerator Diff_fluctuation(int stageNum, int[,] spawn_start_end, int[] difficulty_update)
    {
        int i = 0, j = 0;
        while (true)
        {
            //このエリアが終わった
            if (Status.STAGE_NUMBER != stageNum)
            {
                yield break;
            }

            /* ! */
            if (i < difficulty_update.Length  //配列外参照させない
                && difficulty_update[i] == Status.MAP_POSITION)  //難易度加算場所になった
            {
                Status.DIFFICULTY += Diff_increase();
                i++;
            }

            /* ? */
            if(j < spawn_start_end.Length / 2  //配列外参照させない
                && spawn_start_end[j,0] == Status.MAP_POSITION)  //'?'の場所で、難易度が 128 超えてたら -64 する
            {
                j++;
                if(Status.DIFFICULTY >= 128)
                {
                    Status.DIFFICULTY -= 64;
                }
                //全体の難易度を変更する場合、ここでも難易度加算
            }

            //フレーム毎
            yield return null;
        }
    }




    /// <summary>
    /// マップの'!'マークの位置の難易度増加量
    /// </summary>
    /// <returns>'!'の難易度増加量</returns>
    int Diff_increase()
    {
        //算出手順
        //1...スコアの1/1000(切り捨て)
        int increase = (int)Mathf.Floor(Status.SCORE/1000.0f);
        //2...先頭に0xをつけて16進数とする
        increase = System.Convert.ToInt32($"0x{increase}", 16);
        //3...出撃数(死んだ数+1)で割る
        increase /= Status.DEATHES+1;
        //4...得られた数を16以下にする(16以下ならそのまま)
        if(increase > 16) increase = 16;

        //完成
        return increase;
    }
}
