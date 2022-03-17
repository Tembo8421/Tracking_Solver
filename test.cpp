#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "CYL/MnnDetector.h"
#include "CYL/DefaultTracker.h"
#include "CYL/parser.hpp"

using namespace cv;
using namespace std;

#define TIMER_PRINT_INIT() \
chrono::system_clock::time_point start;\
chrono::system_clock::time_point stop;

#define TIMER_PRINT_START() \
start = chrono::system_clock::now();

#define TIMER_PRINT_END() \
stop = chrono::system_clock::now();\
cout << "Elapsed time:" << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << " ms" << endl;

/////////////////////////////////////////////////////////////////////////////////////////////////////

const cv::Scalar SCALAR_RED     = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE  	= cv::Scalar(255.0, 0.0, 0.0);
const cv::Scalar SCALAR_GREEN   = cv::Scalar(0.0, 255.0, 0.0);

static void drawTrackBBXInfo(const std::vector<objectrack::TrackBBX> &BBXs,
                        	 cv::Mat &imgFrame,
							 cv::Scalar color = SCALAR_RED,
							 bool showId = true);	 

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	int width, height;
	int num_thd;
	float conf;
	std::string model, video_str;
	int iTrackNum, iGiveUpNum;
	float dDiagonalSizeRate4Match, dStableDistance;

	/// pharse argv
	std::tie(width, height, num_thd, conf, model, video_str, iTrackNum, iGiveUpNum, dDiagonalSizeRate4Match, dStableDistance) = parse_arguments(argc, argv);

	/// Initial
	TIMER_PRINT_INIT()

	///
	std::unique_ptr<objectrack::IDetector> myDetector(new objectrack::MNNFaceDetector(model, conf, width, height, num_thd));

	objectrack::TrackBBX::s_uiGiveUpNum = iGiveUpNum > 0 ? iGiveUpNum : 1;
	objectrack::TrackBBX::s_uiTrackNum = iTrackNum > 0 ? iTrackNum : 1;
	objectrack::TrackBBX::s_dDiagonalSizeRate4Match = dDiagonalSizeRate4Match > 0.0 ? dDiagonalSizeRate4Match : 1.0;
	objectrack::TrackBBX::s_dStableDistance = dStableDistance > 0.0 ? dStableDistance : 1.0;
    std::unique_ptr<objectrack::ITrackerDefault> myTracker(new objectrack::DefaultTracker());
	
	///
	VideoCapture video(video_str);
	double FPS = video.get(cv::CAP_PROP_FPS);
	cout << "test video fps: " << FPS << endl;
	unsigned int skipFrame = FPS * 0.7;

	int frame_width = video.get(CAP_PROP_FRAME_WIDTH);
	int frame_height = video.get(CAP_PROP_FRAME_HEIGHT);
	VideoWriter  video_w("result_" + video_str, cv::VideoWriter::fourcc('m','p','4','v'), FPS, Size(frame_width, frame_height));
	
	///
	Mat frame;
	unsigned int frameCount = 0;
	char chCheckForEscKey = 0;

	/// Loop Start
	while(video.read(frame) && chCheckForEscKey != 27)
	{
		/// skipFrame = FPS * 0.7
		if (!(frameCount % skipFrame)) {

			cout << "frame: " << frameCount << endl;

			/// Detecting
			std::vector<cv::Rect> MNN_DetectedBBXs;
			TIMER_PRINT_START()
			myDetector->getDetectedBBX({frame}, MNN_DetectedBBXs);
			TIMER_PRINT_END()

			cout << "human amount : " << MNN_DetectedBBXs.size() << endl;

			/// display detect
			cv::Point offset(2, 2);
			for (auto faceBBX : MNN_DetectedBBXs) {
				faceBBX.x += offset.x;
				faceBBX.y += offset.y;
				cv::rectangle(frame, faceBBX, SCALAR_GREEN, 2, 8);
			}

			/// trurn all detected cv::Rect into objectrack::TrackBBX
			std::vector<objectrack::TrackBBX> currentFrameBBXs;
			currentFrameBBXs.reserve(MNN_DetectedBBXs.size());
			for (const auto &faceBBX : MNN_DetectedBBXs)
				currentFrameBBXs.push_back(objectrack::Rect({faceBBX.x, faceBBX.y}, faceBBX.width, faceBBX.height));

			/// Tracking
			TIMER_PRINT_START()
			myTracker->tracking(currentFrameBBXs);
			TIMER_PRINT_END()
			cout << "existBBXs count : " << myTracker->existBBXs.size() << endl;

			/// Sort
			std::sort(myTracker->existBBXs.begin(), myTracker->existBBXs.end(),
        		[](const objectrack::TrackBBX &lhs, const objectrack::TrackBBX &rhs){
            		return lhs.trackingCenters.back().x < rhs.trackingCenters.back().x;
        		});

        	currentFrameBBXs.clear();
		}

		/// display tracking
        drawTrackBBXInfo(myTracker->existBBXs, frame);
		imshow("img", frame);
        // cv::waitKey(0);                 /// uncomment this line to go frame by frame for debugging

		/// 
        frameCount++;
		video_w.write(frame);
		chCheckForEscKey = cv::waitKey(1);
	}
    return 0;
}


static void drawTrackBBXInfo(const std::vector<objectrack::TrackBBX>& BBXs,
							 cv::Mat &imgFrame,
							 cv::Scalar color,
							 bool showId)
{
    for (unsigned int i = 0; i < BBXs.size(); i++) {
        if (BBXs[i].isStillBeingTracked
		  /*&& BBXs[i].trackingCenters.size() >= objectrack::TrackBBX::s_uiTrackNum*/) {
			if (BBXs[i].isStable())
				color = SCALAR_BLUE;
			cv::Rect BBX(cv::Point(BBXs[i].trackingBBXs.back().tl.x, BBXs[i].trackingBBXs.back().tl.y), cv::Size(BBXs[i].trackingBBXs.back().width, BBXs[i].trackingBBXs.back().height));
            cv::rectangle(imgFrame, BBX, color, 2, 8);
			if (showId){
				int intFontFace = cv::FONT_HERSHEY_SIMPLEX;
				double dblFontScale = BBXs[i].dDiagonalSize / 150.0;
				int intFontThickness = (int)std::round(dblFontScale * 1.0);
				cv::putText(imgFrame, std::to_string(i), cv::Point(BBXs[i].trackingCenters.back().x, BBXs[i].trackingCenters.back().y), intFontFace, dblFontScale, color, intFontThickness);
			}
		}
    } 
}