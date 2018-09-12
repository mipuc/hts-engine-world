/*
 * synthworld.cpp
 *
 *  Created on: 06.08.2018
 *      Author: Michael Pucher
 */
#ifndef SYNTHWORLD_C
#define SYNTHWORLD_C

#ifdef __cplusplus
#define SYNTHWORLD_C_START extern "C" {
#define SYNTHWORLD_C_END   }
#else
#define SYNTHWORLD_C_START
#define SYNTHWORLD_C_END
#endif

SYNTHWORLD_C_START;

#include "synthworld.h"
#include "world/d4c.h"
#include "world/dio.h"
#include "world/matlabfunctions.h"
#include "world/cheaptrick.h"
#include "world/stonemask.h"
#include "world/synthesis.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "HTS_hidden.h"
#include "SPTKfunctions.h"
#include <stdint.h>
#include <string.h>

WorldParameters world_parameters = { 0 };

/*
 * Print parameters for debugging
 */

int PrintParameters(WorldParameters *world_parameters){

	FILE * pFile;
	int n;
	char name [500];

	pFile = fopen ("test.hts.bap","w");
	for (int i = 0; i < world_parameters->f0_length; ++i) {
		for (int j = 0; j < world_parameters->fft_size / 2 ; ++j){
			fprintf(pFile,"%f ",world_parameters->aperiodicity[i][j]);
			//printf("%f\n",world_parameters.aperiodicity[i][j]);
		}
		fprintf(pFile,"%f\n",world_parameters->aperiodicity[i][world_parameters->fft_size / 2]);
	}
	fclose (pFile);
	pFile = fopen ("test.hts.sp","w");
	for (int i = 0; i < world_parameters->f0_length; ++i) {
		for (int j = 0; j < world_parameters->fft_size / 2 ; ++j){
			fprintf(pFile,"%f ",world_parameters->spectrogram[i][j]);
			//printf("%f\n",world_parameters.aperiodicity[i][j]);
		}
		fprintf(pFile,"%f\n",world_parameters->spectrogram[i][world_parameters->fft_size / 2]);
	}
	fclose (pFile);
	pFile = fopen ("test.hts.f0","w");
	for (int i = 0; i < world_parameters->f0_length; ++i) {
		fprintf(pFile,"%f\n",world_parameters->f0[i]);
		//printf("%f\n",world_parameters.aperiodicity[i][j]);
	}
	fclose (pFile);

	return 0;

}

/*
 * From WORLD code
 */
void WaveformSynthesis(WorldParameters *world_parameters, int fs,
    int y_length, double *y) {

	// Synthesis by the aperiodicity
   printf("\nSynthesis start\n");

	Synthesis(world_parameters->f0, world_parameters->f0_length,
      world_parameters->spectrogram, world_parameters->aperiodicity,
      world_parameters->fft_size, world_parameters->frame_period, fs,
      y_length, y);

	printf("\nSynthesis done\n");


}

/*
 * From WORLD code
 */
void WaveformSynthesis2(WorldParameters *world_parameters, int fs,
  int y_length, double *y) {
 // DWORD elapsed_time;
  printf("\nSynthesis 2 (All frames are added at the same time)\n");
  //elapsed_time = timeGetTime();

  WorldSynthesizer synthesizer = { 0 };
  int buffer_size = 64;
  InitializeSynthesizer(world_parameters->fs, world_parameters->frame_period,
      world_parameters->fft_size, buffer_size, 1, &synthesizer);

  // All parameters are added at the same time.
  AddParameters(world_parameters->f0, world_parameters->f0_length,
      world_parameters->spectrogram, world_parameters->aperiodicity,
      &synthesizer);

  int index;
  for (int i = 0; Synthesis2(&synthesizer) != 0; ++i) {
    index = i * buffer_size;
    for (int j = 0; j < buffer_size; ++j)
      y[j + index] = synthesizer.buffer[j];
  }

  //printf("WORLD: %d [msec]\n", timeGetTime() - elapsed_time);
  DestroySynthesizer(&synthesizer);
}


/*
 * From WORLD code
 */
