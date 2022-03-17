#pragma once
#include <opencv2/core/core.hpp>

namespace objectrack{

class IDetector {
public:
    virtual ~IDetector(){};
    virtual bool getDetectedBBX(std::vector<cv::Mat> frames, std::vector<cv::Rect>& BBXs) = 0;
};

class DefaultObjDetector : public IDetector
{
public:
    virtual bool getDetectedBBX(std::vector<cv::Mat> frames, std::vector<cv::Rect>& BBXs) override;

protected:
    // for debug
    void showContours_(const cv::Size& imageSize, const std::vector<std::vector<cv::Point>>& contours, const std::string& strImageName) const;

    unsigned int getVaildBBXs_(const std::vector<std::vector<cv::Point>>& convexHulls, std::vector<cv::Rect>& BBXs) const;
};

};