/*
 *  Copyright 2009,2010 Reality Jockey, Ltd.
 *                 info@rjdj.me
 *                 http://rjdj.me/
 * 
 *  This file is part of ZenGarden.
 *
 *  ZenGarden is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ZenGarden is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ZenGarden.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "me_rjdj_zengarden_ZenGarden.h"
#include "ZenGarden.h"

#define JNI_VERSION JNI_VERSION_1_4

JavaVM *jvm = NULL; // global variable

typedef struct {
  jobject zgObject;
  PdContext *context;
  float *finputBuffer;
  float *foutputBuffer;
  int blockSize;
  int numInputChannels; // the number of input channels supplied by the system
  int numOutputChannels; // the number of input channels supplied by the system
  float *shortToFloatLookupTable;
  int numBytesInBlock;
} PureDataMobileNativeVars;

extern "C" {
  void java_callback(ZGCallbackFunction function, void *userData, void *ptr) {
    JNIEnv *env = NULL;
    jint result = jvm->GetEnv((void **)&env, JNI_VERSION);
    if (result == JNI_OK && env != NULL) {
      jobject zgObject = (jobject) userData;
      switch (function) {
        case ZG_PRINT_STD: {
          env->CallVoidMethod(zgObject,
              env->GetMethodID(env->GetObjectClass(zgObject),
                  "onPrintStd", "(Ljava/lang/String;)V"),
              env->NewStringUTF((char *) ptr));
          break;
        }
        case ZG_PRINT_ERR: {
          env->CallVoidMethod(zgObject,
              env->GetMethodID(env->GetObjectClass(zgObject),
                  "onPrintErr", "(Ljava/lang/String;)V"),
              env->NewStringUTF((char *) ptr));
          break;
        }
        default: {
          break;
        }
      }
    }
  }
}

inline float shortSampleToFloat(short s, PureDataMobileNativeVars *pdmnv) {
  //pdmnv->finputBuffer[z] = ((float) cinputBuffer[j]) / 32767.0f;
  if (s >= 0) {
    return pdmnv->shortToFloatLookupTable[s];
  } else {
    // short s is known to be a negative number. It is made positive by taking the 2's complement.
    // The complement is then used to lookup the corresponding float value from the lookup table.
    // The (positive) float is then made negative by setting the high bit to 1.
    int floatAsInt = (*(int *)(pdmnv->shortToFloatLookupTable + (~s + 1))) | 0x80000000;
    return *(float *)&floatAsInt;
  }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *ur_jvm, void *reserved) {
  // cache the java vm pointer
  // used for getting a pointer to the java environment during callbacks
  jvm = ur_jvm;
  
  return JNI_VERSION;
}

JNIEXPORT jlong JNICALL Java_me_rjdj_zengarden_ZenGarden_loadPdPatch(
    JNIEnv *env, jobject jobj, jstring jdirectory, jstring jfilename, jint blockSize,
    jint numInputChannels, jint numOutputChannels, jfloat sampleRate) {

  // initialise the PureDataMobile native variables
  PureDataMobileNativeVars *pdmnv = (PureDataMobileNativeVars *) malloc(sizeof(PureDataMobileNativeVars));
  pdmnv->zgObject = env->NewGlobalRef(jobj);
  pdmnv->context = zg_new_context(numInputChannels, numOutputChannels, blockSize, sampleRate,
      java_callback, pdmnv->zgObject);
  pdmnv->finputBuffer = (float *) malloc(blockSize * numInputChannels * sizeof(float));
  pdmnv->foutputBuffer = (float *) malloc(blockSize * numOutputChannels * sizeof(float));
  pdmnv->blockSize = blockSize;
  pdmnv->numInputChannels = numInputChannels;
  pdmnv->numOutputChannels = numOutputChannels;
  pdmnv->shortToFloatLookupTable = (float *) malloc(32768 * sizeof(float));
  pdmnv->numBytesInBlock = blockSize * sizeof(float);
  
  
  char *cdirectory = (char *) env->GetStringUTFChars(jdirectory, NULL);
  char *cfilename = (char *) env->GetStringUTFChars(jfilename, NULL);
  PdGraph *graph = zg_new_graph(pdmnv->context, cdirectory, cfilename);
  env->ReleaseStringUTFChars(jdirectory, cdirectory);
  env->ReleaseStringUTFChars(jfilename, cfilename);
  if (graph == NULL) {
    // an error has happened. Clean up and throw an exception.
    Java_me_rjdj_zengarden_ZenGarden_unloadPdPatch(env, jobj, (jlong) pdmnv);
    env->ThrowNew(env->FindClass("me/rjdj/zengarden/NativeLoadException"),
        "Graph could not be created. Is the pd file properly formatted? Are all of the referenced objects implemented?");
    return 0;
  }
  zg_attach_graph(pdmnv->context, graph);
  
  // define the input (microphone) waveshaper
  // linear
  for (int i = 0; i < 32768; i++) {
    pdmnv->shortToFloatLookupTable[i] = ((float) i) / 32767.0f;
  }
  
  return (jlong) pdmnv;
}

JNIEXPORT void JNICALL Java_me_rjdj_zengarden_ZenGarden_unloadPdPatch(
    JNIEnv *env, jobject jobj, jlong nativePtr) {
  
  if (nativePtr != 0) { // sanity check
    // free all of the pure data mobile native variables
    PureDataMobileNativeVars *pdmnv = (PureDataMobileNativeVars *) nativePtr;
    env->DeleteGlobalRef(pdmnv->zgObject);
    free(pdmnv->finputBuffer);
    free(pdmnv->foutputBuffer);
    free(pdmnv->shortToFloatLookupTable);
    zg_delete_context(pdmnv->context);
    free(pdmnv);
  }
}

// this function uses bit-wise manupulation in order to more quickly multiply floats by
// -1 or 32768. See http://en.wikipedia.org/wiki/IEEE_754-1985
JNIEXPORT void JNICALL Java_me_rjdj_zengarden_ZenGarden_process(
    JNIEnv *env, jobject jobj, jshortArray jinputBuffer, jshortArray joutputBuffer, jlong nativePtr) {
  
  PureDataMobileNativeVars *pdmnv = (PureDataMobileNativeVars *) nativePtr;
  
  // set up the floating point input buffers
  short *cinputBuffer = (short *) env->GetPrimitiveArrayCritical(jinputBuffer, NULL);
  switch (pdmnv->numInputChannels) {
    case 1: { // if there is only one input channel (mono)
      // convert all shorts to floats
      for (int i = 0; i < pdmnv->blockSize; i++) {
        pdmnv->finputBuffer[i] = shortSampleToFloat(cinputBuffer[i], pdmnv);
      }
      // copy the float buffer from the left to right channel
      memcpy(pdmnv->finputBuffer + pdmnv->blockSize, pdmnv->finputBuffer, pdmnv->numBytesInBlock);
      break;
    }
    case 2: { // if there is a stereo input
      // uninterleave the short buffer
      for (int i = 0, j = 0; i < pdmnv->blockSize; i++) {
        for (int k = 0, z = i; k < pdmnv->numInputChannels; k++, j++, z+=pdmnv->blockSize) {
          pdmnv->finputBuffer[z] = shortSampleToFloat(cinputBuffer[j], pdmnv);
        }
      }
      break;
    }
    default: {
      break;
    }
  }
  env->ReleasePrimitiveArrayCritical(jinputBuffer, cinputBuffer, JNI_ABORT); // no need to copy back changes. release native buffer.
  
  zg_process(pdmnv->context, pdmnv->finputBuffer, pdmnv->foutputBuffer);
  
  // read back from the floating point output buffers
  // regarding use of lrintf, see http://www.mega-nerd.com/FPcast/
  short *coutputBuffer = (short *) env->GetPrimitiveArrayCritical(joutputBuffer, NULL);
  for (int i = 0, j = 0; i < pdmnv->blockSize; i++) {
    for (int k = 0, z = i; k < pdmnv->numOutputChannels; k++, j++, z+=pdmnv->blockSize) {
      // clip the output (like Pd does)
      float f = pdmnv->foutputBuffer[z];
      if (f > 1.0f) coutputBuffer[j] = 32767;
      else if (f < -1.0f) coutputBuffer[j] = -32768;
      else coutputBuffer[j] = (short) (f * 32767.0f);
      /*
       * WARNING: if pdmnv->foutputBuffer[z] == 1.0f, this method will result in -32768, not 32767
       * and cause a click in the output (i.e., there is overflow when converting to a short)
       int floatAsInt = *(int *)(pdmnv->foutputBuffer+z);
       int exponent = floatAsInt & 0x7F800000;
       exponent += 0x07800000; // multiply by 32768.0f == 2^15 (add 15 to the exponent)
       floatAsInt = (floatAsInt & 0x807FFFFF) | exponent;
       coutputBuffer[j] = (short) lrintf(*(float *)&floatAsInt);
       */
    }
  }
  env->ReleasePrimitiveArrayCritical(joutputBuffer, coutputBuffer, 0); // copy back the changes and release native buffer
}
