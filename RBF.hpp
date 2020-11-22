#ifndef RBF_HPP
#define RBF_HPP

#include <cmath>
#include <limits>
#include <vector>
#include "defs.hpp"
#include "kdtree.hpp"
#include "utils.h"

struct Gaussian
{
	double operator()(double x)const
	{
		return std::exp(-x);
	}
};

struct InverseQuadratic
{
	double operator()(double x)const
	{
		return 1.0 / (1.0 + x * x);
	}
};

struct InverseMultiquadratic
{
	double operator()(double x)const
	{
		return 1.0 / std::sqrt(1.0 + x * x);
	}
};

template<typename Func = Gaussian>
void RBF(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, Func func = Func(), const double tolerance = 1E-8)
{
	std::vector<double> tab{ 0 };
	const int maxR = pow2(src.rows) + pow2(src.cols);
	int R = 1;
	for (double val = std::numeric_limits<double>::max(); R < maxR && val >= tolerance; R++)
	{
		val = func(R);
		tab.push_back(val);
	}
	auto pixs = getPixs(src, dmgv);
	Kdtree<int> kd(pixs.begin(), pixs.end());
	src.copyTo(dst);
	for (auto& p : dmgv)
	{
		auto t = kd.queryR(R, p);
		double color[3]{ 0,0,0 }, sum = 0;
		if (t.empty())
		{
			t = kd.query(8, p);
			int maxsz = 0;
			for (const auto& i : t)
				maxsz = std::max(maxsz, i.first);
			for (int k = int(tab.size()); k <= maxsz; k++)
				tab.push_back(func(k));
		}
		for (const auto& i : t)
			sum += tab[i.first];
		sum = 1.0 / sum;
		for (const auto& i : t)
		{
			auto& cc = src.at<cv::Vec3b>(i.second.x, i.second.y);
			double w = tab[i.first] * sum;
			for (int ch = 0; ch < 3; ch++)
			{
				color[ch] += cc[ch] * w;
			}
		}
		dst.at<cv::Vec3b>(p.x, p.y) << uchar(color[0] + 0.5), uchar(color[1] + 0.5), uchar(color[2] + 0.5);
	}
}

#endif // !RBF_HPP
