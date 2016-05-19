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

#include <sstream> //added by Bastien
#include "tango-app/point_cloud_data.h"

namespace {
const float kSecondToMillisecond = 1000.0f;
}  // namespace

namespace tango_handler_app {

int PointCloudData::GetPointCloudVerticesCount() { return vertices_count_; }

float PointCloudData::GetAverageDepth() { return average_depth_; }

void PointCloudData::SetAverageDepth(float average_depth) {
  average_depth_ = average_depth;
}

float PointCloudData::GetDepthFrameDeltaTime() {
  return static_cast<float>(delta_timestamp_) * kSecondToMillisecond;
}

double PointCloudData::GetCurrentTimstamp() { return cur_frame_timstamp_; }

void PointCloudData::UpdatePointCloud(const TangoXYZij* point_cloud) {
  size_t point_cloud_size = point_cloud->xyz_count * 3;
  vertices_.resize(point_cloud_size);
  std::copy(point_cloud->xyz[0], point_cloud->xyz[0] + point_cloud_size,
            vertices_.begin());

  // Get current frame's point count.
  vertices_count_ = point_cloud->xyz_count;

  // Compute the frame delta time.
  cur_frame_timstamp_ = point_cloud->timestamp;
  delta_timestamp_ = cur_frame_timstamp_ - prev_frame_timestamp_;

  // Set current timestamp to previous timestamp.
  prev_frame_timestamp_ = point_cloud->timestamp;
  FormatPointCloudString();
}

std::string PointCloudData::GetPointCloudDebugString() { return point_cloud_string_; }

void PointCloudData::FormatPointCloudString() {
  std::stringstream string_stream;
  string_stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
  string_stream.precision(3);
  string_stream << ", number of point: " << vertices_count_
                  << ", delta time (ms): " << delta_timestamp_ * kSecondToMillisecond
                  << ", point coordinates (m): ";

  for(unsigned int u=0; u<vertices_count_*3;u++ ){
    float value = vertices_[u];

    switch (u%3){
      case 0:
        string_stream <<"[" << value ; break;
      case 1:
        string_stream <<", " << value ; break;
      case 2:
        string_stream <<", " << value << "] " ; break;
      default:break;
     }
  }
  point_cloud_string_ = string_stream.str();
  string_stream.flush();
}

}  // namespace tango_handler_app
