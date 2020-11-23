#include <opencv2/core/mat.hpp>
#include "utils.h"

extern const P2i neighbor[8] = { {-1,0},{0,-1},{0,1},{1,0}, {-1,-1},{-1,1},{1,-1},{1,1} };

std::unordered_set<P2i, P2i::hasher> getPixs(const cv::Mat& src, const std::vector<P2i>& dmgv)
{
	std::unordered_set<P2i, P2i::hasher> pixs;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			pixs.insert(P2i(i, j));
		}
	}
	for (auto& p : dmgv)
	{
		pixs.erase(p);
	}
	return pixs;
}

std::vector<P2i> getDmgv(const cv::Mat_<uchar>& src)
{
	std::vector<P2i> dmgv;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<uchar>(i, j))
				dmgv.push_back(P2i(i, j));
		}
	}
	return dmgv;
}

void doDamage(cv::Mat& mat, const cv::Mat_<uchar> mask)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			if (mask.at<uchar>(i, j))
				mat.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
		}
	}
}

void doDamage(cv::Mat& mat, const std::vector<P2i>& dmgv)
{
	for (auto& p : dmgv)
		mat.at<cv::Vec3b>(p.x, p.y) = cv::Vec3b(0, 0, 0);
}
