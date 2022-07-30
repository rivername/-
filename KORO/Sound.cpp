#include "Sound.hpp"



const char *BGM_File_str[] = {
	"resources/Sound/BGM/魔王魂 8bit20.wav",
	"resources/Sound/BGM/魔王魂 8bit29.wav",
	"resources/Sound/BGM/魔王魂 アコースティック52.wav",
	"resources/Sound/BGM/魔王魂 ヒーリング13.wav",
	"resources/Sound/BGM/"

};

const char *SE_File_str[] = {
	"resources/Sound/SE/魔王魂 ジングル01.wav",
	"resources/Sound/SE/魔王魂 ジングル09.wav",
	"resources/Sound/SE/魔王魂 効果音 システム35.wav",
	"resources/Sound/SE/魔王魂 効果音 システム46.wav",
	"resources/Sound/SE/魔王魂 効果音 システム48.wav",
	"resources/Sound/SE/魔王魂 効果音 ジングル12.wav",
	"resources/Sound/SE/魔王魂 効果音 ワンポイント27.wav",
	"resources/Sound/SE/魔王魂 効果音 笛01.wav",
	"resources/Sound/SE/"
};

Sound* Sound::instanse;

Sound::Sound()
{
	Sound::instanse = this;

	this->InitSoundStatus();

	this->CreateSecondBufferBGM();

	this->CreateSecondBufferSE();

}

Sound::~Sound()
{
	this->ReleesSound();
}



bool Sound::CreateSecondBufferBGM()
{
	for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++) {

		WavData wav_data;

		if (this->LoadWavFile(BGM_File_str[i], &wav_data) == false)
		{
			return false;
		}

		// バッファ情報の設定
		DSBUFFERDESC dsBuffa_desc;
		ZeroMemory(&dsBuffa_desc, sizeof(DSBUFFERDESC));
		dsBuffa_desc.dwSize = sizeof(DSBUFFERDESC);
		dsBuffa_desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
		dsBuffa_desc.dwBufferBytes = wav_data.Size;
		dsBuffa_desc.guid3DAlgorithm = DS3DALG_DEFAULT;
		dsBuffa_desc.lpwfxFormat = &wav_data.WavFormat;

		// セカンダリバッファ作成
		if (FAILED(this->sound_device->CreateSoundBuffer(
			&dsBuffa_desc,							// バッファ情報
			&this->SoundBufferListBGM[i],	// 作成されたバッファの保存先
			NULL)))
		{
			// 作成失敗
			delete[] wav_data.SoundBuffer;
			return false;
		}

		// 波形データを書き込むためにセカンダリバッファをロックする
		void* buffer;
		DWORD buffer_size;
		if (FAILED(this->SoundBufferListBGM[i]->Lock(
			0,				// オフセット
			wav_data.Size,	// ロックするバッファサイズ
			&buffer,		// ロックされたバッファパート１の保存先
			&buffer_size,	// ロックされたバッファパート１のサイズ
			NULL,			// ロックされたバッファパート２の保存先
			NULL,			// ロックされたバッファパート２のサイズ
			0)))			// ロックオプション
		{
			// ロック失敗
			delete[] wav_data.SoundBuffer;
			return false;
		}

		memcpy(buffer, wav_data.SoundBuffer, buffer_size);

		this->SoundBufferListBGM[i]->Unlock(
			&buffer,		// アンロックするバッファパート１
			buffer_size,	// パート１のバッファサイズ
			NULL,			// アンロックするバッファパート２
			NULL);			// パート２のバッファサイズ

		// コピーが終わったのでサウンドデータを解放
		delete[] wav_data.SoundBuffer;
	}

	return true;
}

bool Sound::CreateSecondBufferSE()
{
	for (int i = 0; i < SoundFileSE::MaxNumSE; i++) {

		WavData wav_data;

		if (this->LoadWavFile(SE_File_str[i], &wav_data) == false)
		{
			return false;
		}

		// バッファ情報の設定
		DSBUFFERDESC dsBuffa_desc;
		ZeroMemory(&dsBuffa_desc, sizeof(DSBUFFERDESC));
		dsBuffa_desc.dwSize = sizeof(DSBUFFERDESC);
		dsBuffa_desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
		dsBuffa_desc.dwBufferBytes = wav_data.Size;
		dsBuffa_desc.guid3DAlgorithm = DS3DALG_DEFAULT;
		dsBuffa_desc.lpwfxFormat = &wav_data.WavFormat;

		// セカンダリバッファ作成
		if (FAILED(this->sound_device->CreateSoundBuffer(
			&dsBuffa_desc,							// バッファ情報
			&this->SoundBufferListSE[i],	// 作成されたバッファの保存先
			NULL)))
		{
			// 作成失敗
			delete[] wav_data.SoundBuffer;
			return false;
		}

		// 波形データを書き込むためにセカンダリバッファをロックする
		void* buffer;
		DWORD buffer_size;
		if (FAILED(this->SoundBufferListSE[i]->Lock(
			0,				// オフセット
			wav_data.Size,	// ロックするバッファサイズ
			&buffer,		// ロックされたバッファパート１の保存先
			&buffer_size,	// ロックされたバッファパート１のサイズ
			NULL,			// ロックされたバッファパート２の保存先
			NULL,			// ロックされたバッファパート２のサイズ
			0)))			// ロックオプション
		{
			// ロック失敗
			delete[] wav_data.SoundBuffer;
			return false;
		}

		memcpy(buffer, wav_data.SoundBuffer, buffer_size);

		this->SoundBufferListSE[i]->Unlock(
			&buffer,		// アンロックするバッファパート１
			buffer_size,	// パート１のバッファサイズ
			NULL,			// アンロックするバッファパート２
			NULL);			// パート２のバッファサイズ

		// コピーが終わったのでサウンドデータを解放
		delete[] wav_data.SoundBuffer;
	}

	return true;
}

