/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define GLM_FORCE_RADIANS

#include <jni.h>
#include <tango-app/handler_app.h>

static tango_handler_app::HandlerApp app;

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jboolean JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_checkTangoVersion(
    JNIEnv* env, jobject, jobject activity, jint min_tango_version) {
  return app.CheckTangoVersion(env, activity, min_tango_version);
}

JNIEXPORT jboolean JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_onTangoServiceConnected(
    JNIEnv* env, jobject /*caller_object*/, jobject binder) {
  return app.OnTangoServiceConnected(env, binder);
}

JNIEXPORT jint JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_setupConfig(
    JNIEnv*, jobject) {
  return app.TangoSetupConfig();
}

JNIEXPORT jboolean JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_connect(JNIEnv*,
                                                                     jobject) {
  return app.TangoConnect();
}

JNIEXPORT jint JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_connectCallbacks(
    JNIEnv*, jobject) {
  return app.TangoConnectCallbacks();
}

JNIEXPORT void JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_disconnect(
    JNIEnv*, jobject) {
  app.TangoDisconnect();
}

JNIEXPORT jstring JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_getPoseString(
    JNIEnv* env, jobject) {
  return (env)->NewStringUTF(app.GetPoseString().c_str());
}

JNIEXPORT jstring JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_getPointCloudString(
    JNIEnv* env, jobject) {
  return (env)->NewStringUTF(app.GetPointCloudString().c_str());
}

JNIEXPORT jstring JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_getPictureString(
    JNIEnv* env, jobject) {
  return (env)->NewStringUTF(app.GetPictureString().c_str());
}

JNIEXPORT jstring JNICALL
Java_com_example_bastien_tangodatahandler_TangoJNINative_getTest(
    JNIEnv* env, jobject) {

  return (env)->NewStringUTF("Hello World FROM JNI");
}

#ifdef __cplusplus
}
#endif
