verilate(alu_test SOURCES alu.v TRACE)
verilate(regfile_test SOURCES regfile.v TRACE)
verilate(imem_test SOURCES imem.v TRACE VERILATOR_ARGS --vpi)
verilate(decode_test SOURCES decode.v TRACE)
