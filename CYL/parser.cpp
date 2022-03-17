/// 
#include "parser.hpp"

///
#include <cassert>

std::tuple<int, int, int, float, std::string, std::string, int, int, float, float> parse_arguments(int argc, char *argv[]) {
  assert(argc == 11 && "program called with wrong number of arguments");
  
	int width   = atoi(argv[1]);
	int height  = atoi(argv[2]);
	int num_thd = atoi(argv[3]);
	float conf  = atof(argv[4]);
	std::string model(argv[5]);
	std::string video_str(argv[6]);
	int iTrackNum = atoi(argv[7]);
	int iGiveUpNum = atoi(argv[8]);
	float dDiagonalSizeRate4Match = atof(argv[9]);
	float dStableDistance = atof(argv[10]);
  
  return std::make_tuple(width, height, num_thd, conf, model, video_str, iTrackNum, iGiveUpNum, dDiagonalSizeRate4Match, dStableDistance);
}