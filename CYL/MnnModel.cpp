#include <iostream>

#include "CYL/MnnModel.hpp"
#include "CYL/UltraFace.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

int MNN_Facedetect(int d_num, Mat srcimg, UltraFace &_ultraface, std::vector<faceDetect_t>& FaceRoi)
{
    int d_count = 0;
    faceDetect_t tmpFace;
    vector<FaceInfo> face_info;

    // auto th_start = std::chrono::system_clock::now();
    _ultraface.detect(srcimg, face_info);
    // auto th_end = std::chrono::system_clock::now();

    if (face_info.size() == 0)
    {
        return 0;
    }
    /* May detect > 1 Face*/
    for (auto face : face_info)
    {
        Point pt1(face.x1, face.y1);
        tmpFace.pt_St = pt1;
        Point pt2(face.x2, face.y2);
        tmpFace.pt_End = pt2;
        tmpFace.p_x1 = face.x1;
        tmpFace.p_y1 = face.y1;
        tmpFace.p_x2 = face.x2;
        tmpFace.p_y2 = face.y2;
        tmpFace.Roi_Width = face.x2 - face.x1;
        tmpFace.Roi_Height = face.y2 - face.y1;

        tmpFace.pcen_face_X = (face.x1 + face.x2) / 2;
        tmpFace.pcen_face_Y = (face.y1 + face.y2) / 2;
        //cv::Rect rect(face.x1, face.y1, face.x2 - face.x1, face.y2 - face.y1);
        //srcimg(rect).copyTo(tmpFace.FaceImg);
        d_count++;
        tmpFace.FaceNo = d_count;
        FaceRoi.push_back(tmpFace);
        if (d_count == d_num)break;
    }
    return d_count;
}

void MNN_init(string model, float conf, int width, int height, int num_thread, UltraFace& ultraFaceL, mnnSetting_t& _MnnSet)
{
    // extern UltraFace ultraFaceL;
    // extern mnnSetting_t _MnnSet;

    _MnnSet._mnn_path = model;
    _MnnSet.detectNumber = 20;

    // _MnnSet._mnn_input_w = 320;
    // _MnnSet._mnn_input_h = 240;

    _MnnSet._mnn_input_w = width;
    _MnnSet._mnn_input_h = height;

    _MnnSet._mnn_num_th = num_thread;
    _MnnSet._mnn_sc_th = conf;

    ultraFaceL.init(_MnnSet._mnn_path, _MnnSet._mnn_input_w, _MnnSet._mnn_input_h, _MnnSet._mnn_num_th, _MnnSet._mnn_sc_th);
}