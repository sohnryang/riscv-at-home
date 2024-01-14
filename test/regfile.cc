#include <gtest/gtest.h>

#include <cstdint>
#include <memory>

#include "Vregfile.h"
#include "test_utils.h"

class RegfileTest : public SingleComponentFixture<Vregfile, 5> {
protected:
  void init_component() override { component->clk = 0; }

  void reset() {
    for (int i = 0; i < 4; i++) {
      component->clk ^= 1;
      component->rst = 1;
      step();
    }
  }
};

TEST_F(RegfileTest, Reset) {
  reset();

  component->read_idx1 = 1;
  component->rst = 0;

  for (int i = 1; i < 32; i++) {
    component->read_idx2 = i;
    EXPECT_EQ((uint32_t)component->read_data2, 0);
    step();
  }
}

TEST_F(RegfileTest, Write) {
  reset();

  component->clk ^= 1;
  component->write_idx = 1;
  component->write_data = 0x12345678;
  component->write_enable = 1;
  component->rst = 0;
  step();

  component->clk ^= 1;
  component->write_enable = 0;
  step();

  for (int i = 1; i < 32; i++) {
    component->clk ^= 1;
    component->read_idx1 = i;
    component->read_idx2 = i;
    step();

    if (i == 1) {
      EXPECT_EQ((uint32_t)component->read_data1, 0x12345678);
      EXPECT_EQ((uint32_t)component->read_data2, 0x12345678);
    } else {
      EXPECT_EQ((uint32_t)component->read_data1, 0);
      EXPECT_EQ((uint32_t)component->read_data2, 0);
    }

    component->clk ^= 1;
    step();
  }
}

TEST_F(RegfileTest, WriteX0) {
  reset();

  component->clk ^= 1;
  component->write_idx = 0;
  component->write_data = 0x12345678;
  component->write_enable = 1;
  component->rst = 0;
  step();

  component->clk ^= 1;
  component->write_enable = 0;
  step();

  component->clk ^= 1;
  component->read_idx1 = 0;
  component->read_idx2 = 0;
  step();

  EXPECT_EQ((uint32_t)component->read_data1, 0);
  EXPECT_EQ((uint32_t)component->read_data2, 0);
}
