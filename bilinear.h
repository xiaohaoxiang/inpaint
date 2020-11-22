#ifndef BILINEAR_H
#define BILINEAR_H

#include <vector>
#include <opencv2/core/mat.hpp>
#include "defs.hpp"

void bilinearEquation(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, const float tolerance = 1E-4);
void bilinearKd(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, const int k = 4);

#endif // !BILINEAR_H
