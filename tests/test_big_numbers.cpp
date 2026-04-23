#include "test_macros.h"
#include "../include/numerics.h"

TEST(BignaturalBasicOps) {
    bignatural a(123);
    bignatural b(456);
    
    bignatural c = a + b;
    uint32_t val;
    ASSERT_TRUE(c.toUint32(val));
    ASSERT_EQ(val, 123 + 456);
    
    bignatural d = b - a;
    ASSERT_TRUE(d.toUint32(val));
    ASSERT_EQ(val, 456 - 123);
    
    bignatural e = a * b;
    ASSERT_TRUE(e.toUint32(val));
    ASSERT_EQ(val, 123 * 456);
}

TEST(BigfloatBasicOps) {
    bigfloat a(1.5);
    bigfloat b(2.5);
    
    bigfloat c = a + b;
    ASSERT_NEAR(c.get_d(), 4.0, 1e-15);
    
    bigfloat d = a * b;
    ASSERT_NEAR(d.get_d(), 3.75, 1e-15);
    
    bigfloat e(1.5);
    bigfloat f = e * 2.0;
    ASSERT_NEAR(f.get_d(), 3.0, 1e-15);
}

TEST(BigrationalBasicOps) {
    bigrational a(bigfloat(1.0));
    bigrational b(bigfloat(3.0));
    
    bigrational c = a / b;
    ASSERT_NEAR(c.get_d(), 1.0/3.0, 1e-15);
    
    bigrational d = c * b;
    ASSERT_NEAR(d.get_d(), 1.0, 1e-15);
    ASSERT_TRUE(d == bigrational(bigfloat(1.0)));
}