void WaveformSynthesis3(WorldParameters *world_parameters, int fs,
    int y_length, double *y) {
  //DWORD elapsed_time;
  // Synthesis by the aperiodicity
  printf("\nSynthesis 3 (Ring buffer is efficiently used.)\n");
 // elapsed_time = timeGetTime();

  WorldSynthesizer synthesizer = { 0 };
  int buffer_size = 64;
  InitializeSynthesizer(world_parameters->fs, world_parameters->frame_period,
      world_parameters->fft_size, buffer_size, 100, &synthesizer);

  int offset = 0;
  int index = 0;
  for (int i = 0; i < world_parameters->f0_length;) {
    // Add one frame (i shows the frame index that should be added)
    if (AddParameters(&world_parameters->f0[i], 1,
      &world_parameters->spectrogram[i], &world_parameters->aperiodicity[i],
      &synthesizer) == 1) ++i;

    // Synthesize speech with length of buffer_size sample.
    // It is repeated until the function returns 0
    // (it suggests that the synthesizer cannot generate speech).
    while (Synthesis2(&synthesizer) != 0) {
      index = offset * buffer_size;
      for (int j = 0; j < buffer_size; ++j)
        y[j + index] = synthesizer.buffer[j];
      offset++;
    }

    // Check the "Lock" (Please see synthesisrealtime.h)
    if (IsLocked(&synthesizer) == 1) {
      printf("Locked!\n");
      break;
    }
  }

  //printf("WORLD: %d [msec]\n", timeGetTime() - elapsed_time);
  DestroySynthesizer(&synthesizer);
}

int DestroyMemory() {

	delete[] world_parameters.time_axis;
	delete[] world_parameters.f0;
	delete[] world_parameters.mgc;
	delete[] world_parameters.spec;
	delete[] world_parameters.imag;

	for (int i = 0; i < world_parameters.f0_length; ++i) {
		delete[] world_parameters.spectrogram[i];
	    delete[] world_parameters.aperiodicity[i];
	  }

	delete[] world_parameters.spectrogram;
	delete[] world_parameters.aperiodicity;

}


/*
 * Synthesis as in merlin with synth program and copy_synthesis.sh.
 * Analysis part of training script must match analysis part in copy_synthesis.sh.
 */

