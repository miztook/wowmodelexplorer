#include "stdafx.h"
#include "CDSSound.h"

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_AUDIO

#include "mywow.h"
#include "CWavInput.h"
#include "CVorbisInput.h"
#include "CMP3Input.h"

#define  NUM_PLAY_NOTIFICATIONS 16

CDSSound::CDSSound(LPDIRECTSOUND pIDS, E_SOUND_TYPE type, u32 index)
	: pIDSound(pIDS), pIDSBuffer(NULL_PTR), pIDNotify(NULL_PTR), ISound(type), Index(index), Callback(NULL_PTR)
{
	INIT_LOCK(&cs);
	INIT_EVENT(&NotificationEvent, "");
	PosNotifyArray = new DSBPOSITIONNOTIFY[NUM_PLAY_NOTIFICATIONS];

	BufferSize = 0;
	NotifySize = 0;
	Volume = 1.0f;

	switch (type)
	{
	case EST_WAV:
		AudioInput = new CWavInput;
		break;
	case EST_OGG:
		AudioInput = new CVorbisInput;
		break;
	case EST_MP3:
		AudioInput = new CMP3Input;
		break;
	default:
		ASSERT(false);
		AudioInput = NULL_PTR;
		break;
	}

	memset(&LastWFX, 0, sizeof(WAVEFORMATEX));
	memset(&WFX, 0, sizeof(WAVEFORMATEX));

	reset();
}

CDSSound::~CDSSound()
{
	unload();
	delete AudioInput;

	SAFE_RELEASE_STRICT(pIDSBuffer);
	SAFE_RELEASE_STRICT(pIDNotify);
	delete[] PosNotifyArray;
	DESTROY_EVENT(&NotificationEvent);
	DESTROY_LOCK(&cs);
}

bool CDSSound::load( const c8* filename )
{
	unload();

	BEGIN_LOCK(&cs);
	bool ret = AudioInput->openFile(filename);
	if (!ret)
	{
		END_LOCK(&cs);
		ASSERT(false);
		return false;
	}
	END_LOCK(&cs);

	//create dsbuffer
	memset(&WFX, 0, sizeof(WAVEFORMATEX));
	WFX.cbSize = 0;
	WFX.wFormatTag = WAVE_FORMAT_PCM;
	WFX.nSamplesPerSec = (DWORD)AudioInput->SampleRate;
	WFX.nChannels = (WORD)AudioInput->NumChannels;
	WFX.wBitsPerSample = (WORD)AudioInput->BitsPerSample;
	WFX.nBlockAlign = (WORD)AudioInput->getSampleSize();
	WFX.nAvgBytesPerSec = WFX.nSamplesPerSec * WFX.nBlockAlign;
	
	if (WFX.wFormatTag != LastWFX.wFormatTag ||
		WFX.nChannels != LastWFX.nChannels ||
		WFX.nSamplesPerSec != LastWFX.nSamplesPerSec ||
		WFX.nAvgBytesPerSec != LastWFX.nAvgBytesPerSec ||
		WFX.nBlockAlign != LastWFX.nBlockAlign ||
		WFX.wBitsPerSample != LastWFX.wBitsPerSample ||
		WFX.cbSize != LastWFX.cbSize)
	{
		//每3秒notify16次
		NotifySize = WFX.nAvgBytesPerSec * 3 / NUM_PLAY_NOTIFICATIONS;
		NotifySize -= NotifySize % WFX.nBlockAlign;
		BufferSize = NotifySize * NUM_PLAY_NOTIFICATIONS;
		
		if(!recreateDSBuffer(g_Engine->getAudioPlayer()->getAudioSetting()))
		{
			ASSERT(false);
			return false;
		}

		Q_memcpy(&LastWFX, sizeof(WAVEFORMATEX), &WFX, sizeof(WAVEFORMATEX));
	}

	reset();

	return true;
}

void CDSSound::unload()
{
	if (isPlaying())
		stop();

	BEGIN_LOCK(&cs);
	if(AudioInput)
		AudioInput->closeFile();
	END_LOCK(&cs);

	reset();
}

