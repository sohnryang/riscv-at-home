module decode (
    input [31:0] inst,
    output reg valid,
    output reg [6:0] opcode,
    output reg [31:0] imm,
    output reg [4:0] rs1,
    output reg [4:0] rs2,
    output reg [4:0] rd,
    output reg [3:0] alu_op,
    output reg negate_alu_lsb,
    output reg [1:0] load_store_size,
    output reg load_high
);
  `include "alu_defs.v"
  `include "decode_defs.v"

  wire [2:0] funct3;
  wire [6:0] funct7, opcode_int;

  assign funct3 = inst[14:12];
  assign funct7 = inst[31:25];
  assign opcode_int = inst[6:0];

  always @(*) begin
    opcode = opcode_int;

    case (opcode_int)
      `OPCODE_LUI: begin
        rd = inst[11:7];
        imm = {inst[31:12], 12'b0};
        valid = 1;
      end

      `OPCODE_AUIPC: begin
        rd = inst[11:7];
        imm = {inst[31:12], 12'b0};
        valid = 1;
      end

      `OPCODE_JAL: begin
        rd = inst[11:7];
        imm = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:21], 1'b0};
        valid = 1;
      end

      `OPCODE_JALR: begin
        rd = inst[11:7];
        rs1 = inst[19:15];
        imm = {{21{inst[31]}}, inst[30:20]};
        valid = 1;
      end

      `OPCODE_BRANCH: begin
        rs1 = inst[19:15];
        rs2 = inst[24:20];
        imm = {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0};
        if (funct3[2:1] == 2'b01) valid = 0;
        else begin
          valid = 1;
          case (funct3[2:1])
            2'b00:   alu_op = `ALU_EQ;
            2'b10:   alu_op = `ALU_LT;
            default: alu_op = `ALU_LTU;
          endcase
          negate_alu_lsb = funct3[0];
        end
      end

      `OPCODE_LOAD: begin
        rd = inst[11:7];
        rs1 = inst[19:15];
        imm = {{21{inst[31]}}, inst[30:20]};
        load_store_size = funct3[1:0];
        load_high = funct3[2];
        if (funct3 == 3'b011 || funct3[2:1] == 2'b11) valid = 0;
        else valid = 1;
      end

      `OPCODE_STORE: begin
        rs1 = inst[19:15];
        rs2 = inst[24:20];
        imm = {{21{inst[31]}}, inst[30:25], inst[11:7]};
        load_store_size = funct3[1:0];
        if (funct3[1:0] == 2'b11 || funct3[2]) valid = 0;
        else valid = 1;
      end

      `OPCODE_OP_IMM: begin
        rd  = inst[11:7];
        rs1 = inst[19:15];

        if (funct3[1:0] == 2'b01) begin
          imm = {27'b0, inst[24:20]};
          if (funct3[2] && {funct7[6], funct7[4:0]} != 6'b0) valid = 0;
          else if (!funct3[2] && funct7 != 7'b0) valid = 0;
          else valid = 1;
        end else begin
          imm   = {{21{inst[31]}}, inst[30:20]};
          valid = 1;
        end

        case (funct3)
          3'b000:  alu_op = `ALU_ADD;
          3'b010:  alu_op = `ALU_LT;
          3'b011:  alu_op = `ALU_LTU;
          3'b100:  alu_op = `ALU_XOR;
          3'b110:  alu_op = `ALU_OR;
          3'b111:  alu_op = `ALU_AND;
          3'b001:  alu_op = `ALU_SLL;
          3'b101:  alu_op = `ALU_SRL;
          default: alu_op = `ALU_SRA;
        endcase
      end

      `OPCODE_OP: begin
        rd  = inst[11:7];
        rs1 = inst[19:15];
        rs2 = inst[24:20];

        if ((funct3 == 3'b0 || funct3 == 3'b101) && {funct7[6], funct7[4:0]} != 6'b0) valid = 0;
        else if (funct7 != 7'b0) valid = 0;
        else valid = 1;

        case (funct3)
          3'b000: begin
            if (funct7[5]) alu_op = `ALU_SUB;
            else alu_op = `ALU_ADD;
          end
          3'b001:  alu_op = `ALU_SLL;
          3'b010:  alu_op = `ALU_LT;
          3'b011:  alu_op = `ALU_LTU;
          3'b100:  alu_op = `ALU_XOR;
          3'b101: begin
            if (funct7[5]) alu_op = `ALU_SRA;
            else alu_op = `ALU_SRL;
          end
          3'b110:  alu_op = `ALU_OR;
          default: alu_op = `ALU_AND;
        endcase
      end

      `OPCODE_MISC_MEM: begin
        rd = inst[11:7];
        rs1 = inst[19:15];
        valid = 1;
      end

      default: valid = 0;
    endcase
  end
endmodule
// vim: set ft=verilog:
