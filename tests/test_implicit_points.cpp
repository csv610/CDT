#include "test_macros.h"
#include "../include/implicit_point.h"
#include <iostream>

TEST(test_explicit_point_2d) {
    explicitPoint2D p1(1.0, 2.0);
    ASSERT_EQ(p1.X(), 1.0);
    ASSERT_EQ(p1.Y(), 2.0);
    ASSERT_TRUE(p1.is2D());
    ASSERT_FALSE(p1.is3D());
    ASSERT_TRUE(p1.isExplicit2D());

    explicitPoint2D p2(1.0, 2.0);
    ASSERT_TRUE(p1 == p2);

    explicitPoint2D p3(2.0, 3.0);
    ASSERT_FALSE(p1 == p3);
}

TEST(test_explicit_point_3d) {
    explicitPoint3D p1(1.0, 2.0, 3.0);
    ASSERT_EQ(p1.X(), 1.0);
    ASSERT_EQ(p1.Y(), 2.0);
    ASSERT_EQ(p1.Z(), 3.0);
    ASSERT_TRUE(p1.is3D());
    ASSERT_FALSE(p1.is2D());
    ASSERT_TRUE(p1.isExplicit3D());

    explicitPoint3D p2(1.0, 2.0, 3.0);
    ASSERT_TRUE(p1 == p2);
}

TEST(test_ssi_point) {
    explicitPoint2D l1_1(0.0, 0.0);
    explicitPoint2D l1_2(2.0, 2.0);
    explicitPoint2D l2_1(0.0, 2.0);
    explicitPoint2D l2_2(2.0, 0.0);

    implicitPoint2D_SSI ssi(l1_1, l1_2, l2_1, l2_2);
    ASSERT_TRUE(ssi.isSSI());
    ASSERT_TRUE(ssi.is2D());

    double x, y;
    ASSERT_TRUE(ssi.getApproxXYCoordinates(x, y));
    ASSERT_NEAR(x, 1.0, 1e-9);
    ASSERT_NEAR(y, 1.0, 1e-9);

    bigrational ex, ey;
    ASSERT_TRUE(ssi.getExactXYCoordinates(ex, ey));
    ASSERT_EQ(ex, bigrational(1));
    ASSERT_EQ(ey, bigrational(1));
}

TEST(test_lpi_point) {
    explicitPoint3D p(1.0, 1.0, 0.0);
    explicitPoint3D q(1.0, 1.0, 2.0);
    explicitPoint3D r(0.0, 0.0, 1.0);
    explicitPoint3D s(2.0, 0.0, 1.0);
    explicitPoint3D t(1.0, 2.0, 1.0);

    implicitPoint3D_LPI lpi(p, q, r, s, t);
    ASSERT_TRUE(lpi.isLPI());
    ASSERT_TRUE(lpi.is3D());

    double x, y, z;
    lpi.getApproxXYZCoordinates(x, y, z);
    ASSERT_NEAR(x, 1.0, 1e-9);
    ASSERT_NEAR(y, 1.0, 1e-9);
    ASSERT_NEAR(z, 1.0, 1e-9);

    bigrational ex, ey, ez;
    ASSERT_TRUE(lpi.getExactXYZCoordinates(ex, ey, ez));
    ASSERT_EQ(ex, bigrational(1));
    ASSERT_EQ(ey, bigrational(1));
    ASSERT_EQ(ez, bigrational(1));
}
