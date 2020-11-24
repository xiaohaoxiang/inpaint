#include <opencv2/imgproc.hpp>
#include "defs.hpp"

double MSE(const cv::Mat& I1, const cv::Mat& I2)
{
    cv::Mat s1;
    absdiff(I1, I2, s1);
    s1.convertTo(s1, CV_32F);
    s1 = s1.mul(s1);

    cv::Scalar s = sum(s1);

    double sse = s.val[0] + s.val[1] + s.val[2];

    return sse / (double)(I1.channels() * I1.total());
}

double PSNR(const cv::Mat& I1, const cv::Mat& I2)
{
    return 10.0 * log10(255.0 * 255.0 / MSE(I1, I2));
}

double MSSIM(const cv::Mat& i1, const cv::Mat& i2, const float C1, const float C2)
{
    cv::Mat I1, I2;
    i1.convertTo(I1, CV_32F);
    i2.convertTo(I2, CV_32F);

    cv::Mat I2_2 = I2.mul(I2);
    cv::Mat I1_2 = I1.mul(I1);
    cv::Mat I1_I2 = I1.mul(I2);

    cv::Mat mu1, mu2;
    cv::GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
    cv::GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

    cv::Mat mu1_2 = mu1.mul(mu1);
    cv::Mat mu2_2 = mu2.mul(mu2);
    cv::Mat mu1_mu2 = mu1.mul(mu2);

    cv::Mat sigma1_2, sigma2_2, sigma12;

    cv::GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    cv::GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    cv::GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    cv::Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + cv::Scalar::all(C1);
    t2 = 2 * sigma12 + cv::Scalar::all(C2);
    t3 = t1.mul(t2);

    t1 = mu1_2 + mu2_2 + cv::Scalar::all(C1);
    t2 = sigma1_2 + sigma2_2 + cv::Scalar::all(C2);
    t1 = t1.mul(t2);

    cv::Mat ssim_map;
    cv::divide(t3, t1, ssim_map);

    cv::Scalar mssim = cv::mean(ssim_map);
    return (mssim[0] + mssim[1] + mssim[2]) / 3.0;
}
