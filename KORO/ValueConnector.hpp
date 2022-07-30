#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include "GameObject.hpp"
#pragma once

using std::string;

using std::ios;

template <class T>
class VC;

template <class T>
class VC_Vector;

class VC_Obj : public GameObject
{
public:
    virtual bool SaveValue() = 0;

public:
    void WithPath(string filename)
    {
        this->filename = filename;
    }

public:
    virtual void EventBeginPlay() override
    {
    }
    virtual void EventTick(float deltaTime) override
    {
    }
    virtual void EventKeyDown(KeyEvent e) override
    {
    }
    virtual void EventKeyUp(KeyEvent e) override
    {
    }
    virtual void EventMouseDown(MouseEvent e) override
    {
    }
    virtual void EventMouseUp(MouseEvent e) override
    {
    }

public:
    string filename;

    bool writeOnly; // 書き込み専用
    bool readOnly;  // 読み込み専用

    bool fileExist; // ファイルが存在したかどうか
};

namespace valueconnector
{
    inline std::vector<VC_Obj*>& get_all_valueconnector()
    {
        static std::vector<VC_Obj*> list;
        return list;
    }

    inline void add_to_list(VC_Obj* valueConnector)
    {
        get_all_valueconnector().push_back(valueConnector);
    }
}

#pragma region 呼び出し用関数

/*==================================
           ValueConnector
  - 変数とファイルをリンクさせる
  - 起動時に値を読み込み終了時に値を保存

               使い方
 1. ValueConnector::Connect(...)で値を接続
==================================*/
namespace ValueConnector
{
    /// <summary>
    /// 値を接続 - 単一変数
    /// </summary>
    /// <param name="filename">：接続するファイル名</param>
    /// <param name="data">：接続するデータのアドレス</param>
    template <class T>
    inline VC_Obj* Connect(
        __in    string filename,
        __inout T*     data,
        __in    bool   writeOnly = false,
        __in    bool   readOnly  = false)
    {
        auto vc = new VC<T>(filename, data);
        
        vc->writeOnly = writeOnly;
        vc->readOnly  = readOnly;

        if (!writeOnly) vc->LoadValue();

        return vc;
    }

    /// <summary>
    /// 値を接続 - std::vector
    /// </summary>
    /// <param name="filename"> ：接続するファイル名</param>
    /// <param name="data_list">：接続するvectorデータのアドレス</param>
    template <class T>
    inline VC_Obj* Connect(
        __in    string          filename,
        __inout std::vector<T>* data_list,
        __in    bool            writeOnly = false,
        __in    bool            readOnly  = false)
    {
        auto vc = new VC_Vector<T>(filename, data_list);
        
        vc->writeOnly = writeOnly;
        vc->readOnly  = readOnly;

        if (!writeOnly) vc->LoadValue();

        return vc;
    }

    inline void SaveAll()
    {
        for (auto& vc : valueconnector::get_all_valueconnector())
        {
            vc->SaveValue();
        }
    }
}

#pragma endregion

/*==================================
       ValueConnector - 単一変数
==================================*/
template <class T>
class VC : public VC_Obj
{
public:
    VC(
        __in    string filename,
        __inout T*     data)
    {
        this->filename = filename;
        this->data     = data;

        valueconnector::add_to_list(this);
    }

public:
    bool LoadValue()
    {
        // ファイルを Read - バイナリ で読み込む
        std::ifstream fin(this->filename, ios::in | ios::binary);

        // ファイル存在セット
        this->fileExist = (bool)fin;

        if (!fin) return false;

        // 読み込み
        fin.read((char*)this->data, sizeof(T));

        // 終了
        fin.close();

        return true;
    }

    virtual bool SaveValue() override
    {
        if (this->readOnly) return true;

        // ファイルを Write - バイナリ - 上書き で読み込む
        std::ofstream fout(this->filename, ios::out | ios::trunc | ios::binary);

        if (!fout) return false;

        // 書き込み
        fout.write((char*)this->data, sizeof(T));

        // 終了
        fout.close();

        return true;
    }

private:
    T* data;
};

/*==================================
     ValueConnector - std::vector
==================================*/
template <class T>
class VC_Vector : public VC_Obj
{
public:
    VC_Vector(
        __in    string          filename,
        __inout std::vector<T>* data)
    {
        this->filename = filename;
        this->data     = data;
        valueconnector::add_to_list(this);
    }

public:
    bool LoadValue()
    {
        // ファイルを Read - バイナリ で読み込む
        std::ifstream fin(this->filename, ios::in | ios::binary);

        // ファイル存在セット
        this->fileExist = (bool)fin;

        if (!fin) return false;

        for (int i = 0; !fin.eof(); i++)
        {
            T tmp = {};

            fin.seekg(sizeof(T) * i);
            fin.read((char*)&tmp, sizeof(T));

            this->data->push_back(tmp);
        }

        this->data->resize(this->data->size() - 1);

        fin.close();

        return true;
    }

    virtual bool SaveValue() override
    {
        if (this->readOnly) return true;

        // ファイルを Write - バイナリ - 上書き で読み込む
        std::ofstream fout(this->filename, ios::out | ios::trunc | ios::binary);

        if (!fout) return false;

        for (T& e : *this->data)
        {
            fout.write((char*)&e, sizeof(T));
        }

        // 終了
        fout.close();

        return true;
    }

private:
    std::vector<T>* data;
};