bool CDSSound::play(bool loop, FN_SOUND_CALLBACK callback)
{
	bool restored = false;
	if (!pIDSBuffer || !restoreBuffer(restored, 20))
		return false;

    reset();

	setVolume(1.0f);

	BEGIN_LOCK(&cs);

	Callback = callback;
	Loop = loop;

	pIDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

	END_LOCK(&cs);

	return true;
}

void CDSSound::stop()
{
	BEGIN_LOCK(&cs);

	if(pIDSBuffer)
		pIDSBuffer->Stop();

	END_LOCK(&cs);
}

void CDSSound::reset()
{
	BEGIN_LOCK(&cs);

	NextWriteOffset = 0;
	Loop = false;
	FillNextNotificationWithSilence = false;
	SilenceCount = 0;
	Callback = NULL_PTR;

	if(AudioInput)
		AudioInput->reset();

	if (pIDSBuffer)
	{
		pIDSBuffer->Stop();
		pIDSBuffer->SetCurrentPosition(0);

		fillBufferWithSilence();
	}

	END_LOCK(&cs);
}

bool CDSSound::isPlaying() const
{
	if (!pIDSBuffer)
		return false;

	DWORD dwStatus = 0;
	BEGIN_LOCK(&cs);
	pIDSBuffer->GetStatus(&dwStatus);
	END_LOCK(&cs);
	return (dwStatus & DSBSTATUS_PLAYING) != 0;
}

bool CDSSound::isStopped() const
{
	if (!pIDSBuffer)
		return false;

	DWORD dwStatus = 0;
	BEGIN_LOCK(&cs);
	pIDSBuffer->GetStatus(&dwStatus);
	END_LOCK(&cs);
	return (dwStatus & DSBSTATUS_TERMINATED) != 0;
}

bool CDSSound::handleStreamNotification()
{
	ASSERT(AudioInput);

	bool restored = false;
	if (!restoreBuffer(restored, 100))
		return false;
	
	VOID* pDSLockedBuffer = NULL_PTR;
	VOID* pDSLockedBuffer2 = NULL_PTR;
	DWORD dwDSLockedBufferSize;
	DWORD dwDSLockedBufferSize2;

	BEGIN_LOCK(&cs);

	if (restored)
	{
		fillBufferWithSilence();
		END_LOCK(&cs);
		return true;
	}

	if (FAILED( pIDSBuffer->Lock( NextWriteOffset, NotifySize,
		&pDSLockedBuffer, &dwDSLockedBufferSize,
		&pDSLockedBuffer2, &dwDSLockedBufferSize2, 0)))
	{
		END_LOCK(&cs);
		ASSERT(false);
		return false;
	}
	ASSERT(!pDSLockedBuffer2);

	u32 written;
	if (!FillNextNotificationWithSilence)
		written = AudioInput->fillBuffer(pDSLockedBuffer, dwDSLockedBufferSize);
	else
	{
		FillMemory((u8*)pDSLockedBuffer, 
			dwDSLockedBufferSize,
			(u8)(AudioInput->BitsPerSample == 8 ? 128 : 0));
		written = dwDSLockedBufferSize;
		++SilenceCount;
	}

	if (written < dwDSLockedBufferSize)
	{	
		if (!Loop)
		{
			FillMemory((u8*)pDSLockedBuffer + written, 
				dwDSLockedBufferSize - written,
				(u8)(AudioInput->BitsPerSample == 8 ? 128 : 0));

			FillNextNotificationWithSilence = true;
			SilenceCount = 0;
		}
		else
		{
			u32 nRead = written;
			AudioInput->reset();
			while(nRead < dwDSLockedBufferSize )
			{
				u32 n = AudioInput->fillBuffer( (u8*)pDSLockedBuffer + nRead,
					dwDSLockedBufferSize - nRead);

				nRead += n;
			}
		}
	}

	pIDSBuffer->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL_PTR, 0);

	bool stopped = false;
	if (FillNextNotificationWithSilence)
	{
		if(SilenceCount >= NUM_PLAY_NOTIFICATIONS)
		{
			pIDSBuffer->Stop();
			stopped = true;
		}
	}

	NextWriteOffset += dwDSLockedBufferSize;
	NextWriteOffset %= BufferSize;

	END_LOCK(&cs);

	if (stopped && Callback)
		Callback();

	return true;
}