int synthesizeWorld(HTS_GStreamSet * gss, int fs,size_t fperiod, double alpha) {

	int debug=0;
    // You must set fs and frame_period before analysis/synthesis.
    world_parameters.fs = fs;
    // 5.0 ms is the default value.
    // Generally, the inverse of the lowest F0 of speech is the best.
    // However, the more elapsed time is required.
    world_parameters.frame_period = ((double)fperiod/fs)*1000;


    CheapTrickOption option = {0};
    // This value may be better one for HMM speech synthesis.
    // Default value is -0.09.
    option.q1 = -0.15;
    // Important notice (2016/02/02)
    // You can control a parameter used for the lowest F0 in speech.
    // You must not set the f0_floor to 0.
    // It will cause a fatal error because fft_size indicates the infinity.
    // You must not change the f0_floor after memory allocation.
    // You should check the fft_size before excucing the analysis/synthesis.
    // The default value (71.0) is strongly recommended.
    // On the other hand, setting the lowest F0 of speech is a good choice
    // to reduce the fft_size.
    option.f0_floor = 71.0;
    // Parameters setting and memory allocation.
    world_parameters.fft_size =
        GetFFTSizeForCheapTrick(world_parameters.fs, &option);
    int specsize=(world_parameters.fft_size / 2 + 1);

    //Copy HTS_GStreamSet parameters to world_parameters
    int mgcdim=gss->gstream[0].vector_length-1;
    int lf0dim=gss->gstream[1].vector_length;
    int bapnum=gss->gstream[2].vector_length;

    if(debug){
    	   printf("%f frame period in ms\n",world_parameters.frame_period);  //MGC
    	   printf("%d fs\n",fs);  //MGC
    	   printf("%d samples\n",gss->total_nsample);  //MGC
    	   printf("%d fft size\n",world_parameters.fft_size);  //MGC
    	   printf("%d spec size\n",specsize);  //MGC
    	   printf("%d MGC values\n",mgcdim);  //MGC
    	   printf("%d LF0 value\n",lf0dim); //LF0
    	   printf("%d BAP values\n",bapnum); //BAP
    	   printf("%d number of frames\n",gss->total_frame); //BAP
    	   printf("%f alpha\n",alpha); //BAP

    }

    //Allocate F0 array
    world_parameters.f0 = new double[gss->total_frame];
    world_parameters.time_axis = new double[gss->total_frame];
    world_parameters.f0_length=gss->total_frame;

    int i;
    //Convert LogFO to F0
    for (i = 0; i < world_parameters.f0_length; ++i){
    	world_parameters.f0[i] = exp(gss->gstream[1].par[i][0]);
    	//printf("%f\n",world_parameters.f0[i]);
    }
    //Set negative values to 0.0
    for (i = 0; i < world_parameters.f0_length; ++i){
    	if(world_parameters.f0[i]<0.0){
    		world_parameters.f0[i]=0.0;
    	}
    }

    //Arrays to convert mgc to spectrum using SPTK function mgc2sp
    world_parameters.spec =  new double[world_parameters.fft_size];
    world_parameters.imag =  new double[world_parameters.fft_size];
    world_parameters.mgc = new double[mgcdim+1];

    //Allocate spectrogram matrix
    world_parameters.spectrogram = new double *[world_parameters.f0_length];
    for (int i = 0; i < world_parameters.f0_length; ++i)
         world_parameters.spectrogram[i] =
        		 new double[specsize];

    //Copy mgc values from gstream
    for (int i = 0; i < world_parameters.f0_length; ++i){
    	for (int j = 0; j < mgcdim+1; ++j){
    		//load one frame into mgc array
    		world_parameters.mgc[j] = gss->gstream[0].par[i][j];
    		// printf("%f\n",gss->gstream[0].par[i][j]);
    		}
    	//convert mgc frame to spectrum
    	mgc2sp(world_parameters.mgc,mgcdim,alpha,0.0,world_parameters.spec,world_parameters.imag,world_parameters.fft_size);
    	//copy spectrum to half side spectrogram
    	for (int j = 0; j<specsize;j++){
    		world_parameters.spectrogram[i][j] = pow(exp(world_parameters.spec[j])/32768.0,2.0);
    		//printf("%f\n",world_parameters.spectrogram[i][j]);
    		}
    }

    // Copy coarse aperiodicities
    double **coarse_aperiodicities = new double *[world_parameters.f0_length];
    world_parameters.aperiodicity = new double *[world_parameters.f0_length];

    for (int i = 0; i < world_parameters.f0_length; ++i) {
        world_parameters.aperiodicity[i] = new double[specsize];
        coarse_aperiodicities[i]  = new double[bapnum];
      }

    //Copy gstream into coarse aperiodicities
    for (int i = 0; i < world_parameters.f0_length; ++i){
    	for (int j = 0; j < bapnum; ++j){
    		coarse_aperiodicities[i][j] = gss->gstream[2].par[i][j];
    	}
    }
    // convert bandaps to full aperiodic spectrum by interpolation (originally in d4c extraction):
    // Linear interpolation to convert the coarse aperiodicity into its
    // spectral representation.

    // -- for interpolating --
    double *coarse_aperiodicity = new double[bapnum + 2];
    coarse_aperiodicity[0] = -60.0;
    coarse_aperiodicity[bapnum + 1] = 0.0;
    double *coarse_frequency_axis = new double[bapnum + 2];

    for (int i = 0; i <= bapnum; ++i)
    	coarse_frequency_axis[i] = static_cast<double>(i) * 3000.0;

    coarse_frequency_axis[bapnum + 1] = fs / 2.0;
    double *frequency_axis = new double[specsize];

    for (int i = 0; i <= world_parameters.fft_size / 2; ++i)
    	frequency_axis[i] = static_cast<double>(i) * fs / world_parameters.fft_size;

    for (int i = 0; i < world_parameters.f0_length; ++i) {
    	// load band ap values for this frame into  coarse_aperiodicity
    	for (int k = 0; k < bapnum; ++k) {
    		coarse_aperiodicity[k+1] = coarse_aperiodicities[i][k];
    	}
    	interp1(coarse_frequency_axis, coarse_aperiodicity, bapnum + 2,
    			frequency_axis, specsize, world_parameters.aperiodicity[i]);

    	for (int j = 0; j <= world_parameters.fft_size / 2; ++j){
    		world_parameters.aperiodicity[i][j] = pow(10.0, world_parameters.aperiodicity[i][j] / 20.0);
    		//printf("%f\n",world_parameters.aperiodicity[i][j]);
    	}
    }

    //Allocate output vector
    int y_length = ((world_parameters.f0_length - 1) *
    		fperiod) + 1;
    double *y = new double[y_length];


    //PrintParameters(&world_parameters);

    WaveformSynthesis3(&world_parameters, fs, y_length, gss->gspeech);

    int16_t tmp_signal;
    for (int i = 0; i < y_length; ++i) {
    	tmp_signal = static_cast<int16_t>(MyMaxInt(-32768,
    			MyMinInt(32767, static_cast<int>(gss->gspeech[i] * 32767))));
    	gss->gspeech[i]=tmp_signal;
    }

    for (int i=0; i<world_parameters.f0_length; i++){
    	delete[] coarse_aperiodicities[i];
    }
    delete[] coarse_aperiodicities;
    delete[] coarse_aperiodicity;
    delete[] frequency_axis;

    printf("complete.\n");
    return 0;

}

SYNTHWORLD_C_END;

#endif                       /* !HTS_GSTREAM_C */

