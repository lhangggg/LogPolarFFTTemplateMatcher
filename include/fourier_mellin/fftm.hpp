#ifndef FOURIER_MELIN_FFTM_HPP
#define FOURIER_MELIN_FFTM_HPP

#include "opencv2/opencv.hpp"

//----------------------------------------------------------------------------------
// As input we need equal sized images, with the same aspect ratio,
// scale difference should not exceed 1.8 times.
//----------------------------------------------------------------------------------

cv::RotatedRect FFTMMatch(const cv::Mat &refer_image, const cv::Mat &test_image,
                          double lower_thresh = 200, double upper_thresh = 100);
#endif
