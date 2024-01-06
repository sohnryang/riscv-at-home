#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Valu.h"

class AluTest : public ::testing::Test {
protected:
  std::unique_ptr<VerilatedContext> ctx;
  std::unique_ptr<Valu> alu;
  std::unique_ptr<VerilatedVcdC> traces;

  AluTest() {
    ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(true);

    alu = std::make_unique<Valu>(ctx.get());
    alu->op = 0;
    alu->in1 = 0;
    alu->in2 = 0;

    traces = std::make_unique<VerilatedVcdC>();
    alu->trace(traces.get(), 5);

    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string trace_filename = std::string(test_info->test_case_name()) +
                                 "_" + test_info->name() + ".vcd";
    traces->open(trace_filename.c_str());
  }

  ~AluTest() {
    traces->dump(ctx->time());
    alu->final();
    traces->close();
  }

  void step() {
    alu->eval();
    traces->dump(ctx->time());
    ctx->timeInc(1);
  }
};

TEST_F(AluTest, Addition) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 1},
       {{1, 1}, 2},
       {{0x7fffffff, 1}, 0x80000000},
       {{0x80000000, 1}, 0x80000001},
       {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 0;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, Substraction) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 0xffffffff},
       {{1, 1}, 0},
       {{0, -1}, 1},
       {{0x7fffffff, 1}, 0x7ffffffe},
       {{0x80000000, 1}, 0x7fffffff},
       {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 1;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, And) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 0},
       {{0, 1}, 0},
       {{1, 1}, 1},
       {{0x7fffffff, 1}, 1},
       {{0x80000000, 1}, 0},
       {{0x80000000, 0x80000000}, 0x80000000}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 2;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, Or) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 1},
       {{1, 1}, 1},
       {{0x7fffffff, 1}, 0x7fffffff},
       {{0x80000000, 1}, 0x80000001},
       {{0x80000000, 0x80000000}, 0x80000000}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 3;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, Xor) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 1},
       {{1, 1}, 0},
       {{0x7fffffff, 1}, 0x7ffffffe},
       {{0x80000000, 1}, 0x80000001},
       {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 4;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, ShiftLeft) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 0},
       {{1, 1}, 2},
       {{0x7fffffff, 1}, 0xfffffffe},
       {{0x7fffffff, 4}, 0xfffffff0},
       {{0x80000000, 1}, 0},
       {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 5;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, ShiftRightLogical) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 0},
       {{1, 1}, 0},
       {{0x7fffffff, 1}, 0x3fffffff},
       {{0x7fffffff, 4}, 0x07ffffff},
       {{0x80000000, 1}, 0x40000000},
       {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 6;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, ShiftRightArithmetic) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},
       {{1, 0}, 1},
       {{0, 1}, 0},
       {{1, 1}, 0},
       {{0x7fffffff, 1}, 0x3fffffff},
       {{0x7fffffff, 4}, 0x07ffffff},
       {{0x80000000, 1}, 0xc0000000},
       {{0x80000000, 0x80000000}, 0xffffffff}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 7;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)alu->out, expected);
  }
}

TEST_F(AluTest, LessThan) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},          {{1, 0}, 0},
       {{0, 1}, 1},          {{1, 1}, 0},
       {{0x7fffffff, 1}, 0}, {{0x7fffffff, 0x80000000}, 0},
       {{0x80000000, 1}, 1}, {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    alu->op = 8;
    alu->in1 = in1;
    alu->in2 = in2;
    step();
    EXPECT_EQ(((uint32_t)alu->out) & 1, expected);
  }
}
