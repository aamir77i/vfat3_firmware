//Copyright 1986-2017 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2017.4 (win64) Build 2086221 Fri Dec 15 20:55:39 MST 2017
//Date        : Sat May 19 20:26:46 2018
//Host        : PCMICVFAT3 running 64-bit Service Pack 1  (build 7601)
//Command     : generate_target test_v1_0_bfm_2_wrapper.bd
//Design      : test_v1_0_bfm_2_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module test_v1_0_bfm_2_wrapper
   (ACLK,
    ARESETN);
  input ACLK;
  input ARESETN;

  wire ACLK;
  wire ARESETN;

  test_v1_0_bfm_2 test_v1_0_bfm_2_i
       (.ACLK(ACLK),
        .ARESETN(ARESETN));
endmodule
