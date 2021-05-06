/*
 * heapHelper.h
 *
 *  Created on: 4 May 2021
 *      Author: nimco
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>
#include "jni.h"
#include "jvmti.h"

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved);
JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options, void* reserved);
JNIEXPORT jobjectArray JNICALL Java_org_softauto_jvm_HeapHelper_findInstances(JNIEnv *env, jclass thisClass, jclass klass);


