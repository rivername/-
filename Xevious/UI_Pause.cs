using System;
using System.Collections;
using UnityEngine;
using UnityEngine.UI;

public class UI_Pause : MonoBehaviour
{
    /* ポーズキー */
    private KeyCode pauseKey;

    /* 選択中のボタン */
    private Button button;

    /* 現在の画面 */
    private UIScene currentUI;

    /* 表示するボタンのテキスト */
    public Text[] buttonsInPauseMenu;

    /* Panel or Text ゲームオブジェクト */
    private GameObject menu;
    private GameObject yourRecordText;
    private GameObject yourRecordValue;
    private GameObject rKeyToReset;
    private GameObject AYSTRYR;

    enum Button
    {
        RESUME_GAME,
        YOUR_RECORD,
        EXIT_GAME
    };

    enum UIScene
    {
        GAMEPLAY,
        PAUSE_SCREEN,
        RECORD_SCREEN,
        ARE_YOU_SURE_TO_RESET_YOUR_RECORD
    };

    void Start()
    {
        pauseKey = KeyCode.Escape;

        menu            = transform.GetChild(1).gameObject;
        yourRecordText  = transform.GetChild(2).gameObject;
        yourRecordValue = transform.GetChild(3).gameObject;
        rKeyToReset     = transform.GetChild(4).gameObject;
        AYSTRYR         = transform.GetChild(5).gameObject;
    }

    void Update()
    {
        /* ポーズキーが押された時 */
        if (Input.GetKeyDown(pauseKey))
        {
            SwitchUI();
        }

        UI__PauseMenu();
        UI__YourRecord();
        UI__AreYouSureToResetYourRecord();
    }

    void SwitchUI()
    {
        if (currentUI == UIScene.GAMEPLAY)
        {
            //-+-+-+-+-+-+-+-+-+-+-+-+
            //  ゲーム画面 -> ポーズ画面
            //-+-+-+-+-+-+-+-+-+-+-+-+

            /* PAUSE_SCENEにする */
            currentUI = UIScene.PAUSE_SCREEN;
            
            /* Resume選択状態にする */
            button = Button.RESUME_GAME;

            /* 全体音量 Change 1.0f to 0.4f */
            AudioListener.volume = 0.4f;

            /* タイムスケールを0 */
            Time.timeScale = 0.0f;

            /* Canvasをオン */
            GetComponent<Canvas>().enabled = true;

            return;
        }

        if (currentUI == UIScene.PAUSE_SCREEN)
        {
            //-+-+-+-+-+-+-+-+-+-+-+-+
            //  ポーズ画面 -> ゲーム画面
            //-+-+-+-+-+-+-+-+-+-+-+-+

            /* GAMEPLAYにする */
            currentUI = UIScene.GAMEPLAY;

            /* 全体音量 Change 0.4f to 1.0f */
            AudioListener.volume = 1.0f;

            /* タイムスケールを1 */
            Time.timeScale = 1.0f;

            /* Canvasをオフ */
            GetComponent<Canvas>().enabled = false;

            return;
        }

        if (currentUI == UIScene.RECORD_SCREEN)
        {
            //-+-+-+-+-+-+-+-+-+-+-+-+
            //  レコード画面 -> ポーズ画面
            //-+-+-+-+-+-+-+-+-+-+-+-+

            /* PAUSE_SCREENにする */
            currentUI = UIScene.PAUSE_SCREEN;

            return;
        }

        if (currentUI == UIScene.ARE_YOU_SURE_TO_RESET_YOUR_RECORD)
        {
            //-+-+-+-+-+-+-+-+-+-+-+-+
            //  記録リセット確認画面 -> レコード画面
            //-+-+-+-+-+-+-+-+-+-+-+-+

            /* RECORD_SCREENにする */
            currentUI = UIScene.RECORD_SCREEN;

            return;
        }
    }

