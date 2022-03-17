#include "DefaultTracker.h"
#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
#include <vector>

// #define CYL_DEBUG

namespace objectrack{
// =============================================================================
// objectrack::TrackBBX
// =============================================================================

unsigned int TrackBBX::s_uiTrackNum = 6;

unsigned int TrackBBX::s_uiGiveUpNum = 2;

double TrackBBX::s_dDiagonalSizeRate4Match = 3.5;

double TrackBBX::s_dStableDistance = 3.0;

TrackBBX::TrackBBX(const objectrack::Rect& BBX) 
{
    trackingBBXs.push_back(BBX);

    dDiagonalSize = BBX.getDiagonalSize();

    trackingCenters.push_back(BBX.getCenter());
}

void TrackBBX::pushTrackingCenter(const Point& pt)
{
    trackingCenters.push_back(pt);
    if(trackingCenters.size() > s_uiTrackNum)
        trackingCenters.pop_front();
}

void TrackBBX::pushTrackingBBX(const Rect& BBX)
{
    trackingBBXs.push_back(BBX);
    if(trackingBBXs.size() > s_uiTrackNum)
        trackingBBXs.pop_front();
}

void TrackBBX::predictNextPosition() 
{
    std::size_t numPositions = trackingCenters.size();
    numPositions =  numPositions > 5 ? 5 : trackingCenters.size();

    int sumOfXChanges = 0;
    int sumOfYChanges = 0;
    int sum = 0;
    for (int i = numPositions-1; i > 0; --i){
        sumOfXChanges += (trackingCenters[i].x - trackingCenters[i-1].x)*i;
        sumOfYChanges += (trackingCenters[i].y - trackingCenters[i-1].y)*i;
        sum += i;
    }
    int deltaX = (int)std::round((float)sumOfXChanges / sum);
    int deltaY = (int)std::round((float)sumOfYChanges / sum);

    predictedNextCenter.x = trackingCenters.back().x + deltaX;
    predictedNextCenter.y = trackingCenters.back().y + deltaY;
}

bool TrackBBX::isStable() const
{
    std::size_t size = trackingCenters.size();
    /// unstable if ...
    if (size < TrackBBX::s_uiTrackNum/2)
        return false;
    
    /// 
    for (std::size_t i = 0; i < size; ++i){
        if (trackingCenters[i].DistanceTo(trackingCenters[(i+1)%size]) > TrackBBX::s_dStableDistance)
            return false;
    }
    return true;
}
// =============================================================================
//
// =============================================================================

void DefaultTracker::tracking(std::vector<objectrack::TrackBBX> &currentFrameBBXs)
{
#ifdef CYL_DEBUG
    std::cout << "1. current BBX count: " << currentFrameBBXs.size() << std::endl;
    std::cout << "1. exist BBX count: " << existBBXs.size() << std::endl;
#endif
    tracking_init();

    /// matching
    for (auto &currentBBX : currentFrameBBXs) {

        int idxOfminDis = 0;
        double dminDis = std::numeric_limits<double>::max();

        for (unsigned int i = 0; i < existBBXs.size(); i++) {
            if (existBBXs[i].isCurrentMatchFoundOrNewOne == false && existBBXs[i].isStillBeingTracked == true) {

                const Point& pt0 = currentBBX.trackingCenters.back();
                const Point& pt1 = existBBXs[i].predictedNextCenter;
                double dSquareDis = (pt1.x - pt0.x)*(pt1.x - pt0.x) + (pt1.y - pt0.y)*(pt1.y - pt0.y);

                if (dSquareDis < dminDis) {
                    dminDis = dSquareDis;
                    idxOfminDis = i;
                }
            }
        }
#ifdef CYL_DEBUG
        std::cout << "dminDis: " << dminDis << std::endl;
#endif
        dminDis = dminDis < 0.0 ? 0.0 : dminDis;
#ifdef CYL_DEBUG
        std::cout << "sqrt_dminDis: " << sqrt(dminDis) << std::endl;
#endif
        if (sqrt(dminDis) < currentBBX.dDiagonalSize * objectrack::TrackBBX::s_dDiagonalSizeRate4Match) {

            objectrack::TrackBBX& targetBBX = existBBXs[idxOfminDis];
            targetBBX.pushTrackingBBX(currentBBX.trackingBBXs.back());
            targetBBX.pushTrackingCenter(currentBBX.trackingCenters.back());

            targetBBX.dDiagonalSize = currentBBX.dDiagonalSize;

            targetBBX.isStillBeingTracked = true;
            targetBBX.isCurrentMatchFoundOrNewOne = true;
            targetBBX.iNumOfLoseMatch = 0;
        }
        else { // add a new one
            currentBBX.isCurrentMatchFoundOrNewOne = true;
            existBBXs.push_back(currentBBX);
        }

    }
#ifdef CYL_DEBUG
    std::cout << "2.match exist BBX count: " << existBBXs.size() << std::endl;
#endif
    tracking_LoseMatch();
	shrinkExistBBXSize();
#ifdef CYL_DEBUG
    std::cout << "3.shrink exist BBX count: " << existBBXs.size() << std::endl;
    if (existBBXs.size() < currentFrameBBXs.size())
        exit(1);
#endif
}

void DefaultTracker::tracking_init()
{
    for (auto &existBBX : existBBXs) {
        existBBX.isCurrentMatchFoundOrNewOne = false;
        existBBX.predictNextPosition();
    }
}

void DefaultTracker::tracking_LoseMatch()
{
    for (auto &existBBX : existBBXs) {
        if (existBBX.isCurrentMatchFoundOrNewOne == false) {
            existBBX.pushTrackingBBX(existBBX.trackingBBXs.back());
            existBBX.pushTrackingCenter(existBBX.trackingCenters.back());
            existBBX.iNumOfLoseMatch++;
        }

        if (existBBX.iNumOfLoseMatch >= objectrack::TrackBBX::s_uiGiveUpNum) {
            existBBX.isStillBeingTracked = false;
        }
    }
}

void DefaultTracker::shrinkExistBBXSize()
{
	std::vector<objectrack::TrackBBX> tempBBXs;
    tempBBXs.swap(existBBXs);
    std::copy_if (tempBBXs.begin(), tempBBXs.end(), std::back_inserter(existBBXs), [](const objectrack::TrackBBX& i){return i.isStillBeingTracked;} );
}
};