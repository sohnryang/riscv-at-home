#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <verilated.h>
#include <verilated_vcd_c.h>
#include <verilated_vpi.h>

#include "Vimem.h"

class ImemTest : public ::testing::Test {
protected:
  std::unique_ptr<VerilatedContext> ctx;
  std::unique_ptr<Vimem> imem;
  std::unique_ptr<VerilatedVcdC> traces;

  ImemTest() {
    ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(true);

    imem = std::make_unique<Vimem>(ctx.get());

    traces = std::make_unique<VerilatedVcdC>();
    imem->trace(traces.get(), 5);

    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string trace_filename = std::string(test_info->test_case_name()) +
                                 "_" + std::string(test_info->name()) + ".vcd";
    traces->open(trace_filename.c_str());
  }

  ~ImemTest() {
    traces->dump(ctx->time());
    imem->final();
    traces->close();
  }

  void step() {
    imem->eval();
    traces->dump(ctx->time());
    ctx->timeInc(1);
  }

  void write_mem(const std::vector<uint32_t> &mem) {
    vpiHandle regs_handle =
        vpi_handle_by_name((PLI_BYTE8 *)"TOP.imem.regs", nullptr);
    if (regs_handle == nullptr) {
      throw std::runtime_error("Could not find imem.regs");
    }

    for (int i = 0; i < mem.size(); i++) {
      vpiHandle reg_handle = vpi_handle_by_index(regs_handle, i);
      if (reg_handle == nullptr) {
        throw std::runtime_error("Could not find imem.regs[" +
                                 std::to_string(i) + "]");
      }
      t_vpi_value value;
      value.format = vpiIntVal;
      value.value.integer = mem[i];
      vpi_put_value(reg_handle, &value, nullptr, vpiNoDelay);
    }
  }
};

TEST_F(ImemTest, Read) {
  std::vector<uint32_t> mem = {0xdeadbeef, 0xbaadf00d, 0x01234567, 0x89abcdef};
  write_mem(mem);
  step();

  for (int i = 0; i < mem.size(); i++) {
    imem->addr = i;
    step();
    EXPECT_EQ((uint32_t)imem->read_data, mem[i]);
  }
}
