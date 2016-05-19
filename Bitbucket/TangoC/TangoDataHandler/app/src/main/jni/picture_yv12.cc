/*
 * Added by Bastien
 * Used to access to to the pixel data (HAL_PIXEL_FORMAT_YV12)
 * It's not recommended for display but it can useful to get the camera data
 * /!\ this is NOT RGB or RGBD !
 * For some details see https://developers.google.com/project-tango/apis/c/reference/group/camera#tangoservice_connectonframeavailable
 */

#include <sstream>
#include "tango-app/picture_yv12.h"

namespace {
const float kSecondToMillisecond = 1000.0f;
}  // namespace

namespace tango_handler_app {

float PictureDataYV12::GetFrameDeltaTime() {
  return static_cast<float>(delta_timestamp_) * kSecondToMillisecond;
}

double PictureDataYV12::GetCurrentTimstamp() { return cur_frame_timstamp_; }

int PictureDataYV12::GetWidth() { return width_; }

int PictureDataYV12::GetHeigth() { return height_; }

int PictureDataYV12::GetStride() { return stride_; }

int PictureDataYV12::GetFrameNumber() { return frame_number_; }

float PictureDataYV12::GetFormat() { return (float) format_; }

void PictureDataYV12::UpdatePicture(const TangoImageBuffer* buffer) {
  width_ = buffer->width;
  height_ = buffer->height;
  cur_frame_timstamp_ = buffer->timestamp;
  stride_ = buffer->stride;
  frame_number_ = buffer->frame_number;
  format_ = buffer->format;
  size_t data_size = width_*height_;

  pixel_.resize(data_size);
  //std::copy(buffer->data[0], buffer->data[0] + data_size, pixel_.begin());
  for(unsigned int it=0;it<data_size;){
    pixel_[it++] = buffer->data[it++];
  }

  cur_frame_timstamp_ = buffer->timestamp;
  delta_timestamp_ = cur_frame_timstamp_ - prev_frame_timestamp_;

  // Set current timestamp to previous timestamp.
  prev_frame_timestamp_ = buffer->timestamp;

}

std::string PictureDataYV12::GetPictureDebugString() {
  FormatPictureString();
  return picture_string_;
}

void PictureDataYV12::SetPictureDebugString(std::string s, int errorInt , bool errorBool, bool errorComp){
  std::stringstream string_stream;
  string_stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
  string_stream.precision(3);
  string_stream << s << " ==> int error = " << errorInt << " // bit = " << errorBool << " // ret = success : " << errorComp;
  picture_string_ = string_stream.str();
  string_stream.flush();
}

void PictureDataYV12::FormatPictureString() {
  std::stringstream string_stream;
  string_stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
  string_stream.precision(3);
  string_stream << ", picture size: " << width_ << "x" << height_
                  << ", delta time (ms): " << delta_timestamp_ * kSecondToMillisecond
                  << ", frame number " << frame_number_
                  << ", pixel: [";

  for(unsigned int u=0; u>width_*height_;u++){
    float value = pixel_[u];
    if(u==width_*height_ -1)
      string_stream << value << "]";
    else
      string_stream << value << ", ";
  }
  picture_string_ = string_stream.str();
  string_stream.flush();
}

}  // namespace tango_handler_app
