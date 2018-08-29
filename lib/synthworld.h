/*
 * synthworld.h
 *
 *  Created on: 06.08.2018
 *      Author: Michael Pucher
 */

#ifndef SYNTHWORLD_H_
#define SYNTHWORLD_H_

#ifdef __cplusplus
#define SYNTHWORLD_H_START extern "C" {
#define SYNTHWORLD_H_END   }
#else
#define SYNTHWORLD_H_START
#define SYNTHWORLD_H_END
#endif

SYNTHWORLD_H_START;

#include "world/d4c.h"
#include "world/dio.h"
#include "world/matlabfunctions.h"
#include "world/cheaptrick.h"
#include "world/stonemask.h"
#include "world/synthesis.h"
#include "world/synthesisrealtime.h"
#include <stdlib.h>
#include  <stdio.h>
#include <math.h>
#include "HTS_hidden.h"



typedef struct {
	    double frame_period;
	    int fs;

	    double *f0;
	    double *time_axis;
	    int f0_length;

	    double **spectrogram;
	    //spec, imag, and mgc as one-dimensional array for SPTK functions
	    double *spec;
	    double *imag;
	    double *mgc;

	    double **aperiodicity;

	    int fft_size;
	} WorldParameters;

	int PrintParameters(WorldParameters *world_parameters);

	void WaveformSynthesis(WorldParameters *world_parameters, int fs,
	    int y_length, double *y);

	void WaveformSynthesis2(WorldParameters *world_parameters, int fs,
	  int y_length, double *y);

	void WaveformSynthesis3(WorldParameters *world_parameters, int fs,
		  int y_length, double *y);

	int DestroyMemory();

	int synthesizeWorld(HTS_GStreamSet * gss, int fs,size_t fperiod, double alpha);


SYNTHWORLD_H_END;


#endif /* SYNTHWORLD_H_ */
