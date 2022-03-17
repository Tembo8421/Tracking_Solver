#pragma once
#include <vector>

namespace objectrack{

template<typename myTrackBBX>
class ITracker
{
public:
    virtual ~ITracker(){};

    virtual void tracking(std::vector<myTrackBBX> &currentFrameBBXs) = 0;

    std::vector<myTrackBBX> existBBXs;
};
};