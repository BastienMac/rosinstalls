//
// Created by bastien on 16/05/16.
//

#include <tango-gl/conversions.h>
#include <tango_support_api.h>

#include "tango-app/handler_app.h"

namespace {
const int kVersionStringLength = 128;

// This function routes onXYZijAvailable callbacks to the application object for
// handling.
//
// @param context, context will be a pointer to a HandlerApp
//        instance on which to call callbacks.
// @param pose, pose data to route to onPoseAvailable function.
void onPointCloudAvailableRouter(void* context, const TangoXYZij* xyz_ij) {
  tango_handler_app::HandlerApp* app =
      static_cast<tango_handler_app::HandlerApp*>(context);
  app->onPointCloudAvailable(xyz_ij);
}

// This function routes onPoseAvailable callbacks to the application object for
// handling.
//
// @param context, context will be a pointer to a HandlerApp
//        instance on which to call callbacks.
// @param pose, pose data to route to onPoseAvailable function.
void onPoseAvailableRouter(void* context, const TangoPoseData* pose) {
  tango_handler_app::HandlerApp* app =
      static_cast<tango_handler_app::HandlerApp*>(context);
  app->onPoseAvailable(pose);
}

// This function routes onTangoEvent callbacks to the application object for
// handling.
//
// @param context, context will be a pointer to a HandlerApp
//        instance on which to call callbacks.
// @param event, TangoEvent to route to onTangoEventAvailable function.
void onTangoEventAvailableRouter(void* context, const TangoEvent* event) {
  tango_handler_app::HandlerApp* app =
      static_cast<tango_handler_app::HandlerApp*>(context);
  app->onTangoEventAvailable(event);
}

void onFrameAvailableRouter(void* context, TangoCameraId id, const TangoImageBuffer* buffer) {
  tango_handler_app::HandlerApp* app = static_cast<tango_handler_app::HandlerApp*>(context);
  app->onFrameAvailable(id, buffer);
}

void onFrameAvailableB( void* context, const TangoCameraId id, const TangoImageBuffer* buffer ){
  tango_handler_app::HandlerApp* app = static_cast<tango_handler_app::HandlerApp*>(context);
  app->frameProcess(buffer);
}

}  // namespace

namespace tango_handler_app {
void HandlerApp::onPointCloudAvailable(const TangoXYZij* xyz_ij) {
  std::lock_guard<std::mutex> lock(point_cloud_mutex_);
  point_cloud_data_.UpdatePointCloud(xyz_ij);
}

void HandlerApp::onPoseAvailable(const TangoPoseData* pose) {
  std::lock_guard<std::mutex> lock(pose_mutex_);
  pose_data_.UpdatePose(pose);
}

HandlerApp::HandlerApp() {}

HandlerApp::~HandlerApp() {
  if (tango_config_ != nullptr) {
    TangoConfig_free(tango_config_);
  }
}

bool HandlerApp::CheckTangoVersion(JNIEnv* env, jobject activity,
                                      int min_tango_version) {
  // Check the installed version of the TangoCore.  If it is too old, then
  // it will not support the most up to date features.
  int version;
  TangoErrorType err = TangoSupport_GetTangoVersion(env, activity, &version);
  return err == TANGO_SUCCESS && version >= min_tango_version;
}

bool HandlerApp::OnTangoServiceConnected(JNIEnv* env, jobject binder) {
  TangoErrorType ret = TangoService_setBinder(env, binder);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: Failed to set Binder Tango service with error code: %d", ret);
    return false;
  }
  return true;
}

