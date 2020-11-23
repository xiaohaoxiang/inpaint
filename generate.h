#ifndef GENERATE_H
#define GENERATE_H

#include "defs.hpp"

enum class MaskType :unsigned
{
	Point,
	Line,
	Curve,
	Text
};

namespace Mask
{
	cv::Mat_<uchar> getMask(const cv::Size sz, const MaskType maskType, double degree = 0);
	cv::Mat_<uchar> damage(cv::Mat& mat, const MaskType maskType, const double degree = 0);
}

#endif // !GENERATE_H
