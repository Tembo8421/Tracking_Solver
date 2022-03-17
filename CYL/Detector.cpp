#include "Detector.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CYLDEBUG
#include <iostream>

namespace objectrack{

static const cv::Scalar BLACK = cv::Scalar(0.0, 0.0, 0.0);
static const cv::Scalar WHITE = cv::Scalar(255.0, 255.0, 255.0);

bool DefaultObjDetector::getDetectedBBX(std::vector<cv::Mat> frames,
                                       std::vector<cv::Rect>& BBXs)
{
    if (frames.size() < 2) {
#ifdef CYLDEBUG 
        std::cout << "frames.size() < 2 !" << std::endl;
#endif
        return false;
    }

    cv::Mat frames0 = frames[0].clone();
	cv::Mat frames1 = frames[1].clone();

    cv::Mat imgDiff;
    cv::Mat imgThresh;

    /// to Gray
    cv::cvtColor(frames0, frames0, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frames1, frames1, cv::COLOR_BGR2GRAY);

    /// GaussianBlur
    cv::GaussianBlur(frames0, frames0, cv::Size(5, 5), 0);
    cv::GaussianBlur(frames1, frames1, cv::Size(5, 5), 0);

    cv::absdiff(frames0, frames1, imgDiff);

    cv::threshold(imgDiff, imgThresh, 30, 255.0, cv::THRESH_BINARY);

    /// Debug show: imgThresh
#ifdef CYLDEBUG 
    cv::imshow("imgThresh", imgThresh);
#endif

    /// kernel array 5x5
    int StructuringElementSize = 5;
    cv::Mat structEle = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(StructuringElementSize, StructuringElementSize));

    /// dilate iterations = 2;
    cv::dilate(imgThresh, imgThresh, structEle);
    cv::dilate(imgThresh, imgThresh, structEle);

    /// erode iterations = 1;
    cv::erode(imgThresh, imgThresh, structEle);

    /// findContours
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(imgThresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    /// Debug show: imgContours
#ifdef CYLDEBUG 
    showContours_(imgThresh.size(), contours, "imgContours");
#endif

    std::size_t size = contours.size();
    std::vector<std::vector<cv::Point>> convexHulls(size);
    for (unsigned int i = 0; i < size; i++)
        cv::convexHull(contours[i], convexHulls[i]);

#ifdef CYLDEBUG 
    showContours_(imgThresh.size(), convexHulls, "imgConvexHulls");
#endif
    
    if (!getVaildBBXs_(convexHulls, BBXs))
        return false;

    return true;
};

void  DefaultObjDetector::showContours_(const cv::Size& imageSize,
                                        const std::vector<std::vector<cv::Point>>& contours,
                                        const std::string& strImageName) const
{
    cv::Mat image(imageSize, CV_8UC3, BLACK);
    cv::drawContours(image, contours, -1, WHITE, -1);
    cv::imshow(strImageName, image);
}

unsigned int DefaultObjDetector::getVaildBBXs_(const std::vector<std::vector<cv::Point>>& convexHulls,
                                               std::vector<cv::Rect>& BBXs) const
{

    unsigned int count = 0;
    std::size_t size = convexHulls.size();
    if (!size)
        return count;

    for (const auto &convexHull : convexHulls) {
        cv::Rect possibleObjBBX = cv::boundingRect(convexHull);
        double diagonalSize = sqrt(pow(possibleObjBBX.width, 2) + pow(possibleObjBBX.height, 2));
        double aspectRatio = (double)possibleObjBBX.width / (double)possibleObjBBX.height;
        int areaBBX = possibleObjBBX.area();

        if (aspectRatio >= 0.2 && aspectRatio <= 1.25 &&
            possibleObjBBX.width > 20 &&
            possibleObjBBX.height > 20 &&
            diagonalSize > 30.0 &&
            areaBBX > 100 &&
            (cv::contourArea(convexHull) / (double)areaBBX) > 0.40)
        {    
            BBXs.push_back(possibleObjBBX);
            ++count;
        }
    }

    return count;
}
};