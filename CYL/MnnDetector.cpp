#include "MnnDetector.h"

namespace objectrack{
MNNFaceDetector::MNNFaceDetector(std::string model, float conf, int width, int height, int num_thread)
{
    MNN_init(model, conf, width, height, num_thread, _ultraFaceL, _MnnSet);
}

bool MNNFaceDetector::getDetectedBBX(std::vector<cv::Mat> frames,
                                    std::vector<cv::Rect>& BBXs)
{
    if (frames.empty()){
#ifdef CYLDEBUG 
        std::cout << "frames is empty !" << std::endl;
#endif
        return false;
    }

    std::vector<faceDetect_t> faceDetL;
	int faceCountL = MNN_Facedetect(_MnnSet.detectNumber, frames.back(), _ultraFaceL, faceDetL);

    BBXs.clear();
    BBXs.reserve(faceCountL);
    for (const auto& faceDt : faceDetL)
        BBXs.push_back(cv::Rect(faceDt.pt_St, faceDt.pt_End));

    return faceCountL ? true : false;
}
};