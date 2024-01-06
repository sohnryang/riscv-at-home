#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>

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
    alu->final();
    traces->close();
  }

  void step() {
    alu->eval();
    traces->dump(ctx->time());
    ctx->timeInc(1);
  }
};

TEST_F(AluTest, TestAddition) {
  alu->op = 0;
  alu->in1 = 0;
  alu->in2 = 0;
  step();
  EXPECT_EQ((uint32_t)alu->out, 0);

  alu->op = 0;
  alu->in1 = 1;
  alu->in2 = 0;
  step();
  EXPECT_EQ((uint32_t)alu->out, 1);

  alu->op = 0;
  alu->in1 = 0;
  alu->in2 = 1;
  step();
  EXPECT_EQ((uint32_t)alu->out, 1);

  alu->op = 0;
  alu->in1 = 1;
  alu->in2 = 1;
  step();
  EXPECT_EQ((uint32_t)alu->out, 2);

  alu->op = 0;
  alu->in1 = 0x7fffffff;
  alu->in2 = 1;
  step();
  EXPECT_EQ((uint32_t)alu->out, 0x80000000);

  alu->op = 0;
  alu->in1 = 0x80000000;
  alu->in2 = 1;
  step();
  EXPECT_EQ((uint32_t)alu->out, 0x80000001);

  alu->op = 0;
  alu->in1 = 0x80000000;
  alu->in2 = 0x80000000;
  step();
  EXPECT_EQ((uint32_t)alu->out, 0);
}