    void UI__PauseMenu()
    {
        bool show = currentUI == UIScene.PAUSE_SCREEN;

        menu.SetActive(show);

        if (!show) return;

        /* 上キーを押したとき */
        if (Input.GetKeyDown(KeyCode.UpArrow))
        {
            if (button == Button.RESUME_GAME) button = Button.EXIT_GAME;
            else if (button == Button.YOUR_RECORD) button = Button.RESUME_GAME;
            else if (button == Button.EXIT_GAME)   button = Button.YOUR_RECORD;
        }

        /* 下キーを押したとき */
        if (Input.GetKeyDown(KeyCode.DownArrow))
        {
            if (button == Button.RESUME_GAME) button = Button.YOUR_RECORD;
            else if (button == Button.YOUR_RECORD) button = Button.EXIT_GAME;
            else if (button == Button.EXIT_GAME)   button = Button.RESUME_GAME;
        }

        /* ▶を表示 */
        foreach (Button value in Enum.GetValues(typeof(Button)))
        {
            string key = Enum.GetName(typeof(Button), value);

            buttonsInPauseMenu[(int)value].text = value.ToString().Replace("_", " ");

            if (button.ToString() == key)
            {
                buttonsInPauseMenu[(int)button].text = $"▶ {button.ToString().Replace("_", " ")}　 ";
            }
        }

        /* エンターキーが押された時 */
        if (Input.GetKeyDown(KeyCode.Return))
        {
            /* RESUME GAME が押された時 */
            if (button == Button.RESUME_GAME)
            {
                SwitchUI();
            }

            /* YOUR RECORD が押された時 */
            if (button == Button.YOUR_RECORD)
            {
                currentUI = UIScene.RECORD_SCREEN;
            }

            /* EXIT GAME が押された時 */
            if (button == Button.EXIT_GAME)
            {
                PlayerData.ApplyAll();
                Debug.Log(PlayerData.GetInt(PlayerData.Type.HIGH_SCORE));
                Debug.Log(PlayerData.GetInt(PlayerData.Type.DEATHES));
                Debug.Log(PlayerData.GetInt(PlayerData.Type.KILLS_BY_ZAPPER));
                Debug.Log(PlayerData.GetInt(PlayerData.Type.KILLS_BY_BLASTER));
                Debug.Log(PlayerData.GetInt(PlayerData.Type.PICKED_UP_ITEM));
                Player.ExitGame();
            }
        }
    }

    void UI__YourRecord()
    {
        bool show = currentUI == UIScene.RECORD_SCREEN;

        /* YOUR RECORDの表示・非表示 */
        yourRecordText.SetActive(show);
        yourRecordValue.SetActive(show);
        rKeyToReset.SetActive(show);

        if (show)
        {
            /* すべて保存 */
            PlayerData.ApplyAll();

            /* ハイスコア表示 */
            yourRecordValue.transform.GetChild(0).GetComponent<Text>().text =
                $"{PlayerData.GetInt(PlayerData.Type.HIGH_SCORE)}";

            /* デス数を表示 */
            yourRecordValue.transform.GetChild(1).GetComponent<Text>().text =
                $"{PlayerData.GetInt(PlayerData.Type.DEATHES)}";

            /* ザッパーで倒した数を表示 */
            yourRecordValue.transform.GetChild(2).GetComponent<Text>().text =
                $"{PlayerData.GetInt(PlayerData.Type.KILLS_BY_ZAPPER)}";

            /* ブラスターで倒した数を表示 */
            yourRecordValue.transform.GetChild(3).GetComponent<Text>().text =
                $"{PlayerData.GetInt(PlayerData.Type.KILLS_BY_BLASTER)}";

            /* とったアイテム数を表示 */
            yourRecordValue.transform.GetChild(4).GetComponent<Text>().text =
                $"{PlayerData.GetInt(PlayerData.Type.PICKED_UP_ITEM)}";
        }

        /* リセットキーが押された時 */
        if (Input.GetKeyDown(KeyCode.R) && show)
        {
            currentUI = UIScene.ARE_YOU_SURE_TO_RESET_YOUR_RECORD;
        }
    }

    void UI__AreYouSureToResetYourRecord()
    {
        bool show = currentUI == UIScene.ARE_YOU_SURE_TO_RESET_YOUR_RECORD;

        /* 表示・非表示 */
        AYSTRYR.SetActive(show);

        if (!show) return;

        Text yes = AYSTRYR.transform.GetChild(2).GetComponent<Text>();
        Text no  = AYSTRYR.transform.GetChild(3).GetComponent<Text>();

        /* 上下キーが押された時 */
        if (Input.GetKeyDown(KeyCode.DownArrow) || Input.GetKeyDown(KeyCode.UpArrow))
        {
            if (yes.text.Contains("▶"))
            {
                yes.text = "YES";
                no.text = "▶ NO　 ";
            }
            else
            {
                yes.text = "▶ YES　 ";
                no.text = "NO";
            }
        }

        /* エンターキーが押された時 */
        if (Input.GetKeyDown(KeyCode.Return))
        {
            if (yes.text.Contains("▶"))
            {
                GetComponent<AudioSource>().Play();
                PlayerData.InitAll();
                SwitchUI();
            }

            if (no.text.Contains("▶"))
            {
                SwitchUI();
            }
        }
    }
}