int HandlerApp::TangoSetupConfig() {
  // Here, we'll configure the service to run in the way we'd want. For this
  // application, we'll start from the default configuration
  // (TANGO_CONFIG_DEFAULT). This enables basic motion tracking capabilities.
  tango_config_ = TangoService_getConfig(TANGO_CONFIG_DEFAULT);
  if (tango_config_ == nullptr) {
    LOGE("HandlerApp: Failed to get default config form");
    return TANGO_ERROR;
  }

  // Set auto-recovery for motion tracking as requested by the user.
  TangoErrorType ret = TangoConfig_setBool(tango_config_, "config_enable_auto_recovery", true);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: config_enable_auto_recovery() failed with error code: %d", ret);
    return ret;
  }

  // Enable depth.
  ret = TangoConfig_setBool(tango_config_, "config_enable_depth", true);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: config_enable_depth() failed with error code: %d", ret);
    return ret;
  }

  // Low latency IMU integration enables aggressive integration of the latest
  // inertial measurements to provide lower latency pose estimates. This will
  // improve the AR experience.
  ret = TangoConfig_setBool(tango_config_,"config_enable_low_latency_imu_integration", true);
  if (ret != TANGO_SUCCESS) {
    LOGE("AugmentedRealityApp: config_enable_low_latency_imu_integration() "
        "failed with error code: %d", ret);
    return ret;
  }

  // Enable color camera from config
  ret = TangoConfig_setBool(tango_config_, "config_enable_color_camera", true);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: config_enable_color_camera() failed with error" "code: %d",ret);
    return ret;
  }

  bool testCameraColor;
  ret = TangoConfig_getBool(tango_config_,"config_enable_color_camera", &testCameraColor);
  picture_data_.SetPictureDebugString("Connected to onFrameAvailable", ret, testCameraColor, ret == TANGO_SUCCESS);

  return ret;
}

int HandlerApp::TangoConnectCallbacks() {
  // Attach onEventAvailable callback.
  // The callback will be called after the service is connected.
  /*
  TangoErrorType ret = TangoService_connectOnTangoEvent(onTangoEventAvailableRouter);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: Failed to connect to event callback with error code: %d", ret);
    return ret;
  }

  // Attach the OnXYZijAvailable callback.
  // The callback will be called after the service is connected.
  ret = TangoService_connectOnXYZijAvailable(onPointCloudAvailableRouter);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: Failed to connect to point cloud callback with error code: %d", ret);
    return ret;
  }

  // Setting up the frame pair for the onPoseAvailable callback.
  TangoCoordinateFramePair pairs;
  pairs.base = TANGO_COORDINATE_FRAME_START_OF_SERVICE;
  pairs.target = TANGO_COORDINATE_FRAME_DEVICE;

  // Attach the onPoseAvailable callback.
  // The callback will be called after the service is connected.
  ret = TangoService_connectOnPoseAvailable(1, &pairs, onPoseAvailableRouter);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: Failed to connect to pose callback with error code: %d", ret);
    return ret;
  }
  */
  // Attach the onFrameAvailable callback.
  // The callback will be called after the service is connected.
  TangoErrorType ret = TangoService_connectOnFrameAvailable(TANGO_CAMERA_COLOR, NULL,onFrameAvailableRouter);
  if (ret != TANGO_SUCCESS) {
    LOGE("HandlerApp: Failed to connect to pose callback with error code: %d", ret);
    return ret;
  }

  return ret;
}

  // Connect to the Tango Service, the service will start running:
  // poses can be queried and callbacks will be called.
  bool HandlerApp::TangoConnect() {
    TangoErrorType err = TangoService_connect(this, tango_config_);
    if (err != TANGO_SUCCESS) {
      LOGE("HandlerApp: Failed to connect to the Tango service with error code: %d", err);
      return false;
    }

    err = UpdateExtrinsics();
    if (err != TANGO_SUCCESS) {
      LOGE("HandlerApp: Failed to query sensor extrinsic with error code: %d",
           err);
      return false;
    }
    return true;
  }

  void HandlerApp::TangoDisconnect() {
    // When disconnecting from the Tango Service, it is important to make sure to
    // free your configuration object. Note that disconnecting from the service,
    // resets all configuration, and disconnects all callbacks. If an application
    // resumes after disconnecting, it must re-register configuration and
    // callbacks with the service.
    TangoConfig_free(tango_config_);
    tango_config_ = nullptr;
    TangoService_disconnect();
  }

