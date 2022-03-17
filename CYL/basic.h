#pragma once

namespace objectrack{

class Point {
public:
    Point() = default;
    Point(int X, int Y):x(X), y(Y) {};

    int x = 0;
    int y = 0;

    double DistanceTo(const objectrack::Point& pt) const;
};

class Rect {
public:
    Rect() = default;
    Rect(const objectrack::Point& topleft, int width, int height);
    objectrack::Point tl;
    int width;
    int height;

    objectrack::Point getCenter() const;

    inline objectrack::Point dr() const { return {tl.x + width, tl.y + height}; };

    double getDiagonalSize() const;
};
};