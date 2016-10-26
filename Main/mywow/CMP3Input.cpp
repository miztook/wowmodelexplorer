#include "stdafx.h"
#include "CMP3Input.h"
#include "mywow.h"

#ifdef MW_PLATFORM_WINDOWS

#define FILE_BUFFER_SIZE		20 * 1024		//缓存文件大小

CMP3Input::CMP3Input()
{
	File = NULL_PTR;

	m_xing.flags = 0;
	m_timer = mad_timer_zero;
	m_length = mad_timer_zero;

	m_buffer = new u8[FILE_BUFFER_SIZE];
	m_samplecount = 0;
	m_bufferlen = 0;

	memset(&m_stream, 0, sizeof(m_stream));
	memset(&m_frame, 0, sizeof(m_frame));
	memset(&m_synth, 0, sizeof(m_synth));

	StreamStart = 0;
}

CMP3Input::~CMP3Input()
{
	closeFile();

	delete[] m_buffer;
}

bool CMP3Input::reset()
{
	if (!File)
		return false;

	File->seek(0, false);
	m_bufferlen = File->read(m_buffer, FILE_BUFFER_SIZE);
	
	mad_stream_buffer(&m_stream, m_buffer, m_bufferlen);
	mad_frame_mute(&m_frame);
	mad_synth_mute(&m_synth);

	m_synth.pcm.length = 0;
	m_samplecount = 0;
	return true;
}

bool CMP3Input::openFile( const c8* filename )
{
	if (File)
		return true;

	c8 ext[10] = {0};
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "mp3") != 0)
		return false;

#ifdef MPQ_AUDIOINPUT
	File = g_Engine->getWowEnvironment()->openFile(filename, false);
#else
	const c8* base = g_Engine->getFileSystem()->getDataDirectory();
	c8 tmp[QMAX_PATH];
	Q_strcpy(tmp, QMAX_PATH, base);
	Q_strcat(tmp, QMAX_PATH, filename);

	File = g_Engine->getFileSystem()->createAndOpenFile(tmp, true);
#endif

	if (!File || File->getSize() < 128)
		return false;

	StreamStart = 0;

	DataSize = File->getSize();

	//ID3v2
	ID3v2Tag tag;
	memset(&tag, 0, sizeof(ID3v2Tag));
	File->read(&tag, sizeof(ID3v2Tag));
	if (memcmp(tag.m_cTAG, "ID3", 3) == 0)
	{
		StreamStart = sizeof(ID3v2Tag);
		StreamStart += (tag.m_cSize_Encoded[0] << 21) | (tag.m_cSize_Encoded[1] << 14)
			| (tag.m_cSize_Encoded[2] << 7) | tag.m_cSize_Encoded[3];

		DataSize -= StreamStart;
		ASSERT(DataSize > 128);
	}

	File->seek(File->getSize() - 128, false);
	ID3Tag tag1;
	memset(&tag1, 0, sizeof(ID3Tag));
	File->read(&tag1, sizeof(ID3Tag));

	if (memcmp(tag1.m_cTAG, "TAG", 3) == 0)
	{
		DataSize -= 128;
	}

	mad_stream_init(&m_stream);
	mad_frame_init(&m_frame);
	mad_synth_init(&m_synth);

	File->seek(StreamStart, false);
	scan_header(&m_frame.header, &m_xing);
	File->seek(StreamStart, false);

	m_synth.pcm.length = 0;
	m_samplecount = 0;
	m_timer = mad_timer_zero;

	if (m_xing.flags & XING_FRAMES)
	{
		m_length = m_frame.header.duration;
		mad_timer_multiply(&m_length, m_xing.frames);
	}
	else
	{
		/* estimate playing time from file size */
		mad_timer_set(&m_length, 0, 1, m_frame.header.bitrate / 8);
		mad_timer_multiply(&m_length, File->getSize());
	}

	SampleRate = m_frame.header.samplerate;
	NumChannels = (m_frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1: 2;
	BitsPerSample = SAMPLE_DEPTH;
	m_bufferlen = 0;

	return true;
}

void CMP3Input::closeFile()
{
	if (File)
	{
		mad_synth_finish(&m_synth);
		mad_frame_finish(&m_frame);
		mad_stream_finish(&m_stream);		

		delete File;
		File = NULL_PTR;
	}
}

