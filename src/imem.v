module imem (
    input [31:0] addr,
    output reg [31:0] read_data
);
  reg [31:0] regs[0:1023]  /*verilator public */;

  always @(*) begin
    read_data = regs[addr];
  end
endmodule
// vim: set ft=verilog:
