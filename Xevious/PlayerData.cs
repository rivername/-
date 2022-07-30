using System;
using UnityEngine;

public class PlayerData
{
    public enum Type
    {
        HIGH_SCORE,
        DEATHES,
        KILLS_BY_ZAPPER,
        KILLS_BY_BLASTER,
        PICKED_UP_ITEM
    };

    #region SET
    public static void Set(Type dataType, float value)
    {
        PlayerPrefs.SetFloat(dataType.ToString(), value);
    }

    public static void Set(Type dataType, int value)
    {
        PlayerPrefs.SetInt(dataType.ToString(), value);
    }

    public static void Set(Type dataType, string value)
    {
        PlayerPrefs.SetString(dataType.ToString(), value);
    }
    #endregion

    #region GET
    public static float GetFloat(Type dataType)
    {
        return PlayerPrefs.GetFloat(dataType.ToString());
    }

    public static int GetInt(Type dataType)
    {
        return PlayerPrefs.GetInt(dataType.ToString());
    }

    public static string GetString(Type dataType)
    {
        return PlayerPrefs.GetString(dataType.ToString());
    }
    #endregion

    #region MODIFY
    public static void Save()
    {
        PlayerPrefs.Save();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  データを初期化する
    //-+-+-+-+-+-+-+-+-+-+-+-+
    public static void InitAll()
    {
        Set(Type.HIGH_SCORE,       40000);
        Set(Type.DEATHES,          0);
        Set(Type.KILLS_BY_ZAPPER,  0);
        Set(Type.KILLS_BY_BLASTER, 0);
        Set(Type.PICKED_UP_ITEM,   0);
        LoadAll();
        Save();
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Statusの変数に反映
    //-+-+-+-+-+-+-+-+-+-+-+-+
    public static void LoadAll()
    {
        Status.HIGH_SCORE       = GetInt(Type.HIGH_SCORE);
        Status.DEATHES          = GetInt(Type.DEATHES);
        Status.KILLS_BY_ZAPPER  = GetInt(Type.KILLS_BY_ZAPPER);
        Status.KILLS_BY_BLASTER = GetInt(Type.KILLS_BY_BLASTER);
        Status.PICKED_UP_ITEM   = GetInt(Type.PICKED_UP_ITEM);
    }

    //-+-+-+-+-+-+-+-+-+-+-+-+
    //  Playerデータに書き込み保存
    //-+-+-+-+-+-+-+-+-+-+-+-+
    public static void ApplyAll()
    {
        Set(Type.HIGH_SCORE,       Status.HIGH_SCORE);
        Set(Type.DEATHES,          Status.DEATHES);
        Set(Type.KILLS_BY_ZAPPER,  Status.KILLS_BY_ZAPPER);
        Set(Type.KILLS_BY_BLASTER, Status.KILLS_BY_BLASTER);
        Set(Type.PICKED_UP_ITEM,   Status.PICKED_UP_ITEM);
        Save();
    }
    #endregion
}
