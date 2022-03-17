#include "basic.h"
#include <cmath>

namespace objectrack{

static bool isPointInRectangle(const objectrack::Rect& rect, const objectrack::Point& point);

// =============================================================================
// objectrack::Point
// =============================================================================
double Point::DistanceTo(const objectrack::Point& pt) const
{
    return sqrt((pt.x - x)*(pt.x - x) + (pt.y - y)*(pt.y - y));
}

// =============================================================================
// objectrack::Rect
// =============================================================================

Rect::Rect(const objectrack::Point& topleft, int width, int height)
{
    tl = topleft;
    this->width = width;
    this->height = height;
}

Point Rect::getCenter() const
{
    return Point((tl.x + dr().x) / 2, (tl.y + dr().y) / 2);
}

double Rect::getDiagonalSize() const
{
    return tl.DistanceTo(dr());
}

static bool isPointInRectangle(const objectrack::Rect& rect, const objectrack::Point& point)
{
    Point A = rect.tl;
    Point B (rect.tl.x + rect.width , rect.tl.y ) ;
    Point C(rect.tl.x + rect.width , rect.tl.y + rect.height);
    Point D(rect.tl.x , rect.tl.y + rect.height );
    int x = point.x;
    int y = point.y;
    int a = (B.x - A.x)*(y - A.y) - (B.y - A.y)*(x - A.x);
    int b = (C.x - B.x)*(y - B.y) - (C.y - B.y)*(x - B.x);
    int c = (D.x - C.x)*(y - C.y) - (D.y - C.y)*(x - C.x);
    int d = (A.x - D.x)*(y - D.y) - (A.y - D.y)*(x - D.x);

    // 
    if((a >= 0 && b >= 0 && c >= 0 && d >= 0) || (a <= 0 && b <= 0 && c <= 0 && d <= 0)) {
        return true;
    }

//      AB X AP = (b.x - a.x, b.y - a.y) x (p.x - a.x, p.y - a.y) = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
//      BC X BP = (c.x - b.x, c.y - b.y) x (p.x - b.x, p.y - b.y) = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
    return false;
}

};