u32 CMP3Input::fillBuffer( void* buffer, u32 size )
{
	if (!File)
		return 0;

	u32 current = (u32)File->getPos();
	if (current >= StreamStart + DataSize)
		return 0;

	if (size > StreamStart + DataSize - current)		//不要读取非data部分
	{
		size = StreamStart + DataSize - current;
	}

	u8* samples = (u8*)buffer;
	u32 nsamples = size / getSampleSize();

	while(nsamples)
	{
		ASSERT(m_synth.pcm.length >= m_samplecount);
		u32 count = m_synth.pcm.length - m_samplecount;
		if (count > nsamples)
			count = nsamples;

		if (count)
		{
			mad_fixed_t const *ch1, *ch2;
			ch1 = m_synth.pcm.samples[0] + m_samplecount;
			ch2 = m_synth.pcm.samples[1] + m_samplecount;

			if (NumChannels != 2)
				ch2 = 0;
			else if (m_synth.pcm.channels == 1)
				ch2 = ch1;

			pack_pcm(&samples, count, ch1, ch2);

			m_samplecount += count;
			nsamples -= count;

			if (nsamples == 0)
				break;
		}

		//文件buffer被读完, 重新读取文件buffer,设置stream参数
		while(mad_frame_decode(&m_frame, &m_stream) == -1)
		{
			if (MAD_RECOVERABLE(m_stream.error))
				continue;

			if (m_stream.next_frame)
			{
				m_bufferlen = (u32)(m_buffer + m_bufferlen - m_stream.next_frame);
				memmove(m_buffer, m_stream.next_frame, m_bufferlen);
			}

			ASSERT(FILE_BUFFER_SIZE > m_bufferlen);

			u32 bytes = File->read(m_buffer + m_bufferlen, FILE_BUFFER_SIZE - m_bufferlen);
			if (bytes == 0)
				return 0;			//文件读完
			
			m_bufferlen += bytes;
			mad_stream_buffer(&m_stream, m_buffer, m_bufferlen);
		}

		m_samplecount = 0;
		mad_synth_frame(&m_synth, &m_frame);
		mad_timer_add(&m_timer, m_frame.header.duration);
	}

	return (u32)(samples - (u8*)buffer);
}

int CMP3Input::parse_xing( struct xing *xing, struct mad_bitptr ptr, unsigned int bitlen )
{
	if (bitlen < 64 || mad_bit_read(&ptr, 32) != XING_MAGIC)
		goto fail;

	xing->flags = mad_bit_read(&ptr, 32);
	bitlen -= 64;

	if (xing->flags & XING_FRAMES)
	{
		if (bitlen < 32)
			goto fail;

		xing->frames = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	if (xing->flags & XING_BYTES)
	{
		if (bitlen < 32)
			goto fail;

		xing->bytes = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	if (xing->flags & XING_TOC)
	{
		int i;

		if (bitlen < 800)
			goto fail;

		for (i = 0; i < 100; ++i)
			xing->toc[i] = (unsigned char) mad_bit_read(&ptr, 8);

		bitlen -= 800;
	}

	if (xing->flags & XING_SCALE)
	{
		if (bitlen < 32)
			goto fail;

		xing->scale = mad_bit_read(&ptr, 32);
	}

	return 0;

fail:
	xing->flags = 0;
	return -1;
}

int CMP3Input::scan_header( struct mad_header *header, struct xing *xing )
{
	struct mad_stream stream;
	struct mad_frame frame;
	u8* buffer = (u8*)Z_AllocateTempMemory(8192);
	unsigned int buflen = 0;
	int count = 0, result = 0;

	mad_stream_init(&stream);
	mad_frame_init(&frame);

	if (xing)
		xing->flags = 0;

	while (1)
	{
		if (buflen < 8192)
		{
			unsigned int bytes;
			if ((bytes = File->read(buffer + buflen, 8192 - buflen)) == 0)
			{
				result = -1;
				break;
			}

			buflen += bytes;
		}

		mad_stream_buffer(&stream, buffer, buflen);

		while (1)
		{
			if (mad_frame_decode(&frame, &stream) == -1)
			{
				if (!MAD_RECOVERABLE(stream.error))
					break;

				continue;
			}

			if (count++ || (xing && parse_xing(xing, stream.anc_ptr, stream.anc_bitlen) == 0))
				break;
		}

		if (count || stream.error != MAD_ERROR_BUFLEN)
			break;

		memmove(buffer, stream.next_frame,
			buflen = (unsigned int)(&buffer[buflen] - stream.next_frame));
	}

	if (count)
	{
		if (header)
			*header = frame.header;
	}

	else
		result = -1;

	mad_frame_finish(&frame);
	mad_stream_finish(&stream);

	Z_FreeTempMemory(buffer);

	return result;
}

void CMP3Input::pack_pcm( unsigned char **pcm, unsigned int nsamples, mad_fixed_t const *ch1, mad_fixed_t const *ch2 )
{
	register signed int s0, s1;
	static struct dither d0, d1;

	if (ch2)    /* stereo */
	{
		while (nsamples--)
		{
			s0 = dither_scale(*ch1++, &d0);
			s1 = dither_scale(*ch2++, &d1);
# if SAMPLE_DEPTH == 16
			(*pcm)[0 + 0] = s0 >> 0;
			(*pcm)[0 + 1] = s0 >> 8;
			(*pcm)[2 + 0] = s1 >> 0;
			(*pcm)[2 + 1] = s1 >> 8;
			*pcm += 2 * 2;
# elif SAMPLE_DEPTH == 8
			(*pcm)[0] = s0 ^ 0x80;
			(*pcm)[1] = s1 ^ 0x80;

			*pcm += 2;
# else
#  error "bad SAMPLE_DEPTH"
# endif
		}
	}

	else    /* mono */
	{
		while (nsamples--)
		{
			s0 = dither_scale(*ch1++, &d0);

# if SAMPLE_DEPTH == 16
			(*pcm)[0] = s0 >> 0;
			(*pcm)[1] = s0 >> 8;
			*pcm += 2;
# elif SAMPLE_DEPTH == 8
			*(*pcm)++ = s0 ^ 0x80;
# endif
		}
	}
}

#endif
