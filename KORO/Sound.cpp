#include "Sound.hpp"



const char *BGM_File_str[] = {
	"resources/Sound/BGM/������ 8bit20.wav",
	"resources/Sound/BGM/������ 8bit29.wav",
	"resources/Sound/BGM/������ �A�R�[�X�e�B�b�N52.wav",
	"resources/Sound/BGM/������ �q�[�����O13.wav",
	"resources/Sound/BGM/"

};

const char *SE_File_str[] = {
	"resources/Sound/SE/������ �W���O��01.wav",
	"resources/Sound/SE/������ �W���O��09.wav",
	"resources/Sound/SE/������ ���ʉ� �V�X�e��35.wav",
	"resources/Sound/SE/������ ���ʉ� �V�X�e��46.wav",
	"resources/Sound/SE/������ ���ʉ� �V�X�e��48.wav",
	"resources/Sound/SE/������ ���ʉ� �W���O��12.wav",
	"resources/Sound/SE/������ ���ʉ� �����|�C���g27.wav",
	"resources/Sound/SE/������ ���ʉ� �J01.wav",
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

		// �o�b�t�@���̐ݒ�
		DSBUFFERDESC dsBuffa_desc;
		ZeroMemory(&dsBuffa_desc, sizeof(DSBUFFERDESC));
		dsBuffa_desc.dwSize = sizeof(DSBUFFERDESC);
		dsBuffa_desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
		dsBuffa_desc.dwBufferBytes = wav_data.Size;
		dsBuffa_desc.guid3DAlgorithm = DS3DALG_DEFAULT;
		dsBuffa_desc.lpwfxFormat = &wav_data.WavFormat;

		// �Z�J���_���o�b�t�@�쐬
		if (FAILED(this->sound_device->CreateSoundBuffer(
			&dsBuffa_desc,							// �o�b�t�@���
			&this->SoundBufferListBGM[i],	// �쐬���ꂽ�o�b�t�@�̕ۑ���
			NULL)))
		{
			// �쐬���s
			delete[] wav_data.SoundBuffer;
			return false;
		}

		// �g�`�f�[�^���������ނ��߂ɃZ�J���_���o�b�t�@�����b�N����
		void* buffer;
		DWORD buffer_size;
		if (FAILED(this->SoundBufferListBGM[i]->Lock(
			0,				// �I�t�Z�b�g
			wav_data.Size,	// ���b�N����o�b�t�@�T�C�Y
			&buffer,		// ���b�N���ꂽ�o�b�t�@�p�[�g�P�̕ۑ���
			&buffer_size,	// ���b�N���ꂽ�o�b�t�@�p�[�g�P�̃T�C�Y
			NULL,			// ���b�N���ꂽ�o�b�t�@�p�[�g�Q�̕ۑ���
			NULL,			// ���b�N���ꂽ�o�b�t�@�p�[�g�Q�̃T�C�Y
			0)))			// ���b�N�I�v�V����
		{
			// ���b�N���s
			delete[] wav_data.SoundBuffer;
			return false;
		}

		memcpy(buffer, wav_data.SoundBuffer, buffer_size);

		this->SoundBufferListBGM[i]->Unlock(
			&buffer,		// �A�����b�N����o�b�t�@�p�[�g�P
			buffer_size,	// �p�[�g�P�̃o�b�t�@�T�C�Y
			NULL,			// �A�����b�N����o�b�t�@�p�[�g�Q
			NULL);			// �p�[�g�Q�̃o�b�t�@�T�C�Y

		// �R�s�[���I������̂ŃT�E���h�f�[�^�����
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

		// �o�b�t�@���̐ݒ�
		DSBUFFERDESC dsBuffa_desc;
		ZeroMemory(&dsBuffa_desc, sizeof(DSBUFFERDESC));
		dsBuffa_desc.dwSize = sizeof(DSBUFFERDESC);
		dsBuffa_desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
		dsBuffa_desc.dwBufferBytes = wav_data.Size;
		dsBuffa_desc.guid3DAlgorithm = DS3DALG_DEFAULT;
		dsBuffa_desc.lpwfxFormat = &wav_data.WavFormat;

		// �Z�J���_���o�b�t�@�쐬
		if (FAILED(this->sound_device->CreateSoundBuffer(
			&dsBuffa_desc,							// �o�b�t�@���
			&this->SoundBufferListSE[i],	// �쐬���ꂽ�o�b�t�@�̕ۑ���
			NULL)))
		{
			// �쐬���s
			delete[] wav_data.SoundBuffer;
			return false;
		}

		// �g�`�f�[�^���������ނ��߂ɃZ�J���_���o�b�t�@�����b�N����
		void* buffer;
		DWORD buffer_size;
		if (FAILED(this->SoundBufferListSE[i]->Lock(
			0,				// �I�t�Z�b�g
			wav_data.Size,	// ���b�N����o�b�t�@�T�C�Y
			&buffer,		// ���b�N���ꂽ�o�b�t�@�p�[�g�P�̕ۑ���
			&buffer_size,	// ���b�N���ꂽ�o�b�t�@�p�[�g�P�̃T�C�Y
			NULL,			// ���b�N���ꂽ�o�b�t�@�p�[�g�Q�̕ۑ���
			NULL,			// ���b�N���ꂽ�o�b�t�@�p�[�g�Q�̃T�C�Y
			0)))			// ���b�N�I�v�V����
		{
			// ���b�N���s
			delete[] wav_data.SoundBuffer;
			return false;
		}

		memcpy(buffer, wav_data.SoundBuffer, buffer_size);

		this->SoundBufferListSE[i]->Unlock(
			&buffer,		// �A�����b�N����o�b�t�@�p�[�g�P
			buffer_size,	// �p�[�g�P�̃o�b�t�@�T�C�Y
			NULL,			// �A�����b�N����o�b�t�@�p�[�g�Q
			NULL);			// �p�[�g�Q�̃o�b�t�@�T�C�Y

		// �R�s�[���I������̂ŃT�E���h�f�[�^�����
		delete[] wav_data.SoundBuffer;
	}

	return true;
}

