#include <gtest/gtest.h>

#include <memory>

#include <verilated.h>
#include <verilated_vcd_c.h>

template <typename T, int trace_levels = 99>
class SingleComponentFixture : public ::testing::Test {
protected:
  std::unique_ptr<VerilatedContext> ctx;
  std::unique_ptr<T> component;
  std::unique_ptr<VerilatedVcdC> traces;

  void SetUp() override {
    ctx = std::make_unique<VerilatedContext>();
    ctx->traceEverOn(true);

    component = std::make_unique<T>(ctx.get());
    init_component();

    traces = std::make_unique<VerilatedVcdC>();
    component->trace(traces.get(), trace_levels);

    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();
    std::string trace_filename = std::string(test_info->test_case_name()) +
                                 "_" + test_info->name() + ".vcd";
    traces->open(trace_filename.c_str());
  }

  void TearDown() override {
    traces->dump(ctx->time());
    component->final();
    traces->close();
  }

  virtual void step() {
    component->eval();
    traces->dump(ctx->time());
    ctx->timeInc(1);
  }

  virtual void init_component() = 0;
};
