#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <vector>
#include "defs.hpp"

void nearestNeighborBfs(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv);
void nearestNeighborRbfs(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv);
void nearestNeighborKd(const cv::Mat& src, cv::Mat& dst, const std::vector<P2i>& dmgv);

#endif // !NEAREST_NEIGHBOR
