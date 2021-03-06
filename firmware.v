module firmware(
      input              CLOCK2_50,
      input              CLOCK3_50,
      inout              CLOCK4_50,
      input              CLOCK_50,

      output      [12:0] DRAM_ADDR,
      output      [1:0]  DRAM_BA,
      output             DRAM_CAS_N,
      output             DRAM_CKE,
      output             DRAM_CLK,
      output             DRAM_CS_N,
      inout       [15:0] DRAM_DQ,
      output             DRAM_LDQM,
      output             DRAM_RAS_N,
      output             DRAM_UDQM,
      output             DRAM_WE_N,

      inout       [35:0] GPIO_0,
      inout       [35:0] GPIO_1,

      output      [6:0]  HEX0,
      output      [6:0]  HEX1,
      output      [6:0]  HEX2,
      output      [6:0]  HEX3,
      output      [6:0]  HEX4,
      output      [6:0]  HEX5,

      input       [3:0]  KEY,

      output      [9:0]  LEDR,

      inout              PS2_CLK,
      inout              PS2_CLK2,
      inout              PS2_DAT,
      inout              PS2_DAT2,

      input              RESET_N,

      output             SD_CLK,
      inout              SD_CMD,
      inout       [3:0]  SD_DATA,

      input       [9:0]  SW,

      output      [3:0]  VGA_B,
      output      [3:0]  VGA_G,
      output             VGA_HS,
      output      [3:0]  VGA_R,
      output             VGA_VS
);

softcore softcore (
        .clk_clk                                (CLOCK_50),
        .sd_clk_export                          (SD_CLK),
        .sd_cmd_export                          (SD_CMD),
        .sd_dat_export                          (SD_DATA),
        .reset_reset_n                          (1'b1)
    );

endmodule