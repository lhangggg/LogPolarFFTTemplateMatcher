#include <fourier_mellin/fftm.hpp>

using namespace std;

namespace
{
//----------------------------------------------------------
// Recombinate image quaters
//----------------------------------------------------------
cv::Mat Recomb(const cv::Mat &src)
{
    int cx = src.cols >> 1;
    int cy = src.rows >> 1;
    cv::Mat tmp(src.size(), src.type());
    cv::Rect lt(0, 0, cx, cy), lb(0, cy, cx, cy);
    cv::Rect rt(cx, 0, cx, cy), rb(cx, cy, cx, cy);
    src(lt).copyTo(tmp(rb));
    src(lb).copyTo(tmp(rt));
    src(rt).copyTo(tmp(lb));
    src(rb).copyTo(tmp(lt));
    return tmp;
}

//----------------------------------------------------------
// 2D Forward FFT
//----------------------------------------------------------
void ForwardFFT(const cv::Mat &src, const cv::Mat *FImg, bool do_recomb = true)
{
    int height = cv::getOptimalDFTSize(src.rows);
    int width = cv::getOptimalDFTSize(src.cols);
    cv::Mat padded;
    cv::copyMakeBorder(src, padded, 0, height - src.rows, 0, width - src.cols,
                   cv::BORDER_CONSTANT, Scalar::all(0));
    // To Do: why ?
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complex_img;
    cv::merge(planes, 2, complex_img);
    cv::dft(complex_img, complex_img);
    cv::split(complex_img, planes);

    // To Do: why ?
    planes[0] = planes[0](cv::Rect(0, 0, planes[0].cols & -2, planes[0].rows & -2));
    planes[1] = planes[1](cv::Rect(0, 0, planes[1].cols & -2, planes[1].rows & -2));

    if (do_recomb)
    {
        Recomb(planes[0], planes[0]);
        Recomb(planes[1], planes[1]);
    }
    float whole_size = 1.f / static_cast<float>(width * height); 
    planes[0] *= whole_size;
    planes[1] *= whole_size;

    FImg[0] = planes[0].clone();
    FImg[1] = planes[1].clone();
}

//----------------------------------------------------------
// 2D inverse FFT
//----------------------------------------------------------
cv::Mat InverseFFT(const std::array<cv::Mat, 2> &f_imgs, bool do_recomb = true)
{
    if (do_recomb)
    {
        Recomb(f_imgs[0], f_imgs[0]);
        Recomb(f_imgs[1], f_imgs[1]);
    }
    cv::Mat complex_img;
    cv::merge(f_imgs, 2, complex_img);
    cv::idft(complex_img, complex_img);
    return cv::extractChannel(common_type, 0);
}

void highpass(Size sz, Mat &dst)
{
    Mat a = Mat(sz.height, 1, CV_32FC1);
    Mat b = Mat(1, sz.width, CV_32FC1);

    float step_y = CV_PI / sz.height;
    float val = -CV_PI * 0.5;

    for (int i = 0; i < sz.height; ++i)
    {
        a.at<float>(i) = cos(val);
        val += step_y;
    }

    val = -CV_PI * 0.5;
    float step_x = CV_PI / sz.width;
    for (int i = 0; i < sz.width; ++i)
    {
        b.at<float>(i) = cos(val);
        val += step_x;
    }

    Mat tmp = a * b;
    dst = (1.0 - tmp).mul(2.0 - tmp);
}

float logpolar(cv::Mat &src, Mat &dst)
{
    float radii = src.cols;
    float angles = src.rows;
    Point2f center(src.cols / 2, src.rows / 2);
    float d = norm(Vec2f(src.cols - center.x, src.rows - center.y));
    float log_base = pow(10.0, log10(d) / radii);
    float d_theta = CV_PI / (float)angles;
    float theta = CV_PI / 2.0;
    float radius = 0;
    Mat map_x(src.size(), CV_32FC1);
    Mat map_y(src.size(), CV_32FC1);
    for (int i = 0; i < angles; ++i)
    {
        for (int j = 0; j < radii; ++j)
        {
            radius = pow(log_base, float(j));
            float x = radius * sin(theta) + center.x;
            float y = radius * cos(theta) + center.y;
            map_x.at<float>(i, j) = x;
            map_y.at<float>(i, j) = y;
        }
        theta += d_theta;
    }
    remap(src, dst, map_x, map_y, cv::INTER_LINEAR, BORDER_CONSTANT,
          Scalar(0, 0, 0));
    return log_base;
}

} // namespace



