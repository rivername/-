#include "Type.hpp"
#include "ValueConnector.hpp"
#include "lib/Neargye/nameof.hpp"

#include <sstream>
#include <filesystem>
#pragma once

/*==================================
         パターンデータローダー
==================================*/
template <class T>
class PatternDataLoader
{
public:
    PatternDataLoader() {}

    // PatternDataLoader
    // @param folderPath : 読み込むファイルがあるフォルダパス
    // @param binaryPath : Releaseビルド時に読み込むバイナリデータ
    PatternDataLoader(string folderPath, string binaryPath) : folderPath(folderPath), out({}), fileCount(0ULL), loadFileNum(0ULL), isReleaseBuild(false)
    {
        // ファイル・フォルダが存在したら書き込みのみ
        if (std::filesystem::exists(folderPath))
        {
            // バイナリファイル読み込み // 書き込みのみ
            ValueConnector::Connect(binaryPath, &this->datas, true);
        }
        // ファイル・フォルダが存在しなかったら読み込みのみ
        else
        {
            // バイナリファイル読み込み // 読み込みのみ
            ValueConnector::Connect(binaryPath, &this->datas, false, true);

            // リリースビルドです
            this->isReleaseBuild = true;

            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(this->folderPath))
        {
            if (!entry.exists()) continue;

            // ファイルパス
            std::stringstream ss;
            ss << entry.path();

            // ファイルパスを取り出す
            String str = ss.str();
            str = str.SubString(1);
            string path = str.ToString();
            path.resize(path.length() - 1);

            // 行
            string line;

            // ファイル
            std::ifstream fin(path);

            if (fin)
            {
                // このファイルの行を保存
                std::vector<string> lines;

                // 1行ずつ読み込む
                while (std::getline(fin, line))
                {
                    // 改行のみの場合は読み飛ばす
                    if (line.empty()) continue;

                    // コメントは読み飛ばす
                    if (line.c_str()[0] == '#') continue;
                    
                    // 行をリストに追加
                    lines.push_back(line);
                }

                // リストに追加
                this->files_lines.push_back(lines);

                // ファイルカウント++
                this->fileCount++;

                // ファイルを閉じる
                fin.close();
            }
        }
    }

public:
    // 読み込みを開始する
    // @param fileNum : ファイル番号
    void BeginLoad(size_t fileNum)
    {
        this->loadFileNum = fileNum;
    }

    // 読み込みを終了する
    void EndLoad()
    {
        // リリースビルドの場合は以下実行しない
        if (this->isReleaseBuild) return;

        // コピーを作成
        auto copy = this->out;

        // コピーをデータリストに追加
        this->datas.push_back(copy);

        // コピー元を初期化
        this->out = {};
    }

    // データを取得
    // @param key : キー
    // @return { String } 値
    String GetValue(string key)
    {
        // 読み込むファイルの行データ
        auto& lines = this->files_lines[this->loadFileNum];

        // 行数分回す
        // Stringに変換
        for (String line : lines)
        {
            // キーが一致
            if (line.Split('=')[0] == key)
            {
                // 文字列値を取り出す
                return line.Split('=')[1];
            }
        }

        return "";
    }

    // 文字列データを読み込み
    // @param key      : 設定キー
    // @param pOut     : 設定先アドレス
    // @param charSize : 文字サイズ
    bool LoadStringValue(string key, char* pOut, byte charSize)
    {
        // 値を取得
        String value = this->GetValue(key);

        if (value.raw.empty())
        {
            strcpy_s(pOut, 1, "");
            return false;
        }

        // ダブルクォーテーションで始まる場合はそれを削除する
        if (value.raw.c_str()[0] == '\"')
        {
            // 文字列を取り出す
            value = value.SubString(1);
            string path = value.ToString();
            path.resize(path.length() - 1);
            value = path;
        }

        // メモリをコピー
        strcpy_s(pOut, charSize, value.raw.c_str());

        return true;
    }

    // Enumデータを読み込み
    // @param key  : 設定キー
    // @param pOut : 設定先アドレス
    template <class EnumT>
    bool LoadEnumValue(string key, EnumT* pOut)
    {
        // 値を取得
        String value = this->GetValue(key);

        if (value.raw.empty()) return false;

        // Enum数分回す
        for (int i = 0; i < (int)EnumT::__num; i++)
        {
            // 比較するEnum
            auto eStr = nameof::nameof_enum((EnumT)i);

            // Enum文字列が一致したら
            if (value == eStr.data())
            {
                *pOut = (EnumT)i;

                break;
            }
        }

        return true;
    }

    // D3DXCOLORデータを読み込み
    // @param key  : 設定キー
    // @param pOut : 設定先アドレス
    bool LoadColorValue(string key, D3DXCOLOR* pOut)
    {
        // 値を取得
        String value = this->GetValue(key);

        // 指定がない場合は白
        if (value.raw.empty())
        {
            *pOut = 0xFFFFFF;
            return false;
        }

        // #から始まる場合 (Hex)
        if (value.raw.c_str()[0] == '#')
        {
            // DWORDカラーに変換
            *pOut = (DWORD)strtol((const char*)&value.raw.c_str()[1], nullptr, 16);
        }

        return true;
    }

public:
    // IDからデータを取得
    _NODISCARD T GetPatternDataFromId(string id)
    {
        for (auto& data : this->datas)
        {
            if (data.id == id)
            {
                return data;
            }
        }

        return {};
    }

public:
    string folderPath;
    T      out;
    size_t fileCount;   // 読み込んだファイルの数
    size_t loadFileNum; // データを読み込むファイル番号

    // リリースビルドかどうか
    bool isReleaseBuild;

    // 読み込んだすべてのデータ
    std::vector<T> datas;

    // すべてのファイルの行文字列
    std::vector<std::vector<string>> files_lines;
};
