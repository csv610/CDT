#include "test_macros.h"
#include "../include/numerics.h"
#include "../include/implicit_point.h"

TEST(Orient2D) {
    explicitPoint2D a(0, 0);
    explicitPoint2D b(1, 0);
    explicitPoint2D c(0, 1);
    
    ASSERT_EQ(genericPoint::orient2D(a, b, c), 1);
    ASSERT_EQ(genericPoint::orient2D(a, c, b), -1);
    
    explicitPoint2D d(0.5, 0);
    ASSERT_EQ(genericPoint::orient2D(a, b, d), 0);
}

TEST(Orient3D) {
    explicitPoint3D a(0, 0, 0);
    explicitPoint3D b(1, 0, 0);
    explicitPoint3D c(0, 1, 0);
    explicitPoint3D d(0, 0, 1);
    
    // a-b-c is in XY plane, d is above (Z+)
    // Right handed system: (b-a) x (c-a) = (1,0,0) x (0,1,0) = (0,0,1)
    // (0,0,1) dot (d-a) = (0,0,1) dot (0,0,1) = 1
    // The code says -orient3D internally for pointType but genericPoint::orient3D is what we use.
    // Let's see what orient3D(a,b,c,d) returns.
    
    ASSERT_EQ(genericPoint::orient3D(a, b, c, d), 1);
}

TEST(Orient2D_Planes) {
    explicitPoint3D a(0, 0, 0);
    explicitPoint3D b(1, 0, 0);
    explicitPoint3D c(0, 1, 0);
    explicitPoint3D d(0, 0, 1);
    
    // XY plane: a, b, c
    ASSERT_EQ(genericPoint::orient2Dxy(a, b, c), 1);
    // YZ plane: a, c, d
    ASSERT_EQ(genericPoint::orient2Dyz(a, c, d), 1);
    // ZX plane: a, d, b
    ASSERT_EQ(genericPoint::orient2Dzx(a, d, b), 1);
}

TEST(PointInSegment) {
    explicitPoint2D a(0, 0);
    explicitPoint2D b(2, 2);
    explicitPoint2D p(1, 1);
    explicitPoint2D q(3, 3);
    
    ASSERT_TRUE(genericPoint::pointInSegment(p, a, b));
    ASSERT_TRUE(genericPoint::pointInInnerSegment(p, a, b));
    ASSERT_TRUE(genericPoint::pointInSegment(a, a, b));
    ASSERT_FALSE(genericPoint::pointInInnerSegment(a, a, b));
    ASSERT_FALSE(genericPoint::pointInSegment(q, a, b));
}

TEST(PointInTriangle) {
    explicitPoint2D a(0, 0);
    explicitPoint2D b(2, 0);
    explicitPoint2D c(0, 2);
    
    explicitPoint2D inside(0.5, 0.5);
    explicitPoint2D on_edge(1, 0);
    explicitPoint2D outside(2, 2);
    
    ASSERT_TRUE(genericPoint::pointInTriangle(inside, a, b, c));
    ASSERT_TRUE(genericPoint::pointInInnerTriangle(inside, a, b, c));
    ASSERT_TRUE(genericPoint::pointInTriangle(on_edge, a, b, c));
    ASSERT_FALSE(genericPoint::pointInInnerTriangle(on_edge, a, b, c));
    ASSERT_FALSE(genericPoint::pointInTriangle(outside, a, b, c));
}

TEST(SegmentsCross) {
    explicitPoint2D a(0, 0);
    explicitPoint2D b(2, 2);
    explicitPoint2D p(0, 2);
    explicitPoint2D q(2, 0);
    
    ASSERT_TRUE(genericPoint::innerSegmentsCross(a, b, p, q));
    ASSERT_TRUE(genericPoint::segmentsCross(a, b, p, q));
    
    explicitPoint2D c(3, 3);
    ASSERT_FALSE(genericPoint::innerSegmentsCross(a, b, q, c));
}