bool Sound::InitSoundStatus()
{

	// デバイスの生成
	if (FAILED(DirectSoundCreate8(
		NULL,
		&this->sound_device,
		NULL)))
	{
		return false;
	}

	// 強調レベルの設定
	if (FAILED(this->sound_device->SetCooperativeLevel(
		Window::instance->GetHWND(),
		DSSCL_NORMAL
	)))
	{
		return false;
	}

	this->volume    = 70.0f;
	this->volumeBGM = std::lerp(-8000.0f, -1000.0f, this->volume / 100.0f);
	this->volumeSE  = std::lerp(-8000.0f, -1000.0f, this->volume / 100.0f);
	
	return false;
}

bool Sound::PlayBGM(SoundFileBGM sound_file, bool isLoop)
{

	if (this->SoundBufferListBGM[sound_file] == NULL)
	{
		return false;
	}

	int loop_bit = isLoop == true ? 1 : 0;

	this->SoundBufferListBGM[sound_file]->SetVolume(this->volumeBGM);

	this->SoundBufferListBGM[sound_file]->Play(
		0,
		0,
		DSBPLAY_LOOPING & loop_bit
	);

	return true;
}

bool Sound::PlaySE(SoundFileSE sound_file, bool isLoop)
{
	if (this->SoundBufferListSE[sound_file] == NULL)
	{
		return false;
	}

	int loop_bit = isLoop == true ? 1 : 0;

	this->SoundBufferListSE[sound_file]->SetVolume(this->volumeSE);

	this->SoundBufferListSE[sound_file]->Play(
		0,
		0,
		DSBPLAY_LOOPING & loop_bit
	);

	return true;
}

bool Sound::StopBGM(SoundFileBGM sound_file)
{
	if (this->SoundBufferListBGM[sound_file] == NULL)
	{
		return false;
	}

	// 停止
	this->SoundBufferListBGM[sound_file]->Stop();
	// 再生位置を先頭に戻す
	this->SoundBufferListBGM[sound_file]->SetCurrentPosition(NULL);

	return true;
}

bool Sound::StopSE(SoundFileSE sound_file)
{
	if (this->SoundBufferListSE[sound_file] == NULL)
	{
		return false;
	}

	// 停止
	this->SoundBufferListSE[sound_file]->Stop();
	// 再生位置を先頭に戻す
	this->SoundBufferListSE[sound_file]->SetCurrentPosition(NULL);

	return true;
}

bool Sound::StopAll()
{

	for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++) 
	{
		if (this->SoundBufferListBGM[i] == NULL)
		{
			return false;
		}

		// 停止
		this->SoundBufferListBGM[i]->Stop();
		// 再生位置を先頭に戻す
		this->SoundBufferListBGM[i]->SetCurrentPosition(NULL);
	}


	for (int i = 0; i < SoundFileSE::MaxNumSE; i++) 
	{
		if (this->SoundBufferListSE[i] == NULL)
		{
			return false;
		}

		// 停止
		this->SoundBufferListSE[i]->Stop();
		// 再生位置を先頭に戻す
		this->SoundBufferListSE[i]->SetCurrentPosition(NULL);
	}


	return false;
}

