#include <vector>
#include <opencv2/photo.hpp>
#include "defs.hpp"

void fastMarching(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, double r)
{
	cv::Mat_<uchar> mask = cv::Mat_<uchar>::zeros(src.size());
	for (auto& p : dmgv)
	{
		mask.at<uchar>(p.x, p.y) = '\1';
	}
	cv::inpaint(src, mask, dst, r, cv::INPAINT_TELEA);
}
