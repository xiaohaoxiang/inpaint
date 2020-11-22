#ifndef FAST_MARCHING
#define FAST_MARCHING

#include <vector>
#include "defs.hpp"

void fastMarching(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, double r = 5);

#endif // !FAST_MARCHING
