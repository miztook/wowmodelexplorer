#pragma once

#include "IAudioInput.h"

#ifdef MW_PLATFORM_WINDOWS
//libmad
#include "stream.h"
#include "frame.h"
#include "synth.h"

# define SAMPLE_DEPTH 16

struct xing
{
	long flags;
	unsigned long frames;
	unsigned long bytes;
	unsigned char toc[100];
	long scale;
};

enum
{
	XING_FRAMES = 0x00000001L,
	XING_BYTES  = 0x00000002L,
	XING_TOC    = 0x00000004L,
	XING_SCALE  = 0x00000008L
};

# define XING_MAGIC (('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')

struct dither
{
	mad_fixed_t error[3];
	mad_fixed_t random;
};

typedef struct _ID3Tag
{
	char m_cTAG[3];  // Must equal "TAG"
	char m_cSong[30];
	char m_cArtist[30];
	char m_cAlbum[30];
	char m_cYear[4];
	char m_cComment[30];
	unsigned char m_cGenre;
} ID3Tag;

typedef struct _ID3v2Tag
{
	char m_cTAG[3];  // Must equal ID3
	unsigned char m_cVersion[2]; // Major / Minor
	unsigned char m_cFlags;
	unsigned char m_cSize_Encoded[4];
} ID3v2Tag;

typedef struct _ID3v2Frame
{
	char m_cFrameID[4];
	unsigned char m_cSize_Encoded[4];
	unsigned short m_cFlags;
} ID3v2Frame;

class IReadFile;
class IMemFile;

class CMP3Input : public IAudioInput
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMP3Input);

public:
	CMP3Input();
	~CMP3Input();

public:
	virtual bool reset();

	virtual bool openFile(const c8* filename);
	virtual void closeFile();

	virtual u32 fillBuffer(void* buffer, u32 size);

private:
	int parse_xing(struct xing *xing, struct mad_bitptr ptr, unsigned int bitlen);
	int scan_header(struct mad_header *header, struct xing *xing);
	void pack_pcm(unsigned char **pcm, unsigned int nsamples, mad_fixed_t const *ch1, mad_fixed_t const *ch2);

	//32-bit pseudo-random number generator
	static unsigned long prng(unsigned long state);

	//dither and scale sample
	int dither_scale(mad_fixed_t sample, struct dither *dither);

private:

#ifdef MPQ_AUDIOINPUT
	IMemFile*		File;
#else
	IReadFile*		File;
#endif

	mad_stream		m_stream;
	mad_frame	m_frame;
	mad_synth		m_synth;
	xing		m_xing;

	mad_timer_t m_timer;
	mad_timer_t m_length;

	u8*		m_buffer;
	u32		m_samplecount;
	u32		m_bufferlen;

	u32		StreamStart;
};

inline unsigned long CMP3Input::prng( unsigned long state )
{
	return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

inline int CMP3Input::dither_scale( mad_fixed_t sample, struct dither *dither )
{
	unsigned int scalebits;
	mad_fixed_t output, mask, random;

	enum
	{
		MIN = -MAD_F_ONE,
		MAX =  MAD_F_ONE - 1
	};

	/* noise shape */
	sample += dither->error[0] - dither->error[1] + dither->error[2];
	dither->error[2] = dither->error[1];
	dither->error[1] = dither->error[0] / 2;

	/* bias */
	output = sample + (1L << (MAD_F_FRACBITS + 1 - SAMPLE_DEPTH - 1));
	scalebits = MAD_F_FRACBITS + 1 - SAMPLE_DEPTH;
	mask = (1L << scalebits) - 1;

	/* dither */
	random  = prng(dither->random);
	output += (random & mask) - (dither->random & mask);
	dither->random = random;

	/* clip */

	if (output > MAX)
	{
		output = MAX;
		if (sample > MAX)
			sample = MAX;
	}

	else if (output < MIN)
	{
		output = MIN;
		if (sample < MIN)
			sample = MIN;
	}

	/* quantize */
	output &= ~mask;

	/* error feedback */
	dither->error[0] = sample - output;

	/* scale */
	return output >> scalebits;
}


#endif