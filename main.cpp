#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "bilinear.h"
#include "evaluate.h"
#include "fast_marching.h"
#include "generate.h"
#include "nearest_neighbor.h"
#include "utils.h"
#include "RBF.hpp"

#define InitMask(MT)                                                                                \
	do                                                                                              \
	{                                                                                               \
		mask##MT[i] = Mask::getMask(sz, MaskType::MT, deg);                                         \
		dmgv##MT[i] = getDmgv(mask##MT[i]);                                                         \
		const std::string maskFileName = std::string("mask-") + #MT + "-" + std::to_string(i + 1);  \
		cv::imwrite(outputDir + #MT + "/" + maskFileName + ".png", mask##MT[i]);                    \
		outstr << maskFileName << " missing pixels:" << cv::countNonZero(mask##MT[i]) << std::endl; \
	} while (false)

#define _OutPut(MT)                                                                                  \
	do                                                                                               \
	{                                                                                                \
		cv::imwrite(outputDir + #MT + "/" + curPicName + ".png", picRepaired);                       \
		outstr << curPicName << " time:" << t << " MSE:" << MSE(picRepaired, picRaw) << " PSNR:"     \
			   << PSNR(picRepaired, picRaw) << " MSSIM:" << MSSIM(picRepaired, picRaw) << std::endl; \
		std::cout << curPicName << " finished" << std::endl;                                         \
	} while (false)

#define _TimingRun1(FuncName, MT, FileName)                                                                                             \
	do                                                                                                                                  \
	{                                                                                                                                   \
		clock_t t = clock();                                                                                                            \
		FuncName(picDamaged, picRepaired, dmgv##MT[i]);                                                                                 \
		t = clock() - t;                                                                                                                \
		const std::string curPicName = std::string("repaired-") + #MT + "-" + std::to_string(i + 1) + "-" + #FuncName + "-" + FileName; \
		_OutPut(MT);                                                                                                                    \
	} while (false)

#define _TimingRun2(FuncName, F, MT, FileName)                                                                                                     \
	do                                                                                                                                             \
	{                                                                                                                                              \
		clock_t t = clock();                                                                                                                       \
		FuncName<F>(picDamaged, picRepaired, dmgv##MT[i]);                                                                                         \
		t = clock() - t;                                                                                                                           \
		const std::string curPicName = std::string("repaired-") + #MT + "-" + std::to_string(i + 1) + "-" + #FuncName + "-" + #F + "-" + FileName; \
		_OutPut(MT);                                                                                                                               \
	} while (false)

#define TimingRun(MT, FileName)                                                                                               \
	do                                                                                                                        \
	{                                                                                                                         \
		picRaw.copyTo(picDamaged);                                                                                            \
		doDamage(picDamaged, dmgv##MT[i]);                                                                                    \
		cv::imwrite(outputDir + #MT + "/damaged-" + #MT + "-" + std::to_string(i + 1) + "-" + FileName + ".png", picDamaged); \
		_TimingRun1(nearestNeighborBfs, MT, FileName);                                                                        \
		_TimingRun1(nearestNeighborRbfs, MT, FileName);                                                                       \
		_TimingRun1(nearestNeighborKd, MT, FileName);                                                                         \
		_TimingRun1(bilinearEquation, MT, FileName);                                                                          \
		_TimingRun1(bilinearKd, MT, FileName);                                                                                \
		_TimingRun1(fastMarching, MT, FileName);                                                                              \
		_TimingRun2(RBF, Gaussian, MT, FileName);                                                                             \
		_TimingRun2(RBF, InverseQuadratic, MT, FileName);                                                                     \
	} while (false)

void check(std::ostringstream& outstr, std::ofstream& outfile)
{
	outfile << outstr.str() << std::endl;
	outstr.str(std::string{});
}

int main(int argc, char* const argv[])
{
	std::ostringstream outstr;
	const std::string picDirs = (argc >= 2 ? argv[1] : "pictures.txt");
	const std::string outputDir = "output/";
    std::filesystem::create_directory(outputDir);
	std::filesystem::create_directory(outputDir + "Point");
	std::filesystem::create_directory(outputDir + "Line");
	std::filesystem::create_directory(outputDir + "Curve");
	std::filesystem::create_directory(outputDir + "Text");
    std::ofstream outfile(outputDir + "data.txt");
	const int
		Rows = (argc >= 4 ? std::max(std::atoi(argv[2]), 300) : 600),
		Cols = (argc >= 4 ? std::max(std::atoi(argv[3]), 400) : 800),
		Pixs = Rows * Cols;
	cv::Size sz(Cols, Rows);

	std::vector<P2i> dmgvPoint[9], dmgvLine[9], dmgvCurve[9], dmgvText[1];
	{
		cv::Mat_<uchar> maskPoint[9], maskLine[9], maskCurve[9], maskText[1];
		double k = 0.1;
		for (int i = 0; i < 9; i++, k += 0.1)
		{
			int deg = int(k * Pixs + 0.5);
			InitMask(Point);
			InitMask(Line);
			InitMask(Curve);
		}
		for (int i = 0; i < 1; i++)
		{
			int deg = 0;
			InitMask(Text);
		}
	}
	check(outstr, outfile);

	std::string picFileName;
	std::ifstream picDirStream(picDirs);
	if (picDirStream.bad())
		return -1;

	for (; std::getline(picDirStream, picFileName);)
	{
		cv::Mat picRaw = cv::imread(picFileName), picDamaged, picRepaired;
		const size_t picNameFirst = std::max(picFileName.find_last_of('/') + 1, picFileName.find_last_of("\\") + 1);
		const std::string picName = picFileName.substr(picNameFirst, picFileName.find_last_of('.') - picNameFirst);
		if (picRaw.empty())
			continue;
		if (picRaw.size() != sz)
			cv::resize(picRaw, picRaw, sz);

		picRaw.copyTo(picDamaged);
		for (int i = 0; i < 9; i++)
		{
			TimingRun(Point, picName);
			check(outstr, outfile);
			TimingRun(Line, picName);
			check(outstr, outfile);
			TimingRun(Curve, picName);
			check(outstr, outfile);
		}
		for (int i = 0; i < 1; i++)
		{
			TimingRun(Text, picName);
			check(outstr, outfile);
		}
	}
	return 0;
}
