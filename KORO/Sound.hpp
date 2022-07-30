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

// �Đ�����SoundBGM���w�肷��񋓌^
enum SoundFileBGM {
	Playing1,
	Playing2,
	Title1,
	Playing3,
	aaa,

	MaxNumBGM
};

// �Đ�����SoundSE���w�肷��񋓌^
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

// wavFile��ǂ݂��ލ\����
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

// �R���X�g���N�^�E�f�X�g���N�^
public:

	Sound();
	~Sound();

// �֐�
public:
	/// <summary>
	/// BGM�Đ�
	/// </summary>
	/// <param name="sound_file">�Đ�����t�@�C��</param>
	/// <param name="isLoop">���[�v���邩�ǂ���</param>
	/// <returns>����@true  ���s�@false</returns>
	bool PlayBGM(SoundFileBGM sound_file, bool isLoop);
	/// <summary>
	/// SE�Đ�
	/// </summary>
	/// <param name="sound_file">�Đ�����t�@�C��</param>
	/// <param name="isLoop">���[�v���邩�ǂ���</param>
	/// <returns>����@true  ���s�@false</returns>
	bool PlaySE(SoundFileSE sound_file, bool isLoop);
	/// <summary>
	/// BGM���~�߂�
	/// </summary>
	/// <param name="sound_file">��~����t�@�C��</param>
	/// <returns>����@true  ���s�@false</returns>
	bool StopBGM(SoundFileBGM sound_file);
	/// <summary>
	/// SE���~�߂�
	/// </summary>
	/// <param name="sound_file">��~����t�@�C��</param>
	/// <returns>����@true  ���s�@false</returns>
	bool StopSE(SoundFileSE sound_file);
	/// <summary>
	/// �Đ�����Sound�����ׂĒ�~
	/// </summary>
	/// <returns>����@true  ���s�@false</returns>
	bool StopAll();
	/// <summary>
	/// ���ʕύX�O�`�P(�����l��0.5)
	/// ������0.01
	/// </summary>
	/// <param name="updown">true = UP false = DOWN</param>
	/// <returns></returns>
	bool UpDownVolumeAll(bool updown);
	/// <summary>
	/// ���ʂ��Z�b�g
	/// </summary>
	/// <param name="vol">����(�L�����F�O�`�P)</param>
	/// <returns></returns>
	bool SetVolumeBuffer(float vol);

private:
	/// <summary>
	/// BGM�̃Z�J���_���o�b�t�@�𐶐�
	/// </summary>
	/// <returns>����@true  ���s�@false</returns>
	bool CreateSecondBufferBGM();
	/// <summary>
	/// SE�̃Z�J���_���o�b�t�@�𐶐�
	/// </summary>
	/// <returns>����@true  ���s�@false</returns>
	bool CreateSecondBufferSE();
	/// <summary>
	/// Sound�f�o�C�X��������
	/// </summary>
	/// <returns>����@true  ���s�@false</returns>
	bool InitSoundStatus();
	/// <summary>
	/// wav�t�@�C�������[�h
	/// </summary>
	/// <param name="fileName">WAV�t�@�C���̃p�X</param>
	/// <param name="wav_data">�ǂݍ���wav�t�@�C��������|�C���^</param>
	/// <returns>����@true  ���s�@false</returns>
	bool LoadWavFile(const char* fileName, WavData* wav_data);
	/// <summary>
	/// �������̊J��
	/// </summary>
	void ReleesSound();


// �ϐ�
public:
	

	float volume,
		  volumeBGM,
		  volumeSE;

	LPDIRECTSOUND8 sound_device; // �T�E���h�f�o�C�X

	LPDIRECTSOUNDBUFFER SoundBufferListBGM[SoundFileBGM::MaxNumBGM];  // BGM�Z�J���_���o�b�t�@�̃��X�g

	LPDIRECTSOUNDBUFFER SoundBufferListSE[SoundFileSE::MaxNumSE]; // SE�̃Z�J���_���o�b�t�@�̃��X�g


};