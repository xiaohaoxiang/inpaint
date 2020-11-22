#ifndef DEFS_HPP
#define DEFS_HPP

#include <cstddef>
#include <functional>
#include <opencv2/core/types.hpp>

struct P2i :public cv::Point2i
{
	using cv::Point2i::Point_;
	P2i(const cv::Point2i& p) :cv::Point2i(p) {}
	struct hasher
	{
		size_t operator()(const P2i& p)const
		{
			return std::hash<size_t>()(*reinterpret_cast<const size_t*>(&p));
		}
	};
	bool operator==(const P2i& p)const
	{
		return *reinterpret_cast<const size_t*>(this) == *reinterpret_cast<const size_t*>(&p);
	}
	bool operator<(const P2i& p)const
	{
		return x < p.x || x == p.x && y < p.y;
	}
};
static_assert(sizeof(P2i) == sizeof(size_t), "sizeof(P2i) == sizeof(size_t)");

template<typename T>
inline T pow2(T x)
{
	return x * x;
}

#endif // !DEFS_HPP