//-----------------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
// As input we need equal sized images, with the same aspect ratio,
// scale difference should not exceed 1.8 times.
//-----------------------------------------------------------------------------------------------------
RotatedRect LogPolarFFTTemplateMatch(Mat &im0, Mat &im1,
                                     double canny_threshold1,
                                     double canny_threshold2)
{
    // Accept 1 or 3 channel CV_8U, CV_32F or CV_64F images.
    CV_Assert((im0.type() == CV_8UC1) || (im0.type() == CV_8UC3) ||
              (im0.type() == CV_32FC1) || (im0.type() == CV_32FC3) ||
              (im0.type() == CV_64FC1) || (im0.type() == CV_64FC3));

    CV_Assert(im0.rows == im1.rows && im0.cols == im1.cols);

    CV_Assert(im0.channels() == 1 || im0.channels() == 3 ||
              im0.channels() == 4);

    CV_Assert(im1.channels() == 1 || im1.channels() == 3 ||
              im1.channels() == 4);

    Mat im0_tmp = im0.clone();
    Mat im1_tmp = im1.clone();
    if (im0.channels() == 3)
    {
        cvtColor(im0, im0, cv::COLOR_BGR2GRAY);
    }

    if (im0.channels() == 4)
    {
        cvtColor(im0, im0, cv::COLOR_BGRA2GRAY);
    }

    if (im1.channels() == 3)
    {
        cvtColor(im1, im1, cv::COLOR_BGR2GRAY);
    }

    if (im1.channels() == 4)
    {
        cvtColor(im1, im1, cv::COLOR_BGRA2GRAY);
    }

    if (im0.type() == CV_32FC1)
    {
        im0.convertTo(im0, CV_8UC1, 255.0);
    }

    if (im1.type() == CV_32FC1)
    {
        im1.convertTo(im1, CV_8UC1, 255.0);
    }

    if (im0.type() == CV_64FC1)
    {
        im0.convertTo(im0, CV_8UC1, 255.0);
    }

    if (im1.type() == CV_64FC1)
    {
        im1.convertTo(im1, CV_8UC1, 255.0);
    }

    // Canny(im0, im0, canny_threshold1, canny_threshold2); // you can change
    // this
    //  Canny(im1, im1, canny_threshold1, canny_threshold2);

    // Ensure both images are of CV_32FC1 type
    im0.convertTo(im0, CV_32FC1, 1.0 / 255.0);
    im1.convertTo(im1, CV_32FC1, 1.0 / 255.0);

    Mat F0[2], F1[2];
    Mat f0, f1;
    ForwardFFT(im0, F0);
    ForwardFFT(im1, F1);
    magnitude(F0[0], F0[1], f0);
    magnitude(F1[0], F1[1], f1);
    // Create filter
    Mat h;
    highpass(f0.size(), h);

    // Apply it in freq domain
    f0 = f0.mul(h);
    f1 = f1.mul(h);
    /*
        cv::Mat mask = cv::Mat::zeros(f0.size(), CV_8UC1);
        cv::rectangle(mask, cv::Point(f0.cols / 2, f0.rows / 2),
       cv::Point(f0.cols, 0), Scalar::all(255), -1); mask = 255 - mask;
        f0.setTo(0, mask);
        f1.setTo(0, mask);
    */
    cv::normalize(f0, f0, 0, 1, NORM_MINMAX);
    cv::normalize(f1, f1, 0, 1, NORM_MINMAX);
    // cv::imshow("f0", f0);
    // cv::imshow("f1", f1);

    float log_base;
    Mat f0lp, f1lp;

    log_base = logpolar(f0, f0lp);
    log_base = logpolar(f1, f1lp);

    cv::Mat dbgImg = cv::Mat::zeros(f0lp.size(), CV_32FC3);
    cv::Mat Z = cv::Mat::zeros(f0lp.size(), CV_32FC1);

    // std::vector<cv::Mat> ch = {f0lp,f1lp,Z};
    // cv::merge(ch, dbgImg);
    // cv::imshow("dbgImg", dbgImg);

    // Find rotation and scale
    Point2d rotation_and_scale = cv::phaseCorrelate(f1lp, f0lp);

    float angle = 180.0 * rotation_and_scale.y / f0lp.rows;
    float scale = pow(log_base, rotation_and_scale.x);
    // --------------
    if (scale > 1.8)
    {
        rotation_and_scale = cv::phaseCorrelate(f1lp, f0lp);
        angle = -180.0 * rotation_and_scale.y / f0lp.rows;
        scale = 1.0 / pow(log_base, rotation_and_scale.x);
        if (scale > 1.8)
        {
            cout << "Images are not compatible. Scale change > 1.8" << endl;
            return RotatedRect();
        }
    }
    // --------------
    if (angle < -90.0)
    {
        angle += 180.0;
    }
    else if (angle > 90.0)
    {
        angle -= 180.0;
    }

    // Now rotate and scale fragment back, then find translation
    Mat rot_mat = getRotationMatrix2D(Point(im1.cols / 2, im1.rows / 2), angle,
                                      1.0 / scale);

    // rotate and scale
    Mat im1_rs;
    warpAffine(im1, im1_rs, rot_mat, im1.size());

    // find translation
    Point2d tr = cv::phaseCorrelate(im1_rs, im0);

    // compute rotated rectangle parameters
    RotatedRect rr;
    rr.center = tr + Point2d(im0.cols / 2, im0.rows / 2);
    rr.angle = -angle;
    rr.size.width = im1.cols / scale;
    rr.size.height = im1.rows / scale;

    im0 = im0_tmp.clone();
    im1 = im1_tmp.clone();

    return rr;
}