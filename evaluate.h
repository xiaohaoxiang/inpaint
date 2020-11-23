#ifndef EVALUATE_H
#define EVALUATE_H

#include <opencv2/core/types.hpp>

double MSE(const cv::Mat& I1, const cv::Mat& I2);
double PSNR(const cv::Mat& I1, const cv::Mat& I2);
double MSSIM(const cv::Mat& i1, const cv::Mat& i2, const float C1 = 6.5025, const float C2 = 58.5225);

#endif // !EVALUATE_H
