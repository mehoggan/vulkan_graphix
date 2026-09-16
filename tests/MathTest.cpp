#include "vulkan_graphix/Math/AxisAligned2D.hpp"
#include "vulkan_graphix/Math/Combinatorics.hpp"
#include "vulkan_graphix/Math/CubicCurve.hpp"
#include "vulkan_graphix/Math/CurveSample3D.hpp"
#include "vulkan_graphix/Math/Geometry.hpp"
#include "vulkan_graphix/Math/Icosahedron.hpp"
#include "vulkan_graphix/Math/Line.hpp"
#include "vulkan_graphix/Math/MathTypes.hpp"
#include "vulkan_graphix/Math/Plane3D.hpp"
#include "vulkan_graphix/Math/Sphere.hpp"
#include "vulkan_graphix/Math/TessellationOps.hpp"
#include "vulkan_graphix/Math/TessellationTypes.hpp"
#include "vulkan_graphix/Math/Triangle.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

using namespace vulkan_graphix::Math;

TEST(CombinatoricsTest, NextCombinationEnumeratesInLexicographicOrder) {
    std::vector<int> values = {0, 1, 2, 3, 4};
    std::size_t const select_count = 3;

    std::vector<std::vector<int>> const expected = {
            {0, 1, 2},
            {0, 1, 3},
            {0, 1, 4},
            {0, 2, 3},
            {0, 2, 4},
            {0, 3, 4},
            {1, 2, 3},
            {1, 2, 4},
            {1, 3, 4},
            {2, 3, 4},
    };

    std::size_t index = 0;
    do {
        ASSERT_LT(index, expected.size());
        std::vector<int> const combination(values.begin(),
                                           values.begin() + select_count);
        EXPECT_EQ(expected[index], combination);
        ++index;
    } while (nextCombination<std::vector<int>, 5>(values, select_count));

    EXPECT_EQ(expected.size(), index);
}

TEST(AxisAligned2DTest, DefaultIsInvalidUntilGrown) {
    AxisAligned2D<float> bounds;
    EXPECT_FALSE(bounds.isValid());

    bounds.growToContain(Vec2<float>(1.0f, 2.0f));
    bounds.growToContain(Vec2<float>(-1.0f, 5.0f));

    EXPECT_TRUE(bounds.isValid());
    EXPECT_FLOAT_EQ(2.0f, bounds.width());
    EXPECT_FLOAT_EQ(3.0f, bounds.height());
    EXPECT_TRUE(bounds.contains(Vec2<float>(0.0f, 3.0f)));
    EXPECT_FALSE(bounds.contains(Vec2<float>(10.0f, 10.0f)));
}

TEST(AxisAligned2DTest, ConstructFromLowerLeftWidthHeight) {
    AxisAligned2D<float> bounds(Vec2<float>(0.0f, 0.0f), 4.0f, 2.0f);
    EXPECT_FLOAT_EQ(4.0f, bounds.width());
    EXPECT_FLOAT_EQ(2.0f, bounds.height());
    EXPECT_EQ(Vec2<float>(2.0f, 1.0f), bounds.center());
}

TEST(Plane3DTest, NormalAndDistanceFromPoint) {
    // The plane z = 0, i.e. 0x + 0y + 1z + 0 = 0.
    Plane3D<float> plane(0.0f, 0.0f, 1.0f, 0.0f);
    EXPECT_TRUE(plane.isValid());
    EXPECT_EQ(Vec3<float>(0.0f, 0.0f, 1.0f), plane.normal());
    EXPECT_FLOAT_EQ(5.0f,
                    plane.distanceFromPoint(Vec3<float>(1.0f, 2.0f, 5.0f)));
}

TEST(Plane3DTest, DefaultConstructedIsInvalid) {
    Plane3D<float> plane;
    EXPECT_FALSE(plane.isValid());
}

TEST(GeometryTest, CentroidAndMidpoint) {
    Triangle<float> flat_tri(Vec3<float>(0.0f, 0.0f, 0.0f),
                             Vec3<float>(3.0f, 0.0f, 0.0f),
                             Vec3<float>(0.0f, 3.0f, 0.0f));
    EXPECT_EQ(Vec3<float>(1.0f, 1.0f, 0.0f), centroidOfTriangle(flat_tri));

    Line<float> line(Vec3<float>(0.0f, 0.0f, 0.0f),
                     Vec3<float>(2.0f, 4.0f, 6.0f));
    EXPECT_EQ(Vec3<float>(1.0f, 2.0f, 3.0f), midpointOfLine(line));
}

TEST(GeometryTest, CollinearPointsAreDetected) {
    Triangle<float> collinear(Vec3<float>(0.0f, 0.0f, 0.0f),
                              Vec3<float>(1.0f, 0.0f, 0.0f),
                              Vec3<float>(2.0f, 0.0f, 0.0f));
    EXPECT_TRUE(pointsOfTriangleAreCollinear(collinear));

    Triangle<float> non_collinear(Vec3<float>(0.0f, 0.0f, 0.0f),
                                  Vec3<float>(1.0f, 0.0f, 0.0f),
                                  Vec3<float>(0.0f, 1.0f, 0.0f));
    EXPECT_FALSE(pointsOfTriangleAreCollinear(non_collinear));
}

TEST(GeometryTest, SphericalCartesianRoundTrip) {
    SphericalCoordinates<float, AngleMode::Degrees> coords(
            45.0f, 30.0f, 10.0f);
    Vec3<float> cartesian = sphericalToCartesian(coords);
    auto round_tripped =
            cartesianToSpherical<float, AngleMode::Degrees>(cartesian);

    EXPECT_NEAR(coords.theta(), round_tripped.theta(), 1e-3f);
    EXPECT_NEAR(coords.phi(), round_tripped.phi(), 1e-3f);
    EXPECT_NEAR(coords.radius(), round_tripped.radius(), 1e-3f);
}

TEST(TessellationOpsTest, SubdivisionProducesMoreTrianglesThanBaseCase) {
    Triangle<float> flat_tri(Vec3<float>(0.0f, 0.0f, 0.0f),
                             Vec3<float>(1.0f, 0.0f, 0.0f),
                             Vec3<float>(0.0f, 1.0f, 0.0f));

    TessellatedTriangleData<float, std::uint32_t> base;
    std::uint32_t base_index = 0;
    tessellateTriangleBySubdivision(flat_tri, 0, base_index, base);
    EXPECT_EQ(3u, base.points().size());
    EXPECT_EQ(3u, base.indices().size());

    TessellatedTriangleData<float, std::uint32_t> subdivided;
    std::uint32_t sub_index = 0;
    tessellateTriangleBySubdivision(flat_tri, 2, sub_index, subdivided);
    EXPECT_GT(subdivided.indices().size(), base.indices().size());
}

TEST(TessellationOpsTest, MidpointSubdivisionSharesEdgeVertices) {
    Triangle<float> flat_tri(Vec3<float>(0.0f, 0.0f, 0.0f),
                             Vec3<float>(1.0f, 0.0f, 0.0f),
                             Vec3<float>(0.0f, 1.0f, 0.0f));

    TessellatedTriangleData<float, std::uint32_t> once;
    std::uint32_t index = 0;
    tessellateTriangleByMidpointSubdivision(flat_tri, 1, index, once);
    // 4 sub-triangles (3 corners + 3 edge midpoints, deduplicated).
    EXPECT_EQ(6u, once.points().size());
    EXPECT_EQ(12u, once.indices().size());
}

TEST(IcosahedronTest, HasTwelvePointsAndTwentyTriangles) {
    Icosahedron<float, std::uint32_t> icosahedron(1.0f);

    EXPECT_FLOAT_EQ(1.0f, icosahedron.radius());
    EXPECT_EQ(12u, icosahedron.points().size());
    EXPECT_EQ(12u, icosahedron.normals().size());
    EXPECT_EQ(60u, icosahedron.indices().size());  // 20 triangles * 3.
}

TEST(SphereTest, UvSphereProducesExpectedVertexAndIndexCounts) {
    std::uint16_t const theta_steps = 8;
    std::uint16_t const phi_steps = 4;
    Sphere<float, std::uint32_t> sphere(1.0f, theta_steps, phi_steps);

    EXPECT_FLOAT_EQ(1.0f, sphere.radius());
    // Two poles + (phi_steps - 1) rings of theta_steps points each.
    std::size_t const expected_points = 2u + (phi_steps - 1u) * theta_steps;
    EXPECT_EQ(expected_points, sphere.points().size());
    EXPECT_EQ(sphere.points().size(), sphere.normals().size());
    // phi_steps rings of triangles, 2 triangles per quad except the pole
    // rings which are single-triangle fans.
    std::size_t const expected_triangles =
            2u * theta_steps + (phi_steps - 2u) * 2u * theta_steps;
    EXPECT_EQ(expected_triangles * 3u, sphere.indices().size());
}

TEST(SphereTest, IcosphereSubdivisionGrowsPointCountWithLevelOfDetail) {
    Sphere<float, std::uint32_t> base(1.0f, static_cast<std::uint8_t>(0));
    Sphere<float, std::uint32_t> subdivided_once(1.0f,
                                                 static_cast<std::uint8_t>(1));
    Sphere<float, std::uint32_t> subdivided_twice(
            1.0f, static_cast<std::uint8_t>(2));

    EXPECT_EQ(12u, base.points().size());
    EXPECT_EQ(60u, base.indices().size());

    // Each level replaces every triangle with four (via its three edge
    // midpoints, shared with the adjacent triangle across each edge), the
    // standard icosphere vertex/triangle progression: V(n) = 10*4^n + 2,
    // T(n) = 20*4^n.
    EXPECT_EQ(42u, subdivided_once.points().size());
    EXPECT_EQ(240u, subdivided_once.indices().size());  // 80 triangles * 3.

    EXPECT_EQ(162u, subdivided_twice.points().size());
    EXPECT_EQ(960u, subdivided_twice.indices().size());  // 320 triangles * 3.

    for (Vec3<float> const& normal : subdivided_twice.normals()) {
        EXPECT_NEAR(1.0f, glm::length(normal), 1e-4f);
    }
}

TEST(CurveSample3DTest, NormalizeTangentProducesUnitLength) {
    CurveSample3D<float> sample(
            Vec3<float>(0.0f), Vec3<float>(3.0f, 0.0f, 0.0f), 0.5f);
    sample.normalizeTangent();
    EXPECT_FLOAT_EQ(1.0f, glm::length(sample.tangent));
}

TEST(CurveSample3DTest, NormalizeTangentLeavesZeroVectorAlone) {
    CurveSample3D<float> sample(Vec3<float>(0.0f), Vec3<float>(0.0f), 0.5f);
    sample.normalizeTangent();
    EXPECT_EQ(Vec3<float>(0.0f), sample.tangent);
}

TEST(CubicCurveTest, BezierEndpointsMatchControlPoints) {
    Vec3<float> point0(0.0f, 0.0f, 0.0f);
    Vec3<float> point1(1.0f, 1.0f, 0.0f);
    Vec3<float> point2(2.0f, -1.0f, 0.0f);
    Vec3<float> point3(3.0f, 0.0f, 0.0f);

    CubicCurve<float> curve(
            CubicCurve<float>::Type::Bezier, point0, point1, point2, point3);

    Vec3<float> start = curve.evaluatePosition(0.0f);
    Vec3<float> end_point = curve.evaluatePosition(1.0f);

    EXPECT_NEAR(point0.x, start.x, 1e-4f);
    EXPECT_NEAR(point0.y, start.y, 1e-4f);
    EXPECT_NEAR(point3.x, end_point.x, 1e-4f);
    EXPECT_NEAR(point3.y, end_point.y, 1e-4f);
}

TEST(CubicCurveTest, HermiteInterpolatesBetweenEndpoints) {
    Vec3<float> point0(0.0f, 0.0f, 0.0f);
    Vec3<float> point1(1.0f, 0.0f, 0.0f);
    Vec3<float> tangent0(1.0f, 0.0f, 0.0f);
    Vec3<float> tangent1(1.0f, 0.0f, 0.0f);

    CubicCurve<float> curve(point0, point1, tangent0, tangent1);

    EXPECT_NEAR(0.0f, curve.evaluatePosition(0.0f).x, 1e-4f);
    EXPECT_NEAR(1.0f, curve.evaluatePosition(1.0f).x, 1e-4f);
}

TEST(CubicCurveTest, SampleValuesSpanZeroToOne) {
    CubicCurve<float> curve;
    std::vector<float> samples = curve.computeSampleValues(5);
    ASSERT_EQ(5u, samples.size());
    EXPECT_FLOAT_EQ(0.0f, samples.front());
    EXPECT_FLOAT_EQ(1.0f, samples.back());
}

TEST(CubicCurveTest, AdaptiveSamplingProducesSortedParameters) {
    Vec3<float> point0(0.0f, 0.0f, 0.0f);
    Vec3<float> point1(1.0f, 2.0f, 0.0f);
    Vec3<float> point2(2.0f, -2.0f, 0.0f);
    Vec3<float> point3(3.0f, 0.0f, 0.0f);
    CubicCurve<float> curve(
            CubicCurve<float>::Type::Bezier, point0, point1, point2, point3);

    std::vector<CurveSample3D<float>> samples =
            curve.computeSamplesAdaptive(0.05f);

    ASSERT_GE(samples.size(), 2u);
    for (std::size_t index = 1; index < samples.size(); ++index) {
        EXPECT_LT(samples[index - 1].parameter, samples[index].parameter);
    }
}

TEST(CubicCurveTest, CatmullRomEndpointsMatchInnerControlPoints) {
    // A Catmull-Rom segment interpolates between its two inner control
    // points (point1, point2); the outer points (point0, point3) only
    // shape the tangents at those ends.
    Vec3<float> point0(0.0f, 0.0f, 0.0f);
    Vec3<float> point1(1.0f, 0.0f, 0.0f);
    Vec3<float> point2(2.0f, 0.0f, 0.0f);
    Vec3<float> point3(3.0f, 0.0f, 0.0f);

    CubicCurve<float> curve(CubicCurve<float>::Type::CatmullRom,
                            point0,
                            point1,
                            point2,
                            point3);

    Vec3<float> start = curve.evaluatePosition(0.0f);
    Vec3<float> end_point = curve.evaluatePosition(1.0f);
    Vec3<float> midpoint = curve.evaluatePosition(0.5f);

    EXPECT_NEAR(point1.x, start.x, 1e-4f);
    EXPECT_NEAR(point2.x, end_point.x, 1e-4f);
    EXPECT_NEAR(1.5f, midpoint.x, 1e-4f);
}
