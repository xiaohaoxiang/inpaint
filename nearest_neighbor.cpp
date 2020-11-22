#include <algorithm>
#include <queue>
#include <unordered_set>
#include <vector>
#include <opencv2/core/mat.hpp>
#include "defs.hpp"
#include "kdtree.hpp"
#include "utils.h"

extern const P2i neighbor[];

void nearestNeighborBfs(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv)
{
	std::unordered_set<P2i, P2i::hasher> dmgs(dmgv.begin(), dmgv.end());
	std::vector<std::vector<int>> dist(src.rows, std::vector<int>(src.cols, std::numeric_limits<int>::max()));
	std::vector<P2i> used;
	src.copyTo(dst);
	for (auto& s : dmgv)
	{
		for (const auto& p : used)
		{
			dist[p.x][p.y] = std::numeric_limits<int>::max();
		}
		used.clear();

		struct State :public P2i
		{
			int d;
			State(int x = 0, int y = 0, int d = 0) :P2i(x, y), d(d) {}
			bool operator< (const State& a)const
			{
				if (d == a.d)
				{
					auto s1 = x + y, s2 = a.x + a.y;
					return s1 < s2 || s1 == s2 && x < a.x;
				}
				else
				{
					return d > a.d;
				}
			}
		};
		std::priority_queue<State> pq;
		pq.push(State(s.x, s.y, 0));
		dist[s.x][s.y] = 0;
		used.push_back(s);
		while (!pq.empty())
		{
			State cur = pq.top();
			pq.pop();
			if (cur.d > dist[cur.x][cur.y])
				continue;
			else if (dmgs.find(cur) == dmgs.end())
			{
				dst.at<cv::Vec3b>(s.x, s.y) = src.at<cv::Vec3b>(cur.x, cur.y);
				break;
			}
			for (int i = 0; i < 4; i++)
			{
				State nxt(cur.x + neighbor[i].x, cur.y + neighbor[i].y);
				if (nxt.x < 0 || nxt.y < 0 || nxt.x >= src.rows || nxt.y >= src.cols)
					continue;
				nxt.d = pow2(nxt.x - s.x) + pow2(nxt.y - s.y);
				int& d = dist[nxt.x][nxt.y];
				if (nxt.d < d)
				{
					d = nxt.d;
					used.push_back(nxt);
					pq.push(nxt);
				}
			}
		}
	}
}

void nearestNeighborRbfs(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv)
{
	std::unordered_set<P2i, P2i::hasher> dmgs(dmgv.begin(), dmgv.end());
	std::vector<P2i> borderv;
	for (auto& p : dmgs)
	{
		for (int i = 0; i < 8; i++)
		{
			P2i nb = p + neighbor[i];
			if (nb.x < 0 || nb.x >= src.rows || nb.y < 0 || nb.y >= src.cols)
				continue;
			if (dmgs.find(nb) == dmgs.end())
			{
				borderv.push_back(nb);
			}
		}
	}
	auto sort_unique = [&borderv]() {
		std::sort(borderv.begin(), borderv.end(), [](const P2i& a, const P2i& b) {
			int sa = a.x + a.y, sb = b.x + b.y;
			return sa > sb || sa == sb && a.x > b.x;
			});
		borderv.resize(std::unique(borderv.begin(), borderv.end()) - borderv.begin());
	};
	src.copyTo(dst);
	sort_unique();
	for (;;)
	{
		std::vector<P2i> repaired;
		for (const auto& p : borderv)
		{
			for (int i = 0; i < 8; i++)
			{
				P2i nb = p + neighbor[i];
				if (nb.x < 0 || nb.x >= src.rows || nb.y < 0 || nb.y >= src.cols)
					continue;
				if (dmgs.find(nb) != dmgs.end())
				{
					repaired.push_back(nb);
					dst.at<cv::Vec3b>(nb.x, nb.y) = dst.at<cv::Vec3b>(p.x, p.y);
				}
			}
		}
		if (borderv.empty())
			break;
		borderv.swap(repaired);
		sort_unique();
		for (const auto& p : borderv)
		{
			dmgs.erase(p);
		}
	}
}

void nearestNeighborKd(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv)
{
	auto pixs = getPixs(src, dmgv);
	Kdtree<double> kd(pixs.begin(), pixs.end());
	src.copyTo(dst);

	for (auto& p : dmgv)
	{
		P2i t = kd.query(p);
		dst.at<cv::Vec3b>(p.x, p.y) = src.at<cv::Vec3b>(t.x, t.y);
	}
}
