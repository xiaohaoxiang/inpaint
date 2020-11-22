#ifndef UTILS_H
#define UTILS_H

#include <unordered_set>
#include <opencv2/core/mat.hpp>
#include "defs.hpp"

std::unordered_set<P2i, P2i::hasher> getPixs(const cv::Mat& src, const std::vector<P2i>& dmgv);

#endif // !UTILS_H
