#include <vector>
#include <opencv2/core.hpp>
#include <Eigen/Sparse>
#include "defs.hpp"
#include "kdtree.hpp"
#include "utils.h"

extern const P2i neighbor[];

void bilinearEquation(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, const float tolerance)
{
	std::vector<std::vector<int>> dmgm(src.rows, std::vector<int>(src.cols, -1));
	for (auto it = dmgv.begin(); it != dmgv.end(); ++it)
	{
		dmgm[it->x][it->y] = int(it - dmgv.begin());
	}
	src.copyTo(dst);
	std::vector<cv::Mat> channels;
	cv::split(src, channels);
	for (auto it = dmgv.begin(); it != dmgv.end(); ++it)
	{
		dmgm[it->x][it->y] = int(it - dmgv.begin());
	}
	for (auto ccIt = channels.cbegin(); ccIt != channels.cend(); ++ccIt)
	{
		Eigen::SparseMatrix<float> A(dmgv.size(), dmgv.size());
		Eigen::VectorXf B(dmgv.size());
		for (size_t i = 0; i < dmgv.size(); i++)
			B[i] = 0;
		std::vector<Eigen::Triplet<float>> triplev;

		B.resize(dmgv.size());

		for (auto& p : dmgv)
		{
			const int& cur = dmgm[p.x][p.y];
			triplev.push_back(Eigen::Triplet<float>(cur, cur, 1));
			float w = 1.f / (4.f - (p.x == 0 || p.x + 1 == src.rows) - (p.y == 0 || p.y + 1 == src.cols));
			for (int k = 0; k < 4; k++)
			{
				P2i nb(p.x + neighbor[k].x, p.y + neighbor[k].y);
				if (nb.x < 0 || nb.y < 0 || nb.x >= src.rows || nb.y >= src.cols)
					continue;
				const int& nn = dmgm[nb.x][nb.y];
				if (nn == -1)
				{
					B[cur] += w * ccIt->at<uchar>(nb.x, nb.y);
				}
				else
				{
					triplev.push_back(Eigen::Triplet<float>(cur, nn, -w));
				}
			}
		}

		A.setFromTriplets(triplev.begin(), triplev.end());
		A.makeCompressed();
		Eigen::LeastSquaresConjugateGradient<Eigen::SparseMatrix<float>> solver{ A };
		solver.setTolerance(tolerance);
		Eigen::VectorXf X = solver.solve(B);
		int cc = int(ccIt - channels.cbegin());
		for (size_t i = 0; i < dmgv.size(); i++)
		{
			auto& p = dmgv[i];
			dst.at<cv::Vec3b>(p.x, p.y)[cc] = uchar(X[i] + 0.5f);
		}
	}
}

void bilinearKd(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv, const int k)
{
	auto pixs = getPixs(src, dmgv);
	Kdtree<double> kd(pixs.begin(), pixs.end());
	src.copyTo(dst);
	for (const auto& p : dmgv)
	{
		auto t = kd.query(k, p);
		double sum = 0;
		for (const auto& i : t)
		{
			sum += 1.0 / i.first;
		}
		double color[3]{ 0,0,0 };
		for (const auto& i : t)
		{
			auto& cur = src.at<cv::Vec3b>(i.second.x, i.second.y);
			for (int ch = 0; ch < 3; ch++)
			{
				color[ch] += cur[ch] / (i.first * sum);
			}
		}
		dst.at<cv::Vec3b>(p.x, p.y) << uchar(color[0] + 0.5), uchar(color[1] + 0.5), uchar(color[2] + 0.5);
	}
}
