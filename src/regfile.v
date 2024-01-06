module regfile (
    input clk,
    input rst,
    input [4:0] read_idx1,
    input [4:0] read_idx2,
    input write_enable,
    input [4:0] write_idx,
    input [31:0] write_data,
    output reg [31:0] read_data1,
    output reg [31:0] read_data2
);
  reg [31:0] regs[0:31];

  always @(posedge clk, posedge rst) begin
    if (rst) begin
      read_data1 = 0;
      read_data2 = 0;
      for (integer i = 0; i < 32; i = i + 1) regs[i] = 0;
    end else if (write_enable) regs[write_idx] = write_data;
  end

  always @(*) begin
    read_data1 <= regs[read_idx1];
    read_data2 <= regs[read_idx2];
  end
endmodule
// vim: set ft=verilog:
