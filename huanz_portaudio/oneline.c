/** @file oneline.c, adapted from patest_mono.c
	@brief Play a monophonic sine wave using the Portable Audio api for several seconds.
	@author Phil Burk  http://www.softsynth.com
 Citation: for the wonderful one-line musics, some of them credit to this forum: https://www.reddit.com/r/programming/comments/kyj77/algorithmic_symphonies_from_one_line_of_code_how/
*/
/*
 * $Id$
 *
 * Authors:
 *    Ross Bencina <rossb@audiomulch.com>
 *    Phil Burk <philburk@softsynth.com>
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include <stdio.h>
#include <math.h>
#include "portaudio.h"

#define NUM_SECONDS   (10)
#define SAMPLE_RATE   (8000)
#define AMPLITUDE     (0.8)
#define FRAMES_PER_BUFFER  (64)
#define OUTPUT_DEVICE Pa_GetDefaultOutputDevice()

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define TABLE_SIZE   (64)
typedef struct
{
    float sine[TABLE_SIZE];
    int phase;
}
paTestData;

// global variable t, incremented in the frames
int t = 0;
// global variable for the frames
long sample;
// global variable for which line, and volume, and reverse
int l = 1;
float v = 1.0;
int r = 1;


long line_0 (int t) { return (t * (((t>>12) | (t>>8)) & (63 & (t>>4)))); }
long line_1 (int t) { return (t * ((t>>12 | t>>3) & 13131 & t>>11)); }
long line_2 (int t) { return ((440 * 256/8000 * t) & (1 << 7)); }
long line_3 (int t) { return (((t^0xf3c35)%64)*((t/1537)&17)*sin(t/0x2500)+cos(t/0x2500)*(((t/3)^1984)%32)*((t/90)&13));}
long line_4 (int t) { return (((t+1<<(t>>t-(t*01231)>>43))/5)/(4.240101)-(t*0.00024)); }
long line_5 (int t) { return sin(t >> 2 | t << 8) * 50 * ((t>>2) & 3); }


void initGlobals()
{
    sample = line_1 (t);
}


/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData)
{
    float *out = (float*)outputBuffer;
    unsigned long i;
    int finished = 0;
//    int t = GLOBAL_T;
    /* avoid unused variable warnings */
    (void) inputBuffer;
    (void) timeInfo;
    (void) statusFlags;
    
    
    
    for( i=0; i<framesPerBuffer; i++ )
    {
        if (l == 1) {
            sample = line_1(t);
        }else if (l == 2){
            sample = line_2(t);
        }else{
            sample = line_0(t);
        }
        
        //    sample = line_1(t);
        sample &= 0xFF; // take only 8 bits
        // convert to float and offset so 128 becomes 0.0:
        float fsample = sample * (1.0F / 256.0F) - 0.5F;
        fsample *= v;
        
        *out++ = fsample;
//        data->sine[data->phase];  /* left */
//        data->phase += 1;
//        if( data->phase >= TABLE_SIZE ) data->phase -= TABLE_SIZE;
        t += r;
    }
    return finished;
}

/*******************************************************************/
int main(int argc, char** argv);
int main(int argc, char** argv)
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    paTestData data;
    int i;
    printf("PortAudio Test: output MONO sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
    
    initGlobals();
    sample = line_1 (t);
    
    /* initialise sinusoidal wavetable */
    for( i=0; i<TABLE_SIZE; i++ )
    {
        data.sine[i] = (float) (AMPLITUDE * sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. ));
    }
    data.phase = 0;
    
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = OUTPUT_DEVICE;
    outputParameters.channelCount = 1;       /* MONO output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              patestCallback,
              &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    
    printf("Play for %d seconds.\n", NUM_SECONDS ); fflush(stdout);
    char init;
    while (init != 'q') {
        printf("Controlling functions: \n" \
               "press q to quit; \n" \
               "press v to change volume \n" \
               "press l to change a line \n" \
               "press r to change reverse \n");
        scanf("%c", &init);
        switch (init) {
            case 'v':
                printf("enter float volume (from 0 to 1), default volume is 0.5 \n");
                scanf("%f", &v);
            case 'l':
                printf("enter a line number (0, 1, 2, 3, 4, 5) \n");
                scanf("%d", &l);
            case 'r':
                printf("we just reversed. \n");
                r *= (-1);
            default:
                break;
        }
    }
    
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    
    Pa_Terminate();
    
    printf("Test finished.\n");
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}