bool Sound::InitSoundStatus()
{

	// �f�o�C�X�̐���
	if (FAILED(DirectSoundCreate8(
		NULL,
		&this->sound_device,
		NULL)))
	{
		return false;
	}

	// �������x���̐ݒ�
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

	// ��~
	this->SoundBufferListBGM[sound_file]->Stop();
	// �Đ��ʒu��擪�ɖ߂�
	this->SoundBufferListBGM[sound_file]->SetCurrentPosition(NULL);

	return true;
}

bool Sound::StopSE(SoundFileSE sound_file)
{
	if (this->SoundBufferListSE[sound_file] == NULL)
	{
		return false;
	}

	// ��~
	this->SoundBufferListSE[sound_file]->Stop();
	// �Đ��ʒu��擪�ɖ߂�
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

		// ��~
		this->SoundBufferListBGM[i]->Stop();
		// �Đ��ʒu��擪�ɖ߂�
		this->SoundBufferListBGM[i]->SetCurrentPosition(NULL);
	}


	for (int i = 0; i < SoundFileSE::MaxNumSE; i++) 
	{
		if (this->SoundBufferListSE[i] == NULL)
		{
			return false;
		}

		// ��~
		this->SoundBufferListSE[i]->Stop();
		// �Đ��ʒu��擪�ɖ߂�
		this->SoundBufferListSE[i]->SetCurrentPosition(NULL);
	}


	return false;
}

bool Sound::UpDownVolumeAll(bool updown)
{
	
	// ������UP or DOWN
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

	// -1000����-10000�̒l�Ŏw�� -8000�ȉ��͐؂�̂�
	this->volumeBGM = std::lerp(-8000.0f ,-1000.0f, this->volume / 100.0f);
	this->volumeSE  = std::lerp(-8000.0f ,-1000.0f, this->volume / 100.0f);
	
	// -8000�̏ꍇ��-10000(�~���[�g)��
	if (this->volumeBGM == -8000.0f) { volumeBGM = -10000.0; }
	if (this->volumeSE  == -8000.0f) { volumeSE  = -10000.0; }

	// BGM�̂��ׂĂɓK�p
	for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++) 
	{
		if (SoundBufferListBGM[i] == NULL) { break; }

		SoundBufferListBGM[i]->SetVolume(this->volumeBGM);
	}

	// SE�̂��ׂĂɓK�p
	for (int i = 0; i < SoundFileSE::MaxNumSE; i++) 
	{
		SoundBufferListSE[i]->SetVolume(this->volumeSE);
	}
	

	return true;
}

bool Sound::SetVolumeBuffer(float vol)
{
	// 1����100�ȊO�̐��l�����͂��ꂽ�疳��
	if (vol < 100.0f || vol > 0.0f) {

		this->volume = vol;

		// -1000����-10000�̒l�Ŏw�� -8000�ȉ��͐؂�̂�
		this->volumeBGM = std::lerp(-8000.0f, -1000.0f, this->volume);
		this->volumeSE = std::lerp(-8000.0f, -1000.0f, this->volume);

		// -8000�̏ꍇ��-10000(�~���[�g)��
		if (this->volumeBGM == -8000.0f) { volumeBGM = -10000.0; }
		if (this->volumeSE == -8000.0f) { volumeSE = -10000.0; }

		// BGM�̂��ׂĂɓK�p
		for (int i = 0; i < SoundFileBGM::MaxNumBGM; i++)
		{
			if (SoundBufferListBGM[i] == NULL) { break; }

			SoundBufferListBGM[i]->SetVolume(this->volumeBGM);
		}

		// SE�̂��ׂĂɓK�p
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

	// �}���`���f�B�A��API�n���h��
	HMMIO handle = NULL;

	// �`�����N���
	MMCKINFO chunk_info;

	// RIFF�`�����N���
	MMCKINFO riff_chunk_info;

	// WAV�T�C�Y
	DWORD wave_size = 0;

	// WAV�t�@�C�����J��
	handle = mmioOpen(
		(char*)fileName,
		NULL,
		MMIO_READ
	);

	if (handle == NULL) {
		return false;
	}

	// RIFF�`�����N�ɐi�����邽�߂�fccType��WAVE��ݒ肷��
	riff_chunk_info.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	// RIFF�`�����N�ɐN��
	if (MMSYSERR_NOERROR != mmioDescend(
		handle,
		&riff_chunk_info,
		NULL,
		MMIO_FINDRIFF
	))
	{
		// ���s
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
		// �ǂݍ��݃T�C�Y����v���ĂȂ��̂ŃG���[
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// �t�H�[�}�b�g�`�F�b�N
	if (wav_data->WavFormat.wFormatTag != WAVE_FORMAT_PCM)
	{
		// �t�H�[�}�b�g�G���[
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// fmt�`�����N��ޏo����
	if (mmioAscend(handle, &chunk_info, 0) != MMSYSERR_NOERROR)
	{
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// data�`�����N�ɐi������
	chunk_info.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(handle, &chunk_info, &riff_chunk_info, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		// �i�����s
		mmioClose(handle, MMIO_FHOPEN);
		return false;
	}

	// �T�C�Y��ۑ�
	wav_data->Size = chunk_info.cksize;

	// data�`�����N�ǂݍ���
	wav_data->SoundBuffer = new char[chunk_info.cksize];
	read_size = mmioRead(handle, (HPSTR)wav_data->SoundBuffer, chunk_info.cksize);
	if (read_size != chunk_info.cksize)
	{
		mmioClose(handle, MMIO_FHOPEN);
		delete[] wav_data->SoundBuffer;
		return false;
	}

	// �t�@�C�������
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
