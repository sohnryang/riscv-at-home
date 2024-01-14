#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

#include "Valu.h"
#include "test_utils.h"

class AluTest : public SingleComponentFixture<Valu, 5> {
protected:
  void init_component() override {
    component->op = 0;
    component->in1 = 0;
    component->in2 = 0;
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
    component->op = 0;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 1;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 2;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 3;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 4;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 5;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 6;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 7;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
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
    component->op = 8;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
  }
}

TEST_F(AluTest, LessThanUnsigned) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 0},          {{1, 0}, 0},
       {{0, 1}, 1},          {{1, 1}, 0},
       {{0x7fffffff, 1}, 0}, {{0x7fffffff, 0x80000000}, 1},
       {{0x80000000, 1}, 0}, {{0x80000000, 0x80000000}, 0}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    component->op = 9;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
  }
}

TEST_F(AluTest, Equal) {
  std::vector<std::tuple<std::tuple<uint32_t, uint32_t>, uint32_t>> test_cases =
      {{{0, 0}, 1},          {{1, 0}, 0},
       {{0, 1}, 0},          {{1, 1}, 1},
       {{0x7fffffff, 1}, 0}, {{0x7fffffff, 0x80000000}, 0},
       {{0x80000000, 1}, 0}, {{0x80000000, 0x80000000}, 1}};

  for (auto &[input, expected] : test_cases) {
    auto &[in1, in2] = input;
    component->op = 10;
    component->in1 = in1;
    component->in2 = in2;
    step();
    EXPECT_EQ((uint32_t)component->out, expected);
  }
}
