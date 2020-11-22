#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <utility>
#include <vector>
#include "defs.hpp"

using P = P2i;

template<typename T = double>
class Kdtree
{
private:
	std::vector<P> S;
	std::vector<int> lc, rc;
	std::vector<T> L, R, D, U;

	inline T dist2(const P& a, int b)const
	{
		T dx = a.x - S[b].x, dy = a.y - S[b].y;
		return dx * dx + dy * dy;
	}

	void maintain(const int x)
	{
		L[x] = R[x] = S[x].x;
		D[x] = U[x] = S[x].y;
		if (lc[x])
			L[x] = std::min(L[x], L[lc[x]]), R[x] = std::max(R[x], R[lc[x]]),
			D[x] = std::min(D[x], D[lc[x]]), U[x] = std::max(U[x], U[lc[x]]);
		if (rc[x])
			L[x] = std::min(L[x], L[rc[x]]), R[x] = std::max(R[x], R[rc[x]]),
			D[x] = std::min(D[x], D[rc[x]]), U[x] = std::max(U[x], U[rc[x]]);
	}

	int build(const int left, const int right)
	{
		if (left >= right)
		{
			L[left] = R[left] = S[left].x;
			D[left] = U[left] = S[left].y;
			return left;
		}

		int mid = (left + right) >> 1;
		double avx = 0, avy = 0, vax = 0, vay = 0;
		for (int i = left; i <= right; i++)
		{
			avx += S[i].x;
			avy += S[i].y;
		}
		avx /= right - left + 1;
		avy /= right - left + 1;
		for (int i = left; i <= right; i++)
		{
			vax += pow2(S[i].x - avx);
			vay += pow2(S[i].y - avy);
		}
		if (vax < vay)
		{
			std::nth_element(S.begin() + left, S.begin() + mid, S.begin() + right + 1, [](const P& a, const P& b) {return a.y < b.y; });
		}
		else
		{
			std::nth_element(S.begin() + left, S.begin() + mid, S.begin() + right + 1, [](const P& a, const P& b) {return a.x < b.x; });
		}
		lc[mid] = build(left, mid - 1);
		rc[mid] = build(mid + 1, right);
		maintain(mid);
		return mid;
	}

	T mindist2(const P& a, int b)const
	{
		T res = 0;
		if (L[b] > a.x)
			res += pow2(L[b] - a.x);
		else if (R[b] < a.x)
			res += pow2(R[b] - a.x);
		if (D[b] > a.y)
			res += pow2(D[b] - a.y);
		else if (U[b] < a.y)
			res += pow2(U[b] - a.y);
		return res;
	}

	void query(T& ans, int& tar, const int left, const int right, const P& x)const
	{
		if (left > right)
			return;
		int mid = (left + right) >> 1;

		auto tmp = dist2(x, mid);
		if (tmp < ans)
		{
			ans = tmp;
			tar = mid;
		}
		if (left == right)
			return;
		T dl = mindist2(x, lc[mid]), dr = mindist2(x, rc[mid]);
		if (dl < ans && dr < ans)
		{
			if (dl < dr)
			{
				query(ans, tar, left, mid - 1, x);
				if (dr < ans)
					query(ans, tar, mid + 1, right, x);
			}
			else
			{
				query(ans, tar, mid + 1, right, x);
				if (dl < ans)
					query(ans, tar, left, mid - 1, x);
			}
		}
		else if (dl < ans)
		{
			query(ans, tar, left, mid - 1, x);
		}
		else if (dr < ans)
		{
			query(ans, tar, mid + 1, right, x);
		}
	}

	void query(std::priority_queue<std::pair<T, int>>& ans, const int left, const int right, const P& x)const
	{
		if (left > right)
			return;
		int mid = (left + right) >> 1;

		auto tmp = dist2(x, mid);
		if (tmp < ans.top().first)
		{
			ans.pop();
			ans.push(std::make_pair(tmp, mid));
		}
		if (left == right)
			return;
		T dl = mindist2(x, lc[mid]), dr = mindist2(x, rc[mid]);
		if (dl < ans.top().first && dr < ans.top().first)
		{
			if (dl < dr)
			{
				query(ans, left, mid - 1, x);
				if (dr < ans.top().first)
					query(ans, mid + 1, right, x);
			}
			else
			{
				query(ans, mid + 1, right, x);
				if (dl < ans.top().first)
					query(ans, left, mid - 1, x);
			}
		}
		else if (dl < ans.top().first)
		{
			query(ans, left, mid - 1, x);
		}
		else if (dr < ans.top().first)
		{
			query(ans, mid + 1, right, x);
		}
	}

	void query(std::vector<std::pair<T, P>>& ans, const T R, const int left, const int right, const P& x)const
	{
		if (left > right)
			return;
		int mid = (left + right) >> 1;
		auto tmp = dist2(x, mid);
		if (tmp < R)
		{
			ans.push_back(std::make_pair(tmp, S[mid]));
		}
		if (left == right)
			return;
		if (mindist2(x, lc[mid]) < R)
			query(ans, R, left, mid - 1, x);
		if (mindist2(x, rc[mid]) < R)
			query(ans, R, mid + 1, right, x);
	}

public:
	Kdtree() = default;
	~Kdtree() = default;
	Kdtree(const Kdtree&) = default;
	Kdtree(Kdtree&&) = default;
	Kdtree& operator=(const Kdtree&) = default;
	Kdtree& operator=(Kdtree&&) = default;

	template<typename It>
	Kdtree(It first, It last)
	{
		S.resize(1);
		S.insert(S.end(), first, last);
		lc.resize(S.size());
		rc.resize(S.size());
		L.resize(S.size());
		R.resize(S.size());
		D.resize(S.size());
		U.resize(S.size());
		build(1, int(S.size()) - 1);
	}

	P query(const P& p)const
	{
		T ans = std::numeric_limits<T>::max();
		int tar = 0;
		query(ans, tar, 1, int(S.size() - 1), p);
		return S[tar];
	}

	std::vector<std::pair<T, P>> query(int k, const P& p)const
	{
		std::priority_queue<std::pair<T, int>> ans;
		for (int i = 0; i < k; i++)
		{
			ans.push(std::make_pair(std::numeric_limits<T>::max(), 0));
		}
		query(ans, 1, int(S.size() - 1), p);
		std::vector<std::pair<T, P>> res;
		for (int i = 0; i < k; i++)
		{
			res.push_back(std::make_pair(ans.top().first, S[ans.top().second]));
			ans.pop();
		}
		return res;
	}

	std::vector<std::pair<T, P>> queryR(T r, const P& p)const
	{
		std::vector<std::pair<T, P>> ans;
		query(ans, r, 1, int(S.size() - 1), p);
		return ans;
	}
};

#endif // !KDTREE_HPP
