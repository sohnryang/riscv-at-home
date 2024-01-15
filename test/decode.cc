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

TEST_F(DecodeTest, AUIPC) {
  component->inst = 0x12345117; // auipc x2, 0x12345
  step();
  EXPECT_TRUE((uint32_t)component->valid);
  EXPECT_EQ((uint32_t)component->opcode, 0b0010111);
  EXPECT_EQ((uint32_t)component->rd, 2);
  EXPECT_EQ((uint32_t)component->imm, 0x12345000);
}

TEST_F(DecodeTest, JAL) {
  component->inst = 0x234011ef; // jal x3, 0x1234
  step();
  EXPECT_TRUE((uint32_t)component->valid);
  EXPECT_EQ((uint32_t)component->opcode, 0b1101111);
  EXPECT_EQ((uint32_t)component->rd, 3);
  EXPECT_EQ((uint32_t)component->imm, 0x1234);
}

TEST_F(DecodeTest, JALR) {
  component->inst = 0x12328267; // jalr x4, x5, 0x123
  step();
  EXPECT_TRUE((uint32_t)component->valid);
  EXPECT_EQ((uint32_t)component->opcode, 0b1100111);
  EXPECT_EQ((uint32_t)component->rd, 4);
  EXPECT_EQ((uint32_t)component->rs1, 5);
  EXPECT_EQ((uint32_t)component->imm, 0x123);
}

TEST_F(DecodeTest, Branches) {
  struct BranchDecodeResult {
    uint32_t valid, opcode, rs1, rs2, imm, alu_op, negate_alu_lsb;

    bool operator==(const BranchDecodeResult &other) const = default;
  };
  std::vector<std::pair<uint32_t, BranchDecodeResult>> test_cases = {
      // beq x6, x7, 0x456
      {0x44730b63, BranchDecodeResult({1, 0b1100011, 6, 7, 0x456, 10, 0})},

      // bne x8, x9, 0xabc
      {0x2a941ee3, BranchDecodeResult({1, 0b1100011, 8, 9, 0xabc, 10, 1})},

      // blt x10, x11, 0xffe
      {0x7eb54fe3, BranchDecodeResult({1, 0b1100011, 10, 11, 0xffe, 8, 0})},

      // bge x12, x13, 0x234
      {0x22d65a63, BranchDecodeResult({1, 0b1100011, 12, 13, 0x234, 8, 1})},

      // bltu x14, x15, 0x12
      {0x00f76963, BranchDecodeResult({1, 0b1100011, 14, 15, 0x12, 9, 0})},

      // bgeu x16, x17, 0x678
      {0x67187c63, BranchDecodeResult({1, 0b1100011, 16, 17, 0x678, 9, 1})},
  };

  for (auto &[inst, expected] : test_cases) {
    component->inst = inst;
    step();
    EXPECT_EQ(expected,
              BranchDecodeResult(
                  {(uint32_t)component->valid, (uint32_t)component->opcode,
                   (uint32_t)component->rs1, (uint32_t)component->rs2,
                   (uint32_t)component->imm, (uint32_t)component->alu_op,
                   (uint32_t)component->negate_alu_lsb}));
  }
}
