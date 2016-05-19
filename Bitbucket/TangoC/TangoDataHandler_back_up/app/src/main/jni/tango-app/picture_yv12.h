/*
 * Added by Bastien
 * Used to access to to the pixel data (HAL_PIXEL_FORMAT_YV12)
 * It's not recommended for display but it can useful to get the camera data
 * /!\ this is NOT RGB or RGBD !
 * For some details see https://developers.google.com/project-tango/apis/c/reference/group/camera#tangoservice_connectonframeavailable
 */

#ifndef TANGO_PICTURE_YV12_H
#define TANGO_PICTURE_YV12_H

#include <jni.h>
#include <vector>

#include <mutex>
#include <string>

#include <tango_client_api.h>
#include <tango-gl/util.h>

namespace tango_handler_app {

class PictureDataYV12 {
 public:
  PictureDataYV12() {}
  ~PictureDataYV12() {}

  // @return the delta time (in milliseconds) between the current frame
  // and the previous frame.
  float GetFrameDeltaTime();

  // Return the current frame's timstamp. The timestamp is used for
  // querying the frame's pose using the TangoService_getPoseAtTime
  // function, so we return the data in double type.
  //
  // @return current frame's timstamp.
  double GetCurrentTimstamp();

  // @return the wifth of the picture
  int GetWidth();

  // @return the heigth of the picture
  int GetHeigth();

  // @return the number of pixels per scanline of image data
  int GetStride();

  // @return the frame number
  int GetFrameNumber();

  // /!\ the format is convert in float, the value of format in hexa :
  //TANGO_HAL_PIXEL_FORMAT_RGBA_8888 = 1,       ///< RGBA 8888
  //TANGO_HAL_PIXEL_FORMAT_YV12 = 0x32315659,   ///< YV12
  //TANGO_HAL_PIXEL_FORMAT_YCrCb_420_SP = 0x11  ///< NV21
  // @return the format of the picture
  float GetFormat();

  // @return the vector of pixel.
  const std::vector<float>& GetVerticeVector() { return pixel_; }

  // Update current point cloud data.
  //
  // @param point_cloud: buffer data of the current camera.
  void UpdatePicture(const TangoImageBuffer* buffer);

  // Compose the picture debug string.
  //
  // @return: point cloud debug strings for dispaly on Java activity.
  std::string GetPictureDebugString();

  void SetPictureDebugString(std::string s, int errorInt , bool errorBool);

 private:
  // Timestamp of current depth frame.
  double cur_frame_timstamp_;

  // Previous depth frame's timestamp for computing the delta_timestamp_.
  double prev_frame_timestamp_;

  // delta_timestamp_ is for debug purposes.
  double delta_timestamp_;

  // The width of the image data.
  int width_;

  // The height of the image data.
  int height_;

  // The number of pixels per scanline of image data.
  int stride_;

  // The frame number of this image.
  int frame_number_;

  //The pixel of the picture
  std::vector<float> pixel_;

  // Pixel format of data.
  TangoImageFormatType format_;

  // Debug cloud point string.
  std::string picture_string_ = "init";

  // Format the cloud point debug string.
  void FormatPictureString();

};
}  // namespace tango_handler_app

#endif  // TANGO_PICTURE_YV12_H
