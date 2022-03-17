#ifndef mnnModel_hpp
#define mnnModel_hpp

#include <string>

#include <opencv2/core/core.hpp>
#include <stdlib.h>

#include "UltraFace.hpp"

extern double calTime(std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point  end);

using namespace std;
using namespace cv;


typedef struct mnnSetting {
    string _mnn_path;   //�פJMNN�ҫ���
    int detectNumber;
    int _mnn_input_w;
    int _mnn_input_h;
    int _mnn_num_th;         //�˴��ƶq���e
    float _mnn_sc_th;     //�˴����ƪ��e
    float _mnn_iou_th;         //�˴����|���e
} mnnSetting_t;

typedef struct faceDetect {
    int FaceNo;
    Point pt_St;
    Point pt_End;
    float pcen_face_X;
    float pcen_face_Y;
    float p_x1;
    float p_y1;
    float p_x2;
    float p_y2;
    int Roi_Width;
    int Roi_Height;
    Mat FaceImg;
    Point flirLeftUp;
    Point flirRightDown;
} faceDetect_t;

typedef struct roiOffset {
    float dist_Offset_th; //cm
    float PintX_Offset;
    float PintY_Offset;
    float X_coef;
    float Y_coef;
    float offset_W;
    float offset_H;
    int fov_x1;
    int fov_y1;
    int fov_x2;
    int fov_y2;
} roiOffset_t;



int MNN_Facedetect(int d_num, Mat srcimg, UltraFace &_ultraface, std::vector<faceDetect_t>& FaceRoi);
void MNN_init(string model, float conf, int width, int height, int num_thread, UltraFace& ultraFaceL, mnnSetting_t& _MnnSet);

#endif