TangoErrorType HandlerApp::UpdateExtrinsics() {
    TangoErrorType ret;
    TangoPoseData pose_data;
    TangoCoordinateFramePair frame_pair;

    // TangoService_getPoseAtTime function is used for query device extrinsics
    // as well. We use timestamp 0.0 and the target frame pair to get the
    // extrinsics from the sensors.
    //
    // Get device with respect to imu transformation matrix.
    frame_pair.base = TANGO_COORDINATE_FRAME_IMU;
    frame_pair.target = TANGO_COORDINATE_FRAME_DEVICE;
    ret = TangoService_getPoseAtTime(0.0, frame_pair, &pose_data);
    if (ret != TANGO_SUCCESS) {
      LOGE(
          "HandlerApp: Failed to get transform between the IMU frame and "
          "device frames");
      return ret;
    }
    pose_data_.SetImuTDevice(pose_data_.GetMatrixFromPose(pose_data));

    // Get color camera with respect to imu transformation matrix.
    frame_pair.base = TANGO_COORDINATE_FRAME_IMU;
    frame_pair.target = TANGO_COORDINATE_FRAME_CAMERA_DEPTH;
    ret = TangoService_getPoseAtTime(0.0, frame_pair, &pose_data);
    if (ret != TANGO_SUCCESS) {
      LOGE(
          "HandlerApp: Failed to get transform between the color camera frame "
          "and device frames");
      return ret;
    }
    pose_data_.SetImuTDepthCamera(pose_data_.GetMatrixFromPose(pose_data));
    return ret;
  }

std::string HandlerApp::GetPoseString() {
  std::lock_guard<std::mutex> lock(pose_mutex_);
  return pose_data_.GetPoseDebugString();
}

std::string HandlerApp::GetPointCloudString() {
  std::lock_guard<std::mutex> lock(point_cloud_mutex_);
  return point_cloud_data_.GetPointCloudDebugString();
}

std::string HandlerApp::GetPictureString() {
  std::lock_guard<std::mutex> lock(picture_mutex_);
  return picture_data_.GetPictureDebugString();
}

std::string HandlerApp::GetEventString() {
  std::lock_guard<std::mutex> lock(tango_event_mutex_);
  return tango_event_data_.GetTangoEventString().c_str();
}

void HandlerApp::onTangoEventAvailable(const TangoEvent* event) {
  std::lock_guard<std::mutex> lock(tango_event_mutex_);
  tango_event_data_.UpdateTangoEvent(event);
}

void HandlerApp::onFrameAvailable(TangoCameraId id, const TangoImageBuffer* buffer) {
  std::lock_guard<std::mutex> lock(picture_mutex_);
  LOGI("OnFrameAvailable(): Cam frame data received");
  /*
  if((int)id == 0)//0 is the id of the CAMERA_COLOR, and the TangoCameraId id is given in hexa
    picture_data_.UpdatePicture(buffer);
    picture_data_.SetPictureDebugString("I was in onFrameAvailable ", 1, true);
    */
}

TangoErrorType HandlerApp::SetupConfigB() {
  // TANGO_CONFIG_DEFAULT is enabling Motion Tracking and disabling Depth
  // Perception.
  tango_config_ = TangoService_getConfig(TANGO_CONFIG_DEFAULT);
  if (tango_config_ == nullptr) {
  return TANGO_ERROR;
  }
  TangoConfig_setBool(tango_config_,"config_enable_color_camera",true);
  return TANGO_SUCCESS;
}

TangoErrorType HandlerApp::ConnectYUVFrameCallbackB() {
    TangoErrorType onFrameErrorType=TangoService_connectOnFrameAvailable( TANGO_CAMERA_COLOR, NULL, onFrameAvailableB);
    if( onFrameErrorType!= TANGO_SUCCESS)
    {
         LOGI("GOOGLE TANGO ONFRAMEAVAILABLE FAILED!");
    }
    LOGI("GOOGLE TANGO ONFRAMEAVAILABLE SUCCESS!");
    return onFrameErrorType;
}

void HandlerApp::frameProcess(const TangoImageBuffer* buffer){
  int width = buffer->width;
  int height = buffer->height;
  double time = buffer->timestamp;
  TangoImageFormatType format = buffer->format;
  LOGI("width and height is: %d,%d",width,height);
  LOGI("Timestamp: %g",time);
  LOGI("6 premieres val : %d,%d,%d,%d,%d,%d",buffer->data[0],buffer->data[1],buffer->data[2],buffer->data[3],buffer->data[4],buffer->data[5]);
  LOGI("Format : %d", format);
}

}  // namespace tango_handler_app
