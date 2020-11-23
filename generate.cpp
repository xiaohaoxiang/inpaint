#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include <opencv2/imgproc.hpp>
#include "defs.hpp"
#include "generate.h"
#include "utils.h"

namespace Mask
{
	cv::Mat_<uchar> getMask(const cv::Size sz, const MaskType maskType, double degree)
	{
		cv::Mat_<uchar> mask(sz, 0);
		const unsigned pixcnt = unsigned(sz.width) * unsigned(sz.height);
		degree = std::min<double>(degree, pixcnt - 1);
		std::default_random_engine gen{ std::random_device{}() };
		std::uniform_int_distribution<int> rdx(0, sz.width), rdy(0, sz.height);
		switch (maskType)
		{
		case MaskType::Point:
		{
			std::vector<unsigned> pv(pixcnt);
			for (unsigned i = 0; i < pixcnt; i++)
				pv[i] = i;
			std::shuffle(pv.begin(), pv.end(), gen);
			for (unsigned i = 0; i < degree; i++)
				mask.at<uchar>(pv[i] % sz.height, pv[i] / sz.height) = 255;
			break;
		}
		case MaskType::Line:
		{
			for (double sum = degree; sum >= 0;)
			{
				P2i p1(rdx(gen), rdy(gen)), p2(rdx(gen), rdy(gen));
				sum -= std::sqrt(P2i(p1 - p2).dist2());
				cv::line(mask, p1, p2, cv::Scalar_<uchar>(255));
			}
			break;
		}
		case MaskType::Curve:
		{
			cv::Point2d p0(rdx(gen), rdy(gen)), p1(rdx(gen), rdy(gen)), p3;
			for (double sum = degree; sum >= 0;)
			{
				p3 = cv::Point2d(rdx(gen), rdy(gen));
				cv::Point2d p2 = p1 + 0.5 * (p3 - p1), d0 = p1 - p0, d1 = p2 - p1, cur = p0;
				const double dt = 4.0 / (double(std::abs(d0.x)) + std::abs(d0.y) + std::abs(d1.x) + std::abs(d1.y) + 1);
				for (double t = dt; t < 1.0; t += dt)
				{
					cv::Point2d nxt = pow2(1 - t) * p0 + 2 * t * (1 - t) * p1 + t * t * p2;
					sum -= std::sqrt(pow2((nxt - cur).x) + pow2((nxt - cur).y));
					cv::line(mask, cur, nxt, cv::Scalar_<uchar>(255));
					cur = nxt;
				}
				p0 = p2;
				p1 = p3;
			}
			break;
		}
		case MaskType::Text:
		{
			static const char chrs[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
			int chcnt, chh;
			{
				int bl;
				auto ssz = cv::getTextSize(chrs, cv::FONT_HERSHEY_COMPLEX, 1, 1, &bl);
				chcnt = 2 * sizeof(chrs) * sz.width / ssz.width;
				chh = ssz.height + bl;
			}
			for (int i = sz.height + chh + 10; i >= -chh; i -= chh)
			{
				std::string txt;
				for (int j = 0; j < chcnt; j++)
					txt.push_back(chrs[gen() % (sizeof(chrs) - 1)]);
				cv::putText(mask, txt, cv::Point(-chh, i), cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar_<uchar>(255));
			}
			break;
		}
		}
		return mask;
	}

	cv::Mat_<uchar> damage(cv::Mat& mat, const MaskType maskType, const double degree)
	{
		cv::Mat_<uchar> mask = getMask(mat.size(), maskType, degree);
		doDamage(mat, mask);
		return mask;
	}
}
