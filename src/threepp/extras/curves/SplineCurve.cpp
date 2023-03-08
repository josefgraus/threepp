
#include "threepp/extras/curves/SplineCurve.hpp"

#include "threepp/extras/core/Interpolations.hpp"


void threepp::SplineCurve::getPoint(float t, threepp::Vector2& point) {

    const auto p = static_cast<float>(points.size() - 1) * t;

    const auto intPoint = static_cast<int>(std::floor(p));
    const auto weight = p - static_cast<float>(intPoint);

    const auto p0 = points[intPoint == 0 ? intPoint : intPoint - 1];
    const auto p1 = points[intPoint];
    const auto p2 = points[intPoint > points.size() - 2 ? points.size() - 1 : intPoint + 1];
    const auto p3 = points[intPoint > points.size() - 3 ? points.size() - 1 : intPoint + 2];

    point.set(
            interpolants::CatmullRom(weight, p0.x, p1.x, p2.x, p3.x),
            interpolants::CatmullRom(weight, p0.y, p1.y, p2.y, p3.y));
}
