#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <vector>

#include <verilated_vpi.h>

#include "Vimem.h"
#include "test_utils.h"

class ImemTest : public SingleComponentFixture<Vimem, 5> {
protected:
  void init_component() override {}

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
    component->addr = i;
    step();
    EXPECT_EQ((uint32_t)component->inst, mem[i]);
  }
}
