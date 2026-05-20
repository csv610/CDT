#include "TestMacros.h"
#include "../include/Numerics.h"
#include "../include/ImplicitPoint.h"

TEST(Orient2D) {
    ExplicitPoint2D a(0, 0);
    ExplicitPoint2D b(1, 0);
    ExplicitPoint2D c(0, 1);
    
    ASSERT_EQ(GenericPoint::Orient2D(a, b, c), 1);
    ASSERT_EQ(GenericPoint::Orient2D(a, c, b), -1);
    
    ExplicitPoint2D d(0.5, 0);
    ASSERT_EQ(GenericPoint::Orient2D(a, b, d), 0);
}

TEST(Orient3D) {
    ExplicitPoint3D a(0, 0, 0);
    ExplicitPoint3D b(1, 0, 0);
    ExplicitPoint3D c(0, 1, 0);
    ExplicitPoint3D d(0, 0, 1);
    
    // a-b-c is in XY plane, d is above (Z+)
    // Right handed system: (b-a) x (c-a) = (1,0,0) x (0,1,0) = (0,0,1)
    // (0,0,1) dot (d-a) = (0,0,1) dot (0,0,1) = 1
    // The code says -Orient3D internally for PointType but GenericPoint::Orient3D is what we use.
    // Let's see what Orient3D(a,b,c,d) returns.
    
    ASSERT_EQ(GenericPoint::Orient3D(a, b, c, d), 1);
}

TEST(Orient2D_Planes) {
    ExplicitPoint3D a(0, 0, 0);
    ExplicitPoint3D b(1, 0, 0);
    ExplicitPoint3D c(0, 1, 0);
    ExplicitPoint3D d(0, 0, 1);
    
    // XY plane: a, b, c
    ASSERT_EQ(GenericPoint::Orient2Dxy(a, b, c), 1);
    // YZ plane: a, c, d
    ASSERT_EQ(GenericPoint::Orient2Dyz(a, c, d), 1);
    // ZX plane: a, d, b
    ASSERT_EQ(GenericPoint::Orient2Dzx(a, d, b), 1);
}

TEST(PointInSegment) {
    ExplicitPoint2D a(0, 0);
    ExplicitPoint2D b(2, 2);
    ExplicitPoint2D p(1, 1);
    ExplicitPoint2D q(3, 3);
    
    ASSERT_TRUE(GenericPoint::pointInSegment(p, a, b));
    ASSERT_TRUE(GenericPoint::pointInInnerSegment(p, a, b));
    ASSERT_TRUE(GenericPoint::pointInSegment(a, a, b));
    ASSERT_FALSE(GenericPoint::pointInInnerSegment(a, a, b));
    ASSERT_FALSE(GenericPoint::pointInSegment(q, a, b));
}

TEST(PointInTriangle) {
    ExplicitPoint2D a(0, 0);
    ExplicitPoint2D b(2, 0);
    ExplicitPoint2D c(0, 2);
    
    ExplicitPoint2D inside(0.5, 0.5);
    ExplicitPoint2D on_edge(1, 0);
    ExplicitPoint2D outside(2, 2);
    
    ASSERT_TRUE(GenericPoint::pointInTriangle(inside, a, b, c));
    ASSERT_TRUE(GenericPoint::pointInInnerTriangle(inside, a, b, c));
    ASSERT_TRUE(GenericPoint::pointInTriangle(on_edge, a, b, c));
    ASSERT_FALSE(GenericPoint::pointInInnerTriangle(on_edge, a, b, c));
    ASSERT_FALSE(GenericPoint::pointInTriangle(outside, a, b, c));
}

TEST(SegmentsCross) {
    ExplicitPoint2D a(0, 0);
    ExplicitPoint2D b(2, 2);
    ExplicitPoint2D p(0, 2);
    ExplicitPoint2D q(2, 0);
    
    ASSERT_TRUE(GenericPoint::innerSegmentsCross(a, b, p, q));
    ASSERT_TRUE(GenericPoint::segmentsCross(a, b, p, q));
    
    ExplicitPoint2D c(3, 3);
    ASSERT_FALSE(GenericPoint::innerSegmentsCross(a, b, q, c));
}
