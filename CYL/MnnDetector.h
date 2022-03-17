#pragma once
#include "Detector.h"
#include "MnnModel.hpp"

namespace objectrack{
class MNNFaceDetector : public IDetector
{
public:
    MNNFaceDetector(std::string model, float conf, int width, int height, int num_thread);

    virtual bool getDetectedBBX(std::vector<cv::Mat> frames, std::vector<cv::Rect>& BBXs) override;

protected:
    UltraFace _ultraFaceL;
    mnnSetting_t _MnnSet;
};
};