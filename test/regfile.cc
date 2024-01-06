#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>

#include <verilated.h>
#include <verilated_vcd_c.h>

#include "Vregfile.h"

class RegfileTest : public ::testing::Test {
protected:
  std::unique_ptr<VerilatedContext> ctx;
  std::unique_ptr<Vregfile> regfile;
  std::unique_ptr<VerilatedVcdC> traces;

  RegfileTest() {
    ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(true);

    regfile = std::make_unique<Vregfile>(ctx.get());
    regfile->clk = 0;

    traces = std::make_unique<VerilatedVcdC>();
    regfile->trace(traces.get(), 5);

    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string trace_filename = std::string(test_info->test_case_name()) +
                                 "_" + std::string(test_info->name()) + ".vcd";
    traces->open(trace_filename.c_str());
  }

  ~RegfileTest() {
    traces->dump(ctx->time());
    regfile->final();
    traces->close();
  }

  void step() {
    regfile->eval();
    traces->dump(ctx->time());
    ctx->timeInc(1);
  }

  void reset() {
    for (int i = 0; i < 4; i++) {
      regfile->clk ^= 1;
      regfile->rst = 1;
      step();
    }
  }
};

TEST_F(RegfileTest, Reset) {
  reset();

  regfile->read_idx1 = 1;
  regfile->rst = 0;

  for (int i = 1; i < 32; i++) {
    regfile->read_idx2 = i;
    EXPECT_EQ((uint32_t)regfile->read_data2, 0);
    step();
  }
}

TEST_F(RegfileTest, Write) {
  reset();

  regfile->clk ^= 1;
  regfile->write_idx = 1;
  regfile->write_data = 0x12345678;
  regfile->write_enable = 1;
  regfile->rst = 0;
  step();

  regfile->clk ^= 1;
  regfile->write_enable = 0;
  step();

  for (int i = 1; i < 32; i++) {
    regfile->clk ^= 1;
    regfile->read_idx1 = i;
    regfile->read_idx2 = i;
    step();

    if (i == 1) {
      EXPECT_EQ((uint32_t)regfile->read_data1, 0x12345678);
      EXPECT_EQ((uint32_t)regfile->read_data2, 0x12345678);
    } else {
      EXPECT_EQ((uint32_t)regfile->read_data1, 0);
      EXPECT_EQ((uint32_t)regfile->read_data2, 0);
    }

    regfile->clk ^= 1;
    step();
  }
}
