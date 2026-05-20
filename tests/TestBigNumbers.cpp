#include "TestMacros.h"
#include "../include/Numerics.h"

TEST(BignaturalBasicOps) {
    BigNatural a(123);
    BigNatural b(456);
    
    BigNatural c = a + b;
    uint32_t val;
    ASSERT_TRUE(c.toUint32(val));
    ASSERT_EQ(val, 123 + 456);
    
    BigNatural d = b - a;
    ASSERT_TRUE(d.toUint32(val));
    ASSERT_EQ(val, 456 - 123);
    
    BigNatural e = a * b;
    ASSERT_TRUE(e.toUint32(val));
    ASSERT_EQ(val, 123 * 456);
}

TEST(BigfloatBasicOps) {
    BigFloat a(1.5);
    BigFloat b(2.5);
    
    BigFloat c = a + b;
    ASSERT_NEAR(c.get_d(), 4.0, 1e-15);
    
    BigFloat d = a * b;
    ASSERT_NEAR(d.get_d(), 3.75, 1e-15);
    
    BigFloat e(1.5);
    BigFloat f = e * 2.0;
    ASSERT_NEAR(f.get_d(), 3.0, 1e-15);
}

TEST(BigrationalBasicOps) {
    BigRational a(BigFloat(1.0));
    BigRational b(BigFloat(3.0));
    
    BigRational c = a / b;
    ASSERT_NEAR(c.get_d(), 1.0/3.0, 1e-15);
    
    BigRational d = c * b;
    ASSERT_NEAR(d.get_d(), 1.0, 1e-15);
    ASSERT_TRUE(d == BigRational(BigFloat(1.0)));
}