bool Sound::UpDownVolumeAll(bool updown)
{
	
	// 引数でUP or DOWN
	if(updown)
	{
		if (this->volume != 100.0f)
		{
			this->volume += 1.0f;
		}
	}
	else
	{
		if (this->volume != 0.0f)
		{
			this->volume -= 1.0f;
		}
	}

	// -1000から-10000の値で指定 -8000以下は切り捨て
	this->volumeBGM = std::lerp(-8000.0f ,-1000.0f, this->volume / 100.0f);
	this->volumeSE  = std::lerp(-8000.0f ,-1000.0f, this->volume / 100.0f);
	
	// -8000の場合は-10000(ミュート)に
	if (this->volumeBGM == -8000.0f) { volumeBGM = -10000.0; }
	if (this->volumeSE  == -8000.0f) { volumeSE  = -10000.0; }

	// BGMのすべてに適用
	for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++) 
	{
		if (SoundBufferListBGM[i] == NULL) { break; }

		SoundBufferListBGM[i]->SetVolume(this->volumeBGM);
	}

	// SEのすべてに適用
	for (int i = 0; i < SoundFileSE::MaxNumSE; i++) 
	{
		SoundBufferListSE[i]->SetVolume(this->volumeSE);
	}
	

	return true;
}

bool Sound::SetVolumeBuffer(float vol)
{
	// 1から100以外の数値が入力されたら無視
	if (vol < 100.0f || vol > 0.0f) {

		this->volume = vol;

		// -1000から-10000の値で指定 -8000以下は切り捨て
		this->volumeBGM = std::lerp(-8000.0f, -1000.0f, this->volume);
		this->volumeSE = std::lerp(-8000.0f, -1000.0f, this->volume);

		// -8000の場合は-10000(ミュート)に
		if (this->volumeBGM == -8000.0f) { volumeBGM = -10000.0; }
		if (this->volumeSE == -8000.0f) { volumeSE = -10000.0; }

		// BGMのすべてに適用
		for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++)
		{
			if (SoundBufferListBGM[i] == NULL) { break; }

			SoundBufferListBGM[i]->SetVolume(this->volumeBGM);
		}

		// SEのすべてに適用
		for (int i = 0; i < SoundFileSE::MaxNumSE; i++)
		{
			SoundBufferListSE[i]->SetVolume(this->volumeSE);
		}

		return true;
	}

	return false;
}

bool Sound::LoadWavFile(const char* fileName, WavData* wav_data)
{

	// マルチメディアのAPIハンドル
	HMMIO handle = NULL;

	// チャンク情報
	MMCKINFO chunk_info;

	// RIFFチャンク情報
	MMCKINFO riff_chunk_info;

	// WAVサイズ
	DWORD wave_size = 0;

	// WAVファイルを開く
	handle = mmioOpen(
		(char*)fileName,
		NULL,
		MMIO_READ
	);

	if (handle == NULL) {
		return false;
	}

	// RIFFチャンクに進入するためにfccTypeにWAVEを設定する
	riff_chunk_info.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	// RIFFチャンクに侵入
	if (MMSYSERR_NOERROR != mmioDescend(
		handle,
		&riff_chunk_info,
		NULL,
		MMIO_FINDRIFF
	))
	{
		// 失敗
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	chunk_info.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (MMSYSERR_NOERROR != mmioDescend(handle, &chunk_info, &riff_chunk_info, MMIO_FINDCHUNK))
	{
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	LONG read_size = mmioRead(
		handle,
		(HPSTR)&wav_data->WavFormat,
		sizeof(wav_data->WavFormat
	));

	if (read_size != sizeof(wav_data->WavFormat))
	{
		// 読み込みサイズが一致してないのでエラー
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// フォーマットチェック
	if (wav_data->WavFormat.wFormatTag != WAVE_FORMAT_PCM)
	{
		// フォーマットエラー
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// fmtチャンクを退出する
	if (mmioAscend(handle, &chunk_info, 0) != MMSYSERR_NOERROR)
	{
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// dataチャンクに進入する
	chunk_info.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(handle, &chunk_info, &riff_chunk_info, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		// 進入失敗
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// サイズを保存
	wav_data->Size = chunk_info.cksize;

	// dataチャンク読み込み
	wav_data->SoundBuffer = new char[chunk_info.cksize];
	read_size = mmioRead(handle, (HPSTR)wav_data->SoundBuffer, chunk_info.cksize);
	if (read_size != chunk_info.cksize)
	{
		mmioClose(handle, MMIO_FHOPEN);
		delete[] wav_data->SoundBuffer;
		return false;
	}

	// ファイルを閉じる
	mmioClose(handle, MMIO_FHOPEN);



	return true;
}

void Sound::ReleesSound()
{

	for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++) 
	{
		if (this->SoundBufferListBGM[i] != NULL)
		{
			SoundBufferListBGM[i]->Stop();
			SoundBufferListBGM[i]->Release();
			SoundBufferListBGM[i] = NULL;
		}
	}

	for (int i = 0; i < SoundFileSE::MaxNumSE; i++)
	{
		if (this->SoundBufferListSE[i] != NULL)
		{
			SoundBufferListSE[i]->Stop();
			SoundBufferListSE[i]->Release();
			SoundBufferListSE[i] = NULL;
		}
	}
	SAFE_RELEASE(this->sound_device);
}
