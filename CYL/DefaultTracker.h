#pragma once
#include "basic.h"
#include "ITracker.hpp"
#include <deque>

namespace objectrack{

class TrackBBX 
{
public:
/// virtual 
    virtual ~TrackBBX(){};

    virtual void predictNextPosition();
    
    virtual bool isStable() const;
/// static
    static unsigned int s_uiTrackNum;

    static unsigned int s_uiGiveUpNum;

    static double s_dDiagonalSizeRate4Match;
    
    static double s_dStableDistance;
public:
    TrackBBX() = default;

    TrackBBX(const objectrack::Rect& BBX);

/// member
    bool isCurrentMatchFoundOrNewOne = true;

    bool isStillBeingTracked =  true;

    unsigned int iNumOfLoseMatch = 0;

    std::deque<objectrack::Rect> trackingBBXs;
    std::deque<objectrack::Point> trackingCenters;

    objectrack::Point predictedNextCenter;
    double dDiagonalSize = 0.0;

    void pushTrackingCenter(const objectrack::Point& pt);
    void pushTrackingBBX(const objectrack::Rect& BBX);
};

using  ITrackerDefault = ITracker<TrackBBX>;

class DefaultTracker : public ITrackerDefault
{
public:
    virtual void tracking(std::vector<TrackBBX> &currentFrameBBXs) override;

protected:
    void tracking_init();

    void tracking_LoseMatch();

    void shrinkExistBBXSize();
};
};