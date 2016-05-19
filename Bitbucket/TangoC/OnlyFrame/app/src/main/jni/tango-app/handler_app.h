//
// Created by bastien on 16/05/16.
//

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

#ifndef TANGO_HANDLER_APP_H_
#define TANGO_HANDLER_APP_H_

#include <jni.h>
#include <memory>
#include <string>

#include <tango_client_api.h>
#include <tango-gl/util.h>

#include <tango-app/point_cloud_data.h>
#include <tango-app/pose_data.h>
#include <tango-app/picture_yv12.h>
#include <tango-app/tango_event_data.h>

namespace tango_handler_app {

// PointCloudApp handles the application lifecycle and resources.
class HandlerApp {
 public:
  // Constructor and deconstructor.
  HandlerApp();
  ~HandlerApp();

  // Tango Service point cloud callback function for depth data. Called when new
  // new point cloud data is available from the Tango Service.
  //
  // @param pose: The current point cloud returned by the service,   //              caller allocated.
  void onPointCloudAvailable(const TangoXYZij* xyz_ij);

  // Tango service pose callback function for pose data. Called when new
  // information about device pose is available from the Tango Service.
  //
  // @param pose: The current pose returned by the service, caller allocated.
  void onPoseAvailable(const TangoPoseData* pose);

  // Check that the installed version of the Tango API is up to date.
  //
  // @return returns true if the application version is compatible with the
  //         Tango Core version.
  bool CheckTangoVersion(JNIEnv* env, jobject activity, int min_tango_version);

  // Called when Tango Service is connected successfully.
  bool OnTangoServiceConnected(JNIEnv* env, jobject binder);

  // Setup the configuration file for the Tango Service.
  int TangoSetupConfig();

  // Connect the onPoseAvailable callback.
  int TangoConnectCallbacks();

  // Connect to Tango Service.
  // This function will start the Tango Service pipeline, in this case, it will
  // start Motion Tracking and Depth Sensing callbacks.
  bool TangoConnect();

  // Disconnect from Tango Service, release all the resources that the app is
  // holding from the Tango Service.
  void TangoDisconnect();

  // Explicitly reset motion tracking and restart the pipeline.
  // Note that this will cause motion tracking to re-initialize.
  void TangoResetMotionTracking();

  // Retrun pose debug string.
  std::string GetPoseString();

  // Retrun point cloud debug string.
  std::string GetPointCloudString();

  // Retrun picture debug string.
  std::string GetPictureString();

  // Retrun Tango event debug string.
  std::string GetEventString();

  // Tango service event callback function for pose data. Called when new events
  // are available from the Tango Service.
  //
  // @param event: Tango event, caller allocated.
  void onTangoEventAvailable(const TangoEvent* event);

  // Tango service pose callback function for frame data. Called when new
  // information about device frame is available from the Tango Service.
  //
  // @param pose: The current frame returned by the service, caller allocated.
  void onFrameAvailable(TangoCameraId id, const TangoImageBuffer* buffer);

  TangoErrorType SetupConfigB();
  TangoErrorType ConnectYUVFrameCallbackB();
  void frameProcess(const TangoImageBuffer* buffer);

 private:
  // Query sensor/camera extrinsic from the Tango Service, the extrinsic is only
  // available after the service is connected.
  //
  // @return: error code.
  TangoErrorType UpdateExtrinsics();

  // point_cloud_ contains the data of current depth frame, it also
  // has the render function to render the points. This instance will be passed
  // to main_scene_ for rendering.
  //
  // point_cloud_ is a thread safe object, the data protection is handled
  // internally inside the PointCloud class.
  PointCloudData point_cloud_data_;

  // Mutex for protecting the point cloud data. The point cloud data is shared
  // between render thread and TangoService callback thread.
  std::mutex point_cloud_mutex_;

  // pose_data_ handles all pose onPoseAvailable callbacks, onPoseAvailable()
  // in this object will be routed to pose_data_ to handle.
  PoseData pose_data_;

  // Mutex for protecting the pose data. The pose data is shared between render
  // thread and TangoService callback thread.
  std::mutex pose_mutex_;

  // Tango configration file, this object is for configuring Tango Service setup
  // before connect to service. For example, we turn on the depth sensing in
  // this example.
  TangoConfig tango_config_;

  // Mutex for protecting the picture data.
  std::mutex picture_mutex_;

  // picture_data_ handles all pose onFrameAvailable callbacks, onFrameAvailable()
  // in this object will be routed to picture_data to handle.
  PictureDataYV12 picture_data_;

  // tango_event_data_ handles all Tango event callbacks,
  // onTangoEventAvailable() in this object will be routed to tango_event_data_
  // to handle.
  TangoEventData tango_event_data_;

  // tango_event_data_ is share between the UI thread we start for updating
  // debug
  // texts and the TangoService event callback thread. We keep event_mutex_ to
  // protect tango_event_data_.
  std::mutex tango_event_mutex_;

};
}  // namespace tango_handler_app


#endif  // TANGO_HANDLER_APP_H_