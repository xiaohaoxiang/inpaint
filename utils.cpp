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