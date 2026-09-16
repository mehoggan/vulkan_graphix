#ifndef VULKAN_GRAPHIX_CUBICCURVE_HPP
#define VULKAN_GRAPHIX_CUBICCURVE_HPP

#include <algorithm>
#include <cstddef>
#include <glm/gtc/epsilon.hpp>
#include <limits>
#include <vector>

#include "vulkan_graphix/Math/CurveSample3D.hpp"
#include "vulkan_graphix/Math/MathTypes.hpp"

namespace vulkan_graphix::Math {

namespace detail {

template <typename T> Mat4<T> catmullRomBasis() {
    return Mat4<T>(Vec4<T>(T(-0.5), T(+1.5), T(-1.5), T(+0.5)),
                   Vec4<T>(T(+1.0), T(-2.5), T(+2.0), T(-0.5)),
                   Vec4<T>(T(-0.5), T(+0.0), T(+0.5), T(+0.0)),
                   Vec4<T>(T(+0.0), T(+1.0), T(+0.0), T(+0.0)));
}

template <typename T> Mat4<T> bezierBasis() {
    return Mat4<T>(Vec4<T>(T(-1.0), T(+3.0), T(-3.0), T(+1.0)),
                   Vec4<T>(T(+3.0), T(-6.0), T(+3.0), T(+0.0)),
                   Vec4<T>(T(-3.0), T(+3.0), T(+0.0), T(+0.0)),
                   Vec4<T>(T(+1.0), T(+0.0), T(+0.0), T(+0.0)));
}

template <typename T> Mat4<T> hermiteBasis() {
    return Mat4<T>(Vec4<T>(T(+2.0), T(-2.0), T(+1.0), T(+1.0)),
                   Vec4<T>(T(-3.0), T(+3.0), T(-2.0), T(-1.0)),
                   Vec4<T>(T(+0.0), T(+0.0), T(+1.0), T(+0.0)),
                   Vec4<T>(T(+1.0), T(+0.0), T(+0.0), T(+0.0)));
}

}  // namespace detail

// A parametric cubic curve (Bezier, Hermite, or Catmull-Rom), always in
// column-major layout (matching glm/Vulkan/GL, which this project targets).
template <typename T> class CubicCurve {
public:
    enum class Type { Bezier, Hermite, CatmullRom };

    explicit CubicCurve(Type impl = Type::Hermite)
            : m_cubic(T(1)), m_impl(impl) {}

    CubicCurve(Vec3<T> const& point0,
               Vec3<T> const& point1,
               Vec3<T> const& tangent0,
               Vec3<T> const& tangent1)
            : m_cubic(T(1)), m_impl(Type::Hermite) {
        setHermite(point0, point1, tangent0, tangent1);
    }

    CubicCurve(Type impl,
               Vec3<T> const& point0,
               Vec3<T> const& point1,
               Vec3<T> const& point2,
               Vec3<T> const& point3)
            : m_cubic(T(1)), m_impl(impl) {
        switch (impl) {
            case Type::Bezier:
                setBezier(point0, point1, point2, point3);
                break;
            case Type::Hermite:
                setHermite(point0, point3, point2 - point0, point3 - point1);
                break;
            case Type::CatmullRom:
                setCatmullRom(point0, point1, point2, point3);
                break;
        }
    }

    Type type() const { return m_impl; }

    void setHermite(Vec3<T> const& point0,
                    Vec3<T> const& point1,
                    Vec3<T> const& tangent0,
                    Vec3<T> const& tangent1) {
        m_impl = Type::Hermite;
        Mat4<T> geom(controlColumn(point0),
                     controlColumn(point1),
                     controlColumn(tangent0),
                     controlColumn(tangent1));
        m_cubic = geom * detail::hermiteBasis<T>();
    }

    void setBezier(Vec3<T> const& point0,
                   Vec3<T> const& point1,
                   Vec3<T> const& point2,
                   Vec3<T> const& point3) {
        m_impl = Type::Bezier;
        Mat4<T> geom(controlColumn(point0),
                     controlColumn(point1),
                     controlColumn(point2),
                     controlColumn(point3));
        m_cubic = geom * detail::bezierBasis<T>();
    }

    void setCatmullRom(Vec3<T> const& point0,
                       Vec3<T> const& point1,
                       Vec3<T> const& point2,
                       Vec3<T> const& point3) {
        m_impl = Type::CatmullRom;
        Mat4<T> geom(controlColumn(point0),
                     controlColumn(point1),
                     controlColumn(point2),
                     controlColumn(point3));
        m_cubic = geom * detail::catmullRomBasis<T>();
    }

    Vec3<T> evaluatePosition(T param) const {
        Vec4<T> powers(param * param * param, param * param, param, T(1));
        Vec4<T> result = m_cubic * powers;
        return Vec3<T>(result.x, result.y, result.z);
    }

    Vec3<T> evaluateTangent(T param) const {
        Vec4<T> powers(T(3) * param * param, T(2) * param, T(1), T(0));
        Vec4<T> result = m_cubic * powers;
        return Vec3<T>(result.x, result.y, result.z);
    }

    CurveSample3D<T> evaluate(T param) const {
        return CurveSample3D<T>(
                evaluatePosition(param), evaluateTangent(param), param);
    }

    std::vector<T> computeSampleValues(std::size_t sample_count = 2) const {
        sample_count = std::max<std::size_t>(sample_count, 2);

        std::vector<T> out_samples;
        out_samples.reserve(sample_count);
        out_samples.push_back(T(0));
        for (std::size_t index = 1; index < sample_count - 1; ++index) {
            out_samples.push_back(static_cast<T>(index) /
                                  static_cast<T>(sample_count - 1));
        }
        out_samples.push_back(T(1));

        return out_samples;
    }

    std::vector<CurveSample3D<T>> computeSamplesUniform(
            std::size_t sample_count = 2) const {
        sample_count = std::max<std::size_t>(sample_count, 2);

        std::vector<CurveSample3D<T>> out_samples;
        out_samples.reserve(sample_count);
        for (std::size_t index = 0; index < sample_count; ++index) {
            T param = static_cast<T>(index) / static_cast<T>(sample_count - 1);
            out_samples.push_back(evaluate(param));
        }

        return out_samples;
    }

    std::vector<CurveSample3D<T>> computeSamplesAdaptive(
            T chordal_tolerance) const {
        std::vector<CurveSample3D<T>> out_samples;

        std::vector<T> t_values = computeSampleValues();
        CurveSample3D<T> begin_sample = evaluate(t_values[0]);
        CurveSample3D<T> end_sample = evaluate(t_values[1]);

        out_samples.push_back(begin_sample);
        subdivide(out_samples, begin_sample, end_sample, chordal_tolerance);
        out_samples.push_back(end_sample);

        return out_samples;
    }

private:
    static Vec4<T> controlColumn(Vec3<T> const& point) {
        return Vec4<T>(point.x, point.y, point.z, T(0));
    }

    static constexpr unsigned short max_subdivide_depth = 10;

    T chordalDistanceSquaredFromSecant(CurveSample3D<T> const& point0,
                                       CurveSample3D<T> const& point1,
                                       T percent) const {
        Vec3<T> secant_position =
                glm::mix(point0.position, point1.position, percent);
        CurveSample3D<T> curve_position =
                evaluate(point0.parameter +
                         (percent * (point1.parameter - point0.parameter)));
        Vec3<T> delta = curve_position.position - secant_position;
        return glm::dot(delta, delta);
    }

    bool isOfGoodQuality(CurveSample3D<T> const& point0,
                         T mid_t,
                         CurveSample3D<T> const& point1,
                         T chordal_tolerance,
                         unsigned short depth) const {
        T current_chordal_tolerance =
                chordalDistanceSquaredFromSecant(point0, point1, mid_t);
        T chordal_tolerance_sq = chordal_tolerance * chordal_tolerance;
        bool is_good = (current_chordal_tolerance <= chordal_tolerance_sq);

        if (depth > max_subdivide_depth) {
            is_good = true;
        } else if (is_good) {
            Vec3<T> secant = point1.position - point0.position;
            T epsilon = std::numeric_limits<T>::epsilon();
            if (glm::all(glm::epsilonEqual(secant, Vec3<T>(T(0)), epsilon)) ||
                glm::all(glm::epsilonEqual(
                        point0.tangent, Vec3<T>(T(0)), epsilon))) {
                is_good = true;
            } else {
                Vec3<T> tangent = glm::normalize(point0.tangent);
                secant = glm::normalize(secant);
                T tangent_deviation = glm::length(secant - tangent);
                if (tangent_deviation > T(0.10)) {
                    is_good = false;
                }
            }
        }

        return is_good;
    }

    void subdivide(std::vector<CurveSample3D<T>>& out_samples,
                   CurveSample3D<T> const& point0,
                   CurveSample3D<T> const& point1,
                   T chordal_tolerance,
                   unsigned short depth = 0) const {
        T mid_param = glm::mix(point0.parameter, point1.parameter, T(0.5));
        CurveSample3D<T> break_point = evaluate(mid_param);

        if (isOfGoodQuality(
                    point0, mid_param, point1, chordal_tolerance, depth)) {
            return;
        }
        ++depth;
        subdivide(out_samples, point0, break_point, chordal_tolerance, depth);
        out_samples.push_back(break_point);
        subdivide(out_samples, break_point, point1, chordal_tolerance, depth);
    }

    Mat4<T> m_cubic;
    Type m_impl;
};

}  // namespace vulkan_graphix::Math

#endif
