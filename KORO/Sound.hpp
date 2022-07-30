#include "const.hpp"
#include "Window.hpp"
#include "utils.hpp"
#include "DX9.hpp"
#include "GameObject.hpp"
#include "Koro.hpp"
#include "Debug.hpp"
#include <format>
#include <string>
#include <dsound.h>
#include <stralign.h>

#pragma once

// 再生するSoundBGMを指定する列挙型
enum SoundFileBGM {
	Playing1,
	Playing2,
	Title1,
	Playing3,
	aaa,

	MaxNumBGM
};

// 再生するSoundSEを指定する列挙型
enum SoundFileSE {
	ClearSound1,
	ClearSound2,
	CursorHover1,
	Click1,
	Click2,
	Start1,
	NextStage1,
	TimeUp,

	MaxNumSE
};

// wavFileを読みこむ構造体
struct WavData
{
	WAVEFORMATEX WavFormat;
	char* SoundBuffer;
	DWORD Size;
};

class Sound
{

public:

	static Sound* instanse;

// コンストラクタ・デストラクタ
public:

	Sound();
	~Sound();

// 関数
public:
	/// <summary>
	/// BGM再生
	/// </summary>
	/// <param name="sound_file">再生するファイル</param>
	/// <param name="isLoop">ループするかどうか</param>
	/// <returns>正常　true  失敗　false</returns>
	bool PlayBGM(SoundFileBGM sound_file, bool isLoop);
	/// <summary>
	/// SE再生
	/// </summary>
	/// <param name="sound_file">再生するファイル</param>
	/// <param name="isLoop">ループするかどうか</param>
	/// <returns>正常　true  失敗　false</returns>
	bool PlaySE(SoundFileSE sound_file, bool isLoop);
	/// <summary>
	/// BGMを止める
	/// </summary>
	/// <param name="sound_file">停止するファイル</param>
	/// <returns>正常　true  失敗　false</returns>
	bool StopBGM(SoundFileBGM sound_file);
	/// <summary>
	/// SEを止める
	/// </summary>
	/// <param name="sound_file">停止するファイル</param>
	/// <returns>正常　true  失敗　false</returns>
	bool StopSE(SoundFileSE sound_file);
	/// <summary>
	/// 再生中のSoundをすべて停止
	/// </summary>
	/// <returns>正常　true  失敗　false</returns>
	bool StopAll();
	/// <summary>
	/// 音量変更０～１(初期値は0.5)
	/// 増加量0.01
	/// </summary>
	/// <param name="updown">true = UP false = DOWN</param>
	/// <returns></returns>
	bool UpDownVolumeAll(bool updown);
	/// <summary>
	/// 音量をセット
	/// </summary>
	/// <param name="vol">音量(有効数：０～１)</param>
	/// <returns></returns>
	bool SetVolumeBuffer(float vol);

private:
	/// <summary>
	/// BGMのセカンダリバッファを生成
	/// </summary>
	/// <returns>正常　true  失敗　false</returns>
	bool CreateSecondBufferBGM();
	/// <summary>
	/// SEのセカンダリバッファを生成
	/// </summary>
	/// <returns>正常　true  失敗　false</returns>
	bool CreateSecondBufferSE();
	/// <summary>
	/// Soundデバイスを初期化
	/// </summary>
	/// <returns>正常　true  失敗　false</returns>
	bool InitSoundStatus();
	/// <summary>
	/// wavファイルをロード
	/// </summary>
	/// <param name="fileName">WAVファイルのパス</param>
	/// <param name="wav_data">読み込んだwavファイルを入れるポインタ</param>
	/// <returns>正常　true  失敗　false</returns>
	bool LoadWavFile(const char* fileName, WavData* wav_data);
	/// <summary>
	/// メモリの開放
	/// </summary>
	void ReleesSound();


// 変数
public:
	

	float volume,
		  volumeBGM,
		  volumeSE;

	LPDIRECTSOUND8 sound_device; // サウンドデバイス

	LPDIRECTSOUNDBUFFER SoundBufferListBGM[SoundFileBGM::MaxNumBGM];  // BGMセカンダリバッファのリスト

	LPDIRECTSOUNDBUFFER SoundBufferListSE[SoundFileSE::MaxNumSE]; // SEのセカンダリバッファのリスト


};