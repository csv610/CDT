#include "TestMacros.h"
#include "../include/Numerics.h"

TEST(IntervalBasicOps) {
    setFPUModeToRoundUP();
    IntervalNumber a(1.0);
    IntervalNumber b(2.0);
    
    IntervalNumber c = a + b;
    ASSERT_NEAR(c.inf(), 3.0, 1e-15);
    ASSERT_NEAR(c.sup(), 3.0, 1e-15);
    
    IntervalNumber d = a - b;
    ASSERT_NEAR(d.inf(), -1.0, 1e-15);
    ASSERT_NEAR(d.sup(), -1.0, 1e-15);

    IntervalNumber e = a * b;
    ASSERT_NEAR(e.inf(), 2.0, 1e-15);
    ASSERT_NEAR(e.sup(), 2.0, 1e-15);

    IntervalNumber f = b / 2.0;
    ASSERT_NEAR(f.inf(), 1.0, 1e-15);
    ASSERT_NEAR(f.sup(), 1.0, 1e-15);
    
    setFPUModeToRoundNEAR();
}

TEST(IntervalInexact) {
    setFPUModeToRoundUP();
    // 1.0 / 3.0 is not exactly representable
    IntervalNumber a(1.0);
    IntervalNumber b(3.0);
    IntervalNumber c = a / 3.0; // This might use the double version if available
    
    // Check that it contains the true value
    double true_val = 1.0 / 3.0;
    ASSERT_TRUE(c.inf() <= true_val);
    ASSERT_TRUE(c.sup() >= true_val);
    ASSERT_TRUE(c.sup() > c.inf()); // Should not be exact
    
    setFPUModeToRoundNEAR();
}

TEST(IntervalSqrt) {
    setFPUModeToRoundUP();
    IntervalNumber a(2.0);
    IntervalNumber b = sqrt(a);
    
    double true_val = std::sqrt(2.0);
    ASSERT_TRUE(b.inf() <= true_val);
    ASSERT_TRUE(b.sup() >= true_val);
    setFPUModeToRoundNEAR();
}
