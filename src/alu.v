module alu (
    input [3:0] op,
    input [31:0] in1,
    input [31:0] in2,
    output reg [31:0] out
);
  `include "alu_defs.v"

  always @(*) begin
    case (op)
      `ALU_ADD: out = in1 + in2;
      `ALU_SUB: out = in1 - in2;
      `ALU_AND: out = in1 & in2;
      `ALU_OR:  out = in1 | in2;
      `ALU_XOR: out = in1 ^ in2;
      `ALU_SLL: out = in1 << in2;
      `ALU_SRL: out = in1 >> in2;
      `ALU_SRA: out = $signed(in1) >>> in2;
      `ALU_LT:  out[0] = $signed(in1) < $signed(in2);
      `ALU_LTU: out[0] = in1 < in2;
      default:  out = 0;
    endcase
  end
endmodule
// vim: set ft=verilog:
