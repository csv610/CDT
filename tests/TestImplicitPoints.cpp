#include "TestMacros.h"
#include "../include/ImplicitPoint.h"
#include <iostream>

TEST(test_explicit_point_2d) {
    ExplicitPoint2D p1(1.0, 2.0);
    ASSERT_EQ(p1.X(), 1.0);
    ASSERT_EQ(p1.Y(), 2.0);
    ASSERT_TRUE(p1.is2D());
    ASSERT_FALSE(p1.is3D());
    ASSERT_TRUE(p1.isExplicit2D());

    ExplicitPoint2D p2(1.0, 2.0);
    ASSERT_TRUE(p1 == p2);

    ExplicitPoint2D p3(2.0, 3.0);
    ASSERT_FALSE(p1 == p3);
}

TEST(test_explicit_point_3d) {
    ExplicitPoint3D p1(1.0, 2.0, 3.0);
    ASSERT_EQ(p1.X(), 1.0);
    ASSERT_EQ(p1.Y(), 2.0);
    ASSERT_EQ(p1.Z(), 3.0);
    ASSERT_TRUE(p1.is3D());
    ASSERT_FALSE(p1.is2D());
    ASSERT_TRUE(p1.isExplicit3D());

    ExplicitPoint3D p2(1.0, 2.0, 3.0);
    ASSERT_TRUE(p1 == p2);
}

TEST(test_ssi_point) {
    ExplicitPoint2D l1_1(0.0, 0.0);
    ExplicitPoint2D l1_2(2.0, 2.0);
    ExplicitPoint2D l2_1(0.0, 2.0);
    ExplicitPoint2D l2_2(2.0, 0.0);

    ImplicitPoint2dSsi ssi(l1_1, l1_2, l2_1, l2_2);
    ASSERT_TRUE(ssi.isSSI());
    ASSERT_TRUE(ssi.is2D());

    double x, y;
    ASSERT_TRUE(ssi.getApproxXYCoordinates(x, y));
    ASSERT_NEAR(x, 1.0, 1e-9);
    ASSERT_NEAR(y, 1.0, 1e-9);

    BigRational ex, ey;
    ASSERT_TRUE(ssi.getExactXYCoordinates(ex, ey));
    ASSERT_EQ(ex, BigRational(1));
    ASSERT_EQ(ey, BigRational(1));
}

TEST(test_lpi_point) {
    ExplicitPoint3D p(1.0, 1.0, 0.0);
    ExplicitPoint3D q(1.0, 1.0, 2.0);
    ExplicitPoint3D r(0.0, 0.0, 1.0);
    ExplicitPoint3D s(2.0, 0.0, 1.0);
    ExplicitPoint3D t(1.0, 2.0, 1.0);

    ImplicitPoint3dLpi lpi(p, q, r, s, t);
    ASSERT_TRUE(lpi.isLPI());
    ASSERT_TRUE(lpi.is3D());

    double x, y, z;
    lpi.getApproxXYZCoordinates(x, y, z);
    ASSERT_NEAR(x, 1.0, 1e-9);
    ASSERT_NEAR(y, 1.0, 1e-9);
    ASSERT_NEAR(z, 1.0, 1e-9);

    BigRational ex, ey, ez;
    ASSERT_TRUE(lpi.getExactXYZCoordinates(ex, ey, ez));
    ASSERT_EQ(ex, BigRational(1));
    ASSERT_EQ(ey, BigRational(1));
    ASSERT_EQ(ez, BigRational(1));
}
