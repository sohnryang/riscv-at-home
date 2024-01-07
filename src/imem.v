module imem (
    input [31:0] addr,
    output reg [31:0] inst
);
  reg [31:0] regs[0:1023]  /*verilator public */;

  always @(*) begin
    inst = regs[addr];
  end
endmodule
// vim: set ft=verilog:
