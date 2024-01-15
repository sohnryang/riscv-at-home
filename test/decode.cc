#include <gtest/gtest.h>

#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

#include "Vdecode.h"
#include "test_utils.h"

class DecodeTest : public SingleComponentFixture<Vdecode> {
protected:
  void init_component() override { component->inst = 0; }
};

TEST_F(DecodeTest, LUI) {
  component->inst = 0x0dead0b7; // lui x1, 0xdead
  step();
  EXPECT_TRUE((uint32_t)component->valid);
  EXPECT_EQ((uint32_t)component->opcode, 0b0110111);
  EXPECT_EQ((uint32_t)component->rd, 1);
  EXPECT_EQ((uint32_t)component->imm, 0x0dead000);
}