bool CDSSound::restoreBuffer(bool& restored, s32 timeout)
{
	restored = false;

	BEGIN_LOCK(&cs);

	DWORD dwStatus;
	HRESULT hr = pIDSBuffer->GetStatus(&dwStatus);
	if (dwStatus & DSBSTATUS_BUFFERLOST)
	{
		do 
		{
			hr = pIDSBuffer->Restore();
			if (hr == DSERR_BUFFERLOST)
			{
				SLEEP(10);
				timeout -= 10;
				if (timeout < 0)
				{
					END_LOCK(&cs);
					return false;
				}
			}
		} while (hr != DS_OK);
	
		restored = true;
	}

	END_LOCK(&cs);
	return true;
}

void CDSSound::fillBufferWithSilence()
{
	VOID* pDSLockedBuffer = NULL_PTR;
	DWORD dwDSLockedBufferSize;

	if (FAILED( pIDSBuffer->Lock( 0, BufferSize,
		&pDSLockedBuffer, &dwDSLockedBufferSize,
		NULL_PTR, NULL_PTR, 0)))
	{
		ASSERT(false);
		return;
	}

	//填充0
	FillMemory((u8*)pDSLockedBuffer, 
		dwDSLockedBufferSize,
		(u8)(AudioInput->BitsPerSample == 8 ? 128 : 0));

	pIDSBuffer->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL_PTR, 0);
}

bool CDSSound::recreateDSBuffer(const SAudioSetting& setting)
{
	DWORD flag = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY ;

	if (setting.hardware)
		flag |= DSBCAPS_STATIC;
	else
		flag |= DSBCAPS_LOCSOFTWARE;
		
	if (setting.global)
		flag |= DSBCAPS_GLOBALFOCUS;

	DSBUFFERDESC desc = {0};
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags =  flag;
	desc.dwBufferBytes = BufferSize;
	desc.guid3DAlgorithm = GUID_NULL;
	desc.lpwfxFormat = &WFX;

	BEGIN_LOCK(&cs);

	SAFE_RELEASE_STRICT(pIDSBuffer);
	SAFE_RELEASE_STRICT(pIDNotify);
	
	if (FAILED(pIDSound->CreateSoundBuffer(&desc, &pIDSBuffer, NULL_PTR)))
	{
		END_LOCK(&cs);
		ASSERT(false);
		return false;
	}

	if (FAILED(pIDSBuffer->QueryInterface(IID_IDirectSoundNotify, (VOID**)&pIDNotify)))
	{
		END_LOCK(&cs);
		ASSERT(false);
		return false;
	}

	for (u32 i=0; i<NUM_PLAY_NOTIFICATIONS; ++i)
	{
		PosNotifyArray[i].dwOffset = NotifySize * i + (NotifySize -1);
		PosNotifyArray[i].hEventNotify = NotificationEvent.handle;
	}

	if (FAILED(pIDNotify->SetNotificationPositions(NUM_PLAY_NOTIFICATIONS, PosNotifyArray)))
	{
		END_LOCK(&cs);
		ASSERT(false);
		return false;
	}

	SAFE_RELEASE_STRICT(pIDNotify);		//来自queryInterface

	LONG vol;
	if ( SUCCEEDED(pIDSBuffer->GetVolume(&vol)) )
		Volume = (f32)(vol - DSBVOLUME_MIN) / (f32)(DSBVOLUME_MAX - DSBVOLUME_MIN);

	END_LOCK(&cs);

	return true;
}

void CDSSound::setVolume(f32 vol)
{
	if (!pIDSBuffer)
		return;

	f32 f = clamp_<f32>(vol, 0.0f, 1.0f);

	LONG val = (LONG)(DSBVOLUME_MIN * (1-f) + DSBVOLUME_MAX * f);
	
	if (FAILED(pIDSBuffer->SetVolume(val)))
		return;

	Volume = vol;
}

#endif

#endif