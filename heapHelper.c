/*
 * heapHelper.c
 *
 *  Created on: 4 May 2021
 *      Author: nimco
 */

#include "jvmti.h"
#include <stdlib.h>
#include "jni.h"

jvmtiEnv *jvmti = NULL;

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved)
{

  jvmtiCapabilities capa;
  jvmtiError error;

  // put a jvmtiEnv instance at jvmti.
  jint result = (*jvm)->GetEnv(jvm, (void **)&jvmti, JVMTI_VERSION_1);
  if (result != JNI_OK) {
    printf("ERROR: Unable to access JVMTI!\n");
  }
  char** p = &jvmti;

  jvmtiCapabilities capabilities = {0};
  capabilities.can_tag_objects = 1;
  error = (*jvmti)->AddCapabilities(*jvmti,&capabilities);

  return JNI_OK;
}

JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options, void* reserved)
{

	    _flushall();
	    jint err = (*vm)->GetEnv(vm, (void **)&jvmti, JVMTI_VERSION_1);
	    if (err != JNI_OK)
	    {
	        printf("Failed to get JVMTI env!\n");
	        _flushall();
	        return err;
	    }

	    return JNI_OK;
}

static jint JNICALL HeapObjectCallback(jlong class_tag, jlong size, jlong* tag_ptr, void* user_data) {
	*tag_ptr = 1;
    return JVMTI_ITERATION_CONTINUE;
}

JNIEXPORT jobjectArray JNICALL Java_org_softauto_jvm_HeapHelper_findInstances(JNIEnv *env, jclass thisClass, jclass klass)
{

		JavaVM* vm;
	    jint err = (*env)->GetJavaVM(*env,&vm);
	    if (err != JNI_OK)
	    	    {
	    	        printf("Failed to get JVMTI env!\n");
	    	        _flushall();

	    	    }

	jvmtiEnv* jvmti;
	jint err1 = (*vm)->GetEnv(vm, (void **)&jvmti, JVMTI_VERSION_1);
	if (err1 != JNI_OK)
	        	    {
	        	        printf("Failed to get JVMTI env!\n");
	        	        _flushall();

	       	    }

	jvmtiCapabilities capabilities = {0};
    capabilities.can_tag_objects = 1;
    jvmtiError addCapabilitiesError = (*jvmti)->AddCapabilities(jvmti,&capabilities);
    if ( addCapabilitiesError != JVMTI_ERROR_NONE ) {
             char       *error_str;
             const char *str;
             error_str = NULL;
             (void)(*jvmti)->GetErrorName(jvmti, addCapabilitiesError, &error_str);
             printf("ERROR: JVMTI: %d(%s): %s\n", addCapabilitiesError,error_str);


         }
    int count = 0;
    jvmtiHeapCallbacks callbacks;
    (void)memset(&callbacks, 0, sizeof(callbacks));
    callbacks.heap_iteration_callback = &HeapObjectCallback;

    //force GC run
    (*jvmti)->ForceGarbageCollection(jvmti);
    //get all klass instances
    jvmtiError error = (*jvmti)->IterateThroughHeap(jvmti,0, klass, &callbacks, &count);
    if ( error != JVMTI_ERROR_NONE ) {
         char       *error_str;
         const char *str;
         error_str = NULL;
         (void)(*jvmti)->GetErrorName(jvmti, error, &error_str);
         printf("ERROR: JVMTI: %d(%s): %s\n", error,error_str);

     }

     jlong tag = 1;
     jobject* instances;
     jobjectArray ret;
     (*jvmti)->GetObjectsWithTags(jvmti,1, &tag, &count, &instances, NULL);
     jmethodID cid = (*env)->GetMethodID(env,klass, "<init>", "()V");
     if(cid == NULL){
    	 printf("fail to get constructor id \n");
     }
     //build the return array
     ret= (jobjectArray)(*env)->NewObjectArray(env,count,klass,  NULL);
     int i;
     for(i=0;i<count;i++) {
    	  //(*env)->SetObjectArrayElement(env,ret,i,(*env)->NewObject(env,klass, cid, instances[i]));
    	  (*env)->SetObjectArrayElement(env,ret,i,instances[i]);
       }
 return (ret);
}

JNIEXPORT jclass JNICALL Java_org_softauto_jvm_HeapHelper_findClass(JNIEnv *env, jclass thisClass, jstring classPath)
{
	jclass class = (*env)->FindClass(env,classPath);
	     if (class == NULL){
	    	 printf("not Found class\n", classPath);
	     }
	return class;
}
