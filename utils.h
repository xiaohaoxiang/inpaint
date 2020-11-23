#ifndef UTILS_H
#define UTILS_H

#include <unordered_set>
#include <vector>
#include <opencv2/core/types.hpp>
#include "defs.hpp"

std::unordered_set<P2i, P2i::hasher> getPixs(const cv::Mat& src, const std::vector<P2i>& dmgv);
std::vector<P2i> getDmgv(const cv::Mat_<uchar>& src);
void doDamage(cv::Mat& mat, const cv::Mat_<uchar> mask);
void doDamage(cv::Mat& mat, const std::vector<P2i>& dmgv);

#endif // !UTILS_H
