#include <stdio.h>
#include <string>
#include <map>

#include <stdlib.h>
#include "aacenc_lib.h"

#include <stdio.h>
#include "exporter.h"

int test(void) {
    printf("%d",sizeof(INT_PCM));
    printf("adios hola mundo\n");
    return 1;
}

INT inBufferIds[] = { IN_AUDIO_DATA };
INT outBufferIds[] = { OUT_BITSTREAM_DATA };
INT inBufferElSize[] = { sizeof (INT_PCM) };
INT outBufferElSize[] = { sizeof(UCHAR) };
INT inBufferSize[] = { 32768 };
INT outBufferSize[] = { 819200 };

class encoder_config {
public:
	HANDLE_AACENCODER encoder;

	// input
	INT_PCM inputBuffer[8*2048];
	void *inBuffer;
	AACENC_BufDesc inBufDesc;

	// output
	UCHAR outputBuffer[819200];
	void *outBuffer;
	AACENC_BufDesc outBufDesc;
	std::string test;
};
std::map <int,encoder_config *> encoders;

int openEncoder(int index, UCHAR *inputBuffer) {	
	encoder_config *config = new encoder_config();
	config->test="hola mundo";
	printf("open encoder\n");
	
	// solo se coge para stereo
	if (aacEncOpen(&(config->encoder), 0, 0) != AACENC_OK){
		return 1;	
	}

	// config mandatory parameters
	aacEncoder_SetParam(config->encoder, AACENC_CHANNELMODE, MODE_2);
	aacEncoder_SetParam(config->encoder, AACENC_AOT, AOT_AAC_LC);
	aacEncoder_SetParam(config->encoder, AACENC_BITRATE, 128000);
	aacEncoder_SetParam(config->encoder, AACENC_SAMPLERATE, 44100);

/*	
	INT inBufferIds[] = { IN_AUDIO_DATA };
	INT outBufferIds[] = { OUT_BITSTREAM_DATA };
	INT inBufferElSize[] = { sizeof (INT_PCM) };
	INT outBufferElSize[] = { sizeof(UCHAR) };
	INT inBufferSize[] = { sizeof (config->inputBuffer) };
	INT outBufferSize[] = { sizeof (config->outputBuffer) };
	printf("input %d output %d\n", sizeof(config->inputBuffer), sizeof(config->outputBuffer));
	*/

	// input buffer
	config->inBuffer = inputBuffer; //config->inputBuffer;
	config->inBufDesc.numBufs = 1;
	config->inBufDesc.bufs = (void **) &(config->inBuffer);
	config->inBufDesc.bufferIdentifiers = inBufferIds; 
	config->inBufDesc.bufSizes = inBufferSize;
	config->inBufDesc.bufElSizes = inBufferElSize;
	printf("post input buffer\n");

	// output buffer
	config->outBuffer = config->outputBuffer;
	config->outBufDesc.numBufs = 1;
	config->outBufDesc.bufs = (void **) &(config->outBuffer);
	config->outBufDesc.bufferIdentifiers = outBufferIds;
	config->outBufDesc.bufSizes = outBufferSize;
	config->outBufDesc.bufElSizes = outBufferElSize;
	printf("post output buffer\n");
	
	// initialize
	printf("config encoder\n");
	if ( aacEncEncode(config->encoder, NULL, NULL, NULL, NULL) != AACENC_OK) {
		printf("error initialize\n");
		return 3;
	}

	encoders[index] = config;
/*	encode();
	encode();
	encode();*/

	return 0;
}	

int counter = 0;
int encode() {
	encoder_config *config = encoders[1];
	
//	printf("zeros %d\n",zeros);
	AACENC_InArgs inArgs;
	inArgs.numInSamples=2048;
	inArgs.numAncBytes=0;
	AACENC_OutArgs outArgs;
	AACENC_ERROR error = aacEncEncode(config->encoder, &(config->inBufDesc), &(config->outBufDesc), &inArgs, &outArgs);	
	if (error != AACENC_OK) {
		printf("error encoding\n");
		return 2;
	}

	counter++;
	if (counter % 100 == 0){
		printf("1 outBytes %d inSamples %d ancbytes %d bitResState %d\n", 
				outArgs.numOutBytes,
				outArgs.numInSamples,
				outArgs.numAncBytes,
				outArgs.bitResState);
	}

	return outArgs.numOutBytes; 
}

void * getOutputBuffer(int index) {
	return encoders[index]->outputBuffer;
}
