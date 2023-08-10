#include "OpenAL.h"
#include <AL/alc.h>

#include <string>

/* Some helper functions to get the name from the format enums. */
const char *FormatName(ALenum type);

/* Easy device init/deinit functions. InitAL returns 0 on success. */
int InitAL(char ***argv, int *argc);
void CloseAL(void);

/* Cross-platform timeget and sleep functions. */
int altime_get(void);
void al_nssleep(unsigned long nsec);

/* C doesn't allow casting between function and non-function pointer types, so
 * with C99 we need to use a union to reinterpret the pointer type. Pre-C99
 * still needs to use a normal cast and live with the warning (C++ is fine with
 * a regular reinterpret_cast).
 */
#if __STDC_VERSION__ >= 199901L
#define FUNCTION_CAST(T, ptr) (union{void *p; T f;}){ptr}.f
#elif defined(__cplusplus)
#define FUNCTION_CAST(T, ptr) reinterpret_cast<T>(ptr)
#else
#define FUNCTION_CAST(T, ptr) (T)(ptr)
#endif

/* InitAL opens a device and sets up a context using default attributes, making
 * the program ready to call OpenAL functions. */
int InitAL(char ***argv, int *argc)
{
    const ALCchar *name;
    ALCdevice *device;
    ALCcontext *ctx;

    /* Open and initialize a device */
    device = nullptr;
    if(argc && argv && *argc > 1 && strcmp((*argv)[0], "-device") == 0)
    {
        device = alcOpenDevice((*argv)[1]);
        if(!device)
            fprintf(stderr, "Failed to open \"%s\", trying default\n", (*argv)[1]);
        (*argv) += 2;
        (*argc) -= 2;
    }
    if(!device)
        device = alcOpenDevice(nullptr);
    if(!device)
    {
        fprintf(stderr, "Could not open a device!\n");
        return 1;
    }

    ctx = alcCreateContext(device, nullptr);
    if(ctx == nullptr || alcMakeContextCurrent(ctx) == ALC_FALSE)
    {
        if(ctx != nullptr)
            alcDestroyContext(ctx);
        alcCloseDevice(device);
        fprintf(stderr, "Could not set a context!\n");
        return 1;
    }

    name = nullptr;
    if(alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
    if(!name || alcGetError(device) != AL_NO_ERROR)
        name = alcGetString(device, ALC_DEVICE_SPECIFIER);
    printf("Opened \"%s\"\n", name);

    return 0;
}

/* CloseAL closes the device belonging to the current context, and destroys the
 * context. */
void CloseAL(void)
{
    ALCdevice *device;
    ALCcontext *ctx;

    ctx = alcGetCurrentContext();
    if(ctx == nullptr)
        return;

    device = alcGetContextsDevice(ctx);

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(ctx);
    alcCloseDevice(device);
}


const char *FormatName(ALenum format)
{
    switch(format)
    {
    case AL_FORMAT_MONO8: return "Mono, U8";
    case AL_FORMAT_MONO16: return "Mono, S16";
    case AL_FORMAT_MONO_FLOAT32: return "Mono, Float32";
    case AL_FORMAT_MONO_MULAW: return "Mono, muLaw";
    case AL_FORMAT_MONO_ALAW_EXT: return "Mono, aLaw";
    case AL_FORMAT_MONO_IMA4: return "Mono, IMA4 ADPCM";
    case AL_FORMAT_MONO_MSADPCM_SOFT: return "Mono, MS ADPCM";
    case AL_FORMAT_STEREO8: return "Stereo, U8";
    case AL_FORMAT_STEREO16: return "Stereo, S16";
    case AL_FORMAT_STEREO_FLOAT32: return "Stereo, Float32";
    case AL_FORMAT_STEREO_MULAW: return "Stereo, muLaw";
    case AL_FORMAT_STEREO_ALAW_EXT: return "Stereo, aLaw";
    case AL_FORMAT_STEREO_IMA4: return "Stereo, IMA4 ADPCM";
    case AL_FORMAT_STEREO_MSADPCM_SOFT: return "Stereo, MS ADPCM";
    case AL_FORMAT_QUAD8: return "Quadraphonic, U8";
    case AL_FORMAT_QUAD16: return "Quadraphonic, S16";
    case AL_FORMAT_QUAD32: return "Quadraphonic, Float32";
    case AL_FORMAT_QUAD_MULAW: return "Quadraphonic, muLaw";
    case AL_FORMAT_51CHN8: return "5.1 Surround, U8";
    case AL_FORMAT_51CHN16: return "5.1 Surround, S16";
    case AL_FORMAT_51CHN32: return "5.1 Surround, Float32";
    case AL_FORMAT_51CHN_MULAW: return "5.1 Surround, muLaw";
    case AL_FORMAT_61CHN8: return "6.1 Surround, U8";
    case AL_FORMAT_61CHN16: return "6.1 Surround, S16";
    case AL_FORMAT_61CHN32: return "6.1 Surround, Float32";
    case AL_FORMAT_61CHN_MULAW: return "6.1 Surround, muLaw";
    case AL_FORMAT_71CHN8: return "7.1 Surround, U8";
    case AL_FORMAT_71CHN16: return "7.1 Surround, S16";
    case AL_FORMAT_71CHN32: return "7.1 Surround, Float32";
    case AL_FORMAT_71CHN_MULAW: return "7.1 Surround, muLaw";
    case AL_FORMAT_BFORMAT2D_8: return "B-Format 2D, U8";
    case AL_FORMAT_BFORMAT2D_16: return "B-Format 2D, S16";
    case AL_FORMAT_BFORMAT2D_FLOAT32: return "B-Format 2D, Float32";
    case AL_FORMAT_BFORMAT2D_MULAW: return "B-Format 2D, muLaw";
    case AL_FORMAT_BFORMAT3D_8: return "B-Format 3D, U8";
    case AL_FORMAT_BFORMAT3D_16: return "B-Format 3D, S16";
    case AL_FORMAT_BFORMAT3D_FLOAT32: return "B-Format 3D, Float32";
    case AL_FORMAT_BFORMAT3D_MULAW: return "B-Format 3D, muLaw";
    case AL_FORMAT_UHJ2CHN8_SOFT: return "UHJ 2-channel, U8";
    case AL_FORMAT_UHJ2CHN16_SOFT: return "UHJ 2-channel, S16";
    case AL_FORMAT_UHJ2CHN_FLOAT32_SOFT: return "UHJ 2-channel, Float32";
    case AL_FORMAT_UHJ3CHN8_SOFT: return "UHJ 3-channel, U8";
    case AL_FORMAT_UHJ3CHN16_SOFT: return "UHJ 3-channel, S16";
    case AL_FORMAT_UHJ3CHN_FLOAT32_SOFT: return "UHJ 3-channel, Float32";
    case AL_FORMAT_UHJ4CHN8_SOFT: return "UHJ 4-channel, U8";
    case AL_FORMAT_UHJ4CHN16_SOFT: return "UHJ 4-channel, S16";
    case AL_FORMAT_UHJ4CHN_FLOAT32_SOFT: return "UHJ 4-channel, Float32";
    }
    return "Unknown Format";
}


#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

int altime_get(void)
{
    static int start_time = 0;
    int cur_time;
    union {
        FILETIME ftime;
        ULARGE_INTEGER ulint;
    } systime;
    GetSystemTimeAsFileTime(&systime.ftime);
    /* FILETIME is in 100-nanosecond units, or 1/10th of a microsecond. */
    cur_time = (int)(systime.ulint.QuadPart/10000);

    if(!start_time)
        start_time = cur_time;
    return cur_time - start_time;
}

void al_nssleep(unsigned long nsec)
{
    Sleep(nsec / 1000000);
}

#else

#include <sys/time.h>
#include <unistd.h>
#include <time.h>

int altime_get(void)
{
    static int start_time = 0u;
    int cur_time;

#if _POSIX_TIMERS > 0
    struct timespec ts;
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
    if(ret != 0) return 0;
    cur_time = (int)(ts.tv_sec*1000 + ts.tv_nsec/1000000);
#else /* _POSIX_TIMERS > 0 */
    struct timeval tv;
    int ret = gettimeofday(&tv, nullptr);
    if(ret != 0) return 0;
    cur_time = (int)(tv.tv_sec*1000 + tv.tv_usec/1000);
#endif

    if(!start_time)
        start_time = cur_time;
    return cur_time - start_time;
}

void al_nssleep(unsigned long nsec)
{
    struct timespec ts, rem;
    ts.tv_sec = (time_t)(nsec / 1000000000ul);
    ts.tv_nsec = (long)(nsec % 1000000000ul);
    while(nanosleep(&ts, &rem) == -1 && errno == EINTR)
        ts = rem;
}

#endif

enum FormatType {
    Int16,
    Float,
    IMA4,
    MSADPCM
};

/* LoadBuffer loads the named audio file into an OpenAL buffer object, and
 * returns the new buffer ID.
 */
static ALuint LoadSound(const char *filename)
{
    enum FormatType sample_format = Int16;
    ALint byteblockalign = 0;
    ALint splblockalign = 0;
    sf_count_t num_frames;
    ALenum err, format;
    ALsizei num_bytes;
    SNDFILE *sndfile;
    SF_INFO sfinfo;
    ALuint buffer;
    void *membuf;

    /* Open the audio file and check that it's usable. */
    sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if(!sndfile)
    {
        fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndfile));
        return 0;
    }
    if(sfinfo.frames < 1)
    {
        fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
        sf_close(sndfile);
        return 0;
    }

    /* Detect a suitable format to load. Formats like Vorbis and Opus use float
     * natively, so load as float to avoid clipping when possible. Formats
     * larger than 16-bit can also use float to preserve a bit more precision.
     */
    switch((sfinfo.format&SF_FORMAT_SUBMASK))
    {
    case SF_FORMAT_PCM_24:
    case SF_FORMAT_PCM_32:
    case SF_FORMAT_FLOAT:
    case SF_FORMAT_DOUBLE:
    case SF_FORMAT_VORBIS:
    case SF_FORMAT_OPUS:
    case SF_FORMAT_ALAC_20:
    case SF_FORMAT_ALAC_24:
    case SF_FORMAT_ALAC_32:
    case 0x0080/*SF_FORMAT_MPEG_LAYER_I*/:
    case 0x0081/*SF_FORMAT_MPEG_LAYER_II*/:
    case 0x0082/*SF_FORMAT_MPEG_LAYER_III*/:
        if(alIsExtensionPresent("AL_EXT_FLOAT32"))
            sample_format = Float;
        break;
    case SF_FORMAT_IMA_ADPCM:
        /* ADPCM formats require setting a block alignment as specified in the
         * file, which needs to be read from the wave 'fmt ' chunk manually
         * since libsndfile doesn't provide it in a format-agnostic way.
         */
        if(sfinfo.channels <= 2 && (sfinfo.format&SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
             && alIsExtensionPresent("AL_EXT_IMA4")
             && alIsExtensionPresent("AL_SOFT_block_alignment"))
            sample_format = IMA4;
        break;
    case SF_FORMAT_MS_ADPCM:
        if(sfinfo.channels <= 2 && (sfinfo.format&SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
             && alIsExtensionPresent("AL_SOFT_MSADPCM")
             && alIsExtensionPresent("AL_SOFT_block_alignment"))
            sample_format = MSADPCM;
        break;
    }

    if(sample_format == IMA4 || sample_format == MSADPCM)
    {
        /* For ADPCM, lookup the wave file's "fmt " chunk, which is a
         * WAVEFORMATEX-based structure for the audio format.
         */
        SF_CHUNK_INFO inf = { "fmt ", 4, 0, nullptr };
        SF_CHUNK_ITERATOR *iter = sf_get_chunk_iterator(sndfile, &inf);

        /* If there's an issue getting the chunk or block alignment, load as
         * 16-bit and have libsndfile do the conversion.
         */
        if(!iter || sf_get_chunk_size(iter, &inf) != SF_ERR_NO_ERROR || inf.datalen < 14)
            sample_format = Int16;
        else
        {
            ALubyte *fmtbuf = calloc(inf.datalen, 1);
            inf.data = fmtbuf;
            if(sf_get_chunk_data(iter, &inf) != SF_ERR_NO_ERROR)
                sample_format = Int16;
            else
            {
                /* Read the nBlockAlign field, and convert from bytes- to
                 * samples-per-block (verifying it's valid by converting back
                 * and comparing to the original value).
                 */
                byteblockalign = fmtbuf[12] | (fmtbuf[13]<<8);
                if(sample_format == IMA4)
                {
                    splblockalign = (byteblockalign/sfinfo.channels - 4)/4*8 + 1;
                    if(splblockalign < 1
                         || ((splblockalign-1)/2 + 4)*sfinfo.channels != byteblockalign)
                        sample_format = Int16;
                }
                else
                {
                    splblockalign = (byteblockalign/sfinfo.channels - 7)*2 + 2;
                    if(splblockalign < 2
                         || ((splblockalign-2)/2 + 7)*sfinfo.channels != byteblockalign)
                        sample_format = Int16;
                }
            }
            free(fmtbuf);
        }
    }

    if(sample_format == Int16)
    {
        splblockalign = 1;
        byteblockalign = sfinfo.channels * 2;
    }
    else if(sample_format == Float)
    {
        splblockalign = 1;
        byteblockalign = sfinfo.channels * 4;
    }

    /* Figure out the OpenAL format from the file and desired sample type. */
    format = AL_NONE;
    if(sfinfo.channels == 1)
    {
        if(sample_format == Int16)
            format = AL_FORMAT_MONO16;
        else if(sample_format == Float)
            format = AL_FORMAT_MONO_FLOAT32;
        else if(sample_format == IMA4)
            format = AL_FORMAT_MONO_IMA4;
        else if(sample_format == MSADPCM)
            format = AL_FORMAT_MONO_MSADPCM_SOFT;
    }
    else if(sfinfo.channels == 2)
    {
        if(sample_format == Int16)
            format = AL_FORMAT_STEREO16;
        else if(sample_format == Float)
            format = AL_FORMAT_STEREO_FLOAT32;
        else if(sample_format == IMA4)
            format = AL_FORMAT_STEREO_IMA4;
        else if(sample_format == MSADPCM)
            format = AL_FORMAT_STEREO_MSADPCM_SOFT;
    }
    else if(sfinfo.channels == 3)
    {
        if(sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT)
        {
            if(sample_format == Int16)
                format = AL_FORMAT_BFORMAT2D_16;
            else if(sample_format == Float)
                format = AL_FORMAT_BFORMAT2D_FLOAT32;
        }
    }
    else if(sfinfo.channels == 4)
    {
        if(sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT)
        {
            if(sample_format == Int16)
                format = AL_FORMAT_BFORMAT3D_16;
            else if(sample_format == Float)
                format = AL_FORMAT_BFORMAT3D_FLOAT32;
        }
    }
    if(!format)
    {
        fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
        sf_close(sndfile);
        return 0;
    }

    if(sfinfo.frames/splblockalign > (sf_count_t)(INT_MAX/byteblockalign))
    {
        fprintf(stderr, "Too many samples in %s (%" PRId64 ")\n", filename, sfinfo.frames);
        sf_close(sndfile);
        return 0;
    }

    /* Decode the whole audio file to a buffer. */
    membuf = malloc((size_t)(sfinfo.frames / splblockalign * byteblockalign));

    if(sample_format == Int16)
        num_frames = sf_readf_short(sndfile, membuf, sfinfo.frames);
    else if(sample_format == Float)
        num_frames = sf_readf_float(sndfile, membuf, sfinfo.frames);
    else
    {
        sf_count_t count = sfinfo.frames / splblockalign * byteblockalign;
        num_frames = sf_read_raw(sndfile, membuf, count);
        if(num_frames > 0)
            num_frames = num_frames / byteblockalign * splblockalign;
    }
    if(num_frames < 1)
    {
        free(membuf);
        sf_close(sndfile);
        fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
        return 0;
    }
    num_bytes = (ALsizei)(num_frames / splblockalign * byteblockalign);

    printf("Loading: %s (%s, %dhz)\n", filename, FormatName(format), sfinfo.samplerate);
    fflush(stdout);

    /* Buffer the audio data into a new buffer object, then free the data and
     * close the file.
     */
    buffer = 0;
    alGenBuffers(1, &buffer);
    if(splblockalign > 1)
        alBufferi(buffer, AL_UNPACK_BLOCK_ALIGNMENT_SOFT, splblockalign);
    alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

    free(membuf);
    sf_close(sndfile);

    /* Check if an error occurred, and clean up if so. */
    err = alGetError();
    if(err != AL_NO_ERROR)
    {
        fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
        if(buffer && alIsBuffer(buffer))
            alDeleteBuffers(1, &buffer);
        return 0;
    }

    return buffer;
}


int main(int argc, char **argv)
{
    ALuint source, buffer;
    ALfloat offset;
    ALenum state;

    /* Print out usage if no arguments were specified */
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s [-device <name>] <filename>\n", argv[0]);
        return 1;
    }

    /* Initialize OpenAL. */
    argv++; argc--;
    if(InitAL(&argv, &argc) != 0)
        return 1;

    /* Load the sound into a buffer. */
    buffer = LoadSound(argv[0]);
    if(!buffer)
    {
        CloseAL();
        return 1;
    }

    /* Create the source to play the sound with. */
    source = 0;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, (ALint)buffer);
    assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source");

    /* Play the sound until it finishes. */
    alSourcePlay(source);
    do {
        al_nssleep(10000000);
        alGetSourcei(source, AL_SOURCE_STATE, &state);

        /* Get the source offset. */
        alGetSourcef(source, AL_SEC_OFFSET, &offset);
        printf("\rOffset: %f  ", offset);
        fflush(stdout);
    } while(alGetError() == AL_NO_ERROR && state == AL_PLAYING);
    printf("\n");

    /* All done. Delete resources, and close down OpenAL. */
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    CloseAL();

    return 0;
}