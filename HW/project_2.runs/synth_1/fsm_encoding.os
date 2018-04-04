
 add_fsm_encoding \
       {wr_chnl.GEN_WDATA_SM_NO_ECC_DUAL_REG_WREADY.wr_data_sm_cs} \
       { }  \
       {{000 000} {001 100} {010 011} {011 001} {100 010} }

 add_fsm_encoding \
       {rd_chnl.rlast_sm_cs} \
       { }  \
       {{000 000} {001 010} {010 011} {011 100} {100 001} }

 add_fsm_encoding \
       {rd_chnl.rd_data_sm_cs} \
       { }  \
       {{0000 0000} {0001 0001} {0010 0010} {0011 0011} {0100 0100} {0101 0101} {0110 0110} {0111 0111} {1000 1000} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__2.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__2.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__1.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__1.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__1.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__1.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__1.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__1.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__1.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__1.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {rx_axistream_if.rxs_axistream_current_state} \
       { }  \
       {{00000 00000} {00001 00001} {00010 00010} {00011 00011} {00100 00100} {00101 01010} {00110 00111} {00111 00101} {01000 00110} {01001 01001} {01010 01000} {01011 01011} {01100 01100} {01101 01101} {01110 01110} {01111 01111} {10000 10000} {10001 10001} {10010 10010} {10011 10011} {10100 10100} {10101 10101} {10110 10110} {10111 10111} {11000 11000} {11001 11001} {11010 11010} {11011 11011} }

 add_fsm_encoding \
       {rx_axistream_if.rxd_axistream_current_state} \
       { }  \
       {{0000 0000} {0001 0001} {0010 0010} {0011 0101} {0100 0110} {0101 0111} {0110 1000} {0111 0011} {1000 0100} }

 add_fsm_encoding \
       {rx_emac_if.receive_frame_current_state} \
       { }  \
       {{0000 0000} {0001 0001} {0010 0010} {0011 0011} {0100 0100} {0101 1011} {0110 0101} {0111 0110} {1000 0111} {1001 1000} {1010 1001} {1011 1010} {1100 1100} }

 add_fsm_encoding \
       {tx_csum_full_fsm.fcsum_wr_cs} \
       { }  \
       {{0000 0000} {0001 0001} {0010 0010} {0011 0011} {0100 0100} {0101 0101} {0110 0110} {0111 0111} {1000 1000} {1001 1001} {1010 1100} {1011 1110} {1100 1101} {1101 1111} {1110 1010} {1111 1011} }

 add_fsm_encoding \
       {tx_csum_full_if.txd_wr_cs} \
       { }  \
       {{0000 000000001} {0001 000000010} {0010 000000100} {0011 000001000} {0100 010000000} {0101 000100000} {0110 001000000} {0111 000010000} {1000 100000000} }

 add_fsm_encoding \
       {tx_csum_full_if.txc_wr_cs} \
       { }  \
       {{0000 0000} {0001 0001} {0010 0010} {0011 0011} {0100 0100} {0101 1111} {0110 0101} {0111 0110} {1000 0111} {1001 1001} {1010 1000} {1011 1010} {1100 1110} {1101 1101} {1110 1100} {1111 1011} }

 add_fsm_encoding \
       {tx_emac_if.txc_rd_cs} \
       { }  \
       {{000 000} {001 001} {010 010} {011 011} {100 100} {101 101} {110 110} {111 111} }

 add_fsm_encoding \
       {tx_emac_if.txd_rd_cs} \
       { }  \
       {{00000 0000} {01001 0001} {01010 0010} {01011 0011} {01100 0100} {01101 0101} {01110 0110} {10000 0111} {10001 1000} }

 add_fsm_encoding \
       {bd_1ed2_mac_0_support_resets.idelay_reset_cnt} \
       { }  \
       {{0000 0000000000001} {0001 0000000000010} {0010 0000000000100} {0011 0000000001000} {0100 0000000010000} {0101 0000000100000} {0110 0000001000000} {0111 0000010000000} {1000 0000100000000} {1001 0001000000000} {1010 0010000000000} {1011 0100000000000} {1100 1000000000000} }

 add_fsm_encoding \
       {tri_mode_ethernet_mac_v9_0_10_pfc_tx_cntl.legacy_state} \
       { }  \
       {{000 0001} {001 0010} {010 0100} {011 1000} }

 add_fsm_encoding \
       {axi_datamover_pcc.sig_pcc_sm_state} \
       { }  \
       {{000 000} {001 001} {010 010} {011 011} {100 100} {101 101} {110 110} {111 111} }

 add_fsm_encoding \
       {axi_datamover_pcc__parameterized0.sig_pcc_sm_state} \
       { }  \
       {{000 000} {001 001} {010 010} {011 011} {100 100} {101 101} {110 110} {111 111} }

 add_fsm_encoding \
       {axi_datamover_s2mm_realign.sig_cmdcntl_sm_state} \
       { }  \
       {{000 000} {001 001} {010 010} {011 011} {101 100} }

 add_fsm_encoding \
       {axi_dma_mm2s_sm.mm2s_cs} \
       { }  \
       {{00 00} {01 01} {11 10} }

 add_fsm_encoding \
       {axi_dma_s2mm_sm.GEN_SM_FOR_LENGTH.s2mm_cs} \
       { }  \
       {{000 000} {001 001} {010 010} {011 011} {100 100} }

 add_fsm_encoding \
       {iic_control.scl_state} \
       { }  \
       {{0000 0000} {0001 0001} {0010 0010} {0011 0011} {0100 0100} {0101 0101} {0110 0110} {0111 0111} {1000 1000} {1001 1001} }

 add_fsm_encoding \
       {iic_control.state} \
       { }  \
       {{000 0000001} {001 0000010} {010 0000100} {011 0001000} {100 0100000} {101 0010000} {110 1000000} }

 add_fsm_encoding \
       {axi_data_fifo_v2_1_14_axic_reg_srl_fifo.state} \
       { }  \
       {{00 0100} {01 1000} {10 0001} {11 0010} }

 add_fsm_encoding \
       {axi_data_fifo_v2_1_14_axic_reg_srl_fifo__parameterized0.state} \
       { }  \
       {{00 0100} {01 1000} {10 0001} {11 0010} }

 add_fsm_encoding \
       {axi_data_fifo_v2_1_14_axic_reg_srl_fifo__parameterized1.state} \
       { }  \
       {{00 0100} {01 1000} {10 0001} {11 0010} }

 add_fsm_encoding \
       {axi_data_fifo_v2_1_14_axic_reg_srl_fifo__parameterized2.state} \
       { }  \
       {{00 0100} {01 1000} {10 0001} {11 0010} }

 add_fsm_encoding \
       {MMU_UTLB.State} \
       { }  \
       {{00000 00000} {00001 00001} {00010 00110} {00011 01001} {00100 00111} {00101 01011} {00110 01000} {00111 01010} {01000 00011} {01001 01100} {01010 01110} {01011 01101} {01100 01111} {01101 10000} {01110 00010} {01111 10001} {10000 00100} {10001 00101} {11111 10010} }

 add_fsm_encoding \
       {mig_7series_v4_0_tempmon.xadc_supplied_temperature.tempmon_state} \
       { }  \
       {{000 0001} {001 0010} {010 0100} {011 1000} }

 add_fsm_encoding \
       {mig_7series_v4_0_ddr_phy_wrlvl.wl_state_r} \
       { }  \
       {{00000 00000} {00001 00001} {00010 00010} {00011 00101} {00100 00110} {00101 01000} {00110 01010} {00111 01011} {01000 00111} {01001 11000} {01010 11011} {01011 11001} {01100 11010} {01101 10001} {01110 00011} {01111 01111} {10000 00100} {10001 10101} {10010 10110} {10011 10111} {10100 10000} {10101 10010} {10110 10011} {10111 01100} {11000 01101} {11001 01001} {11010 01110} {11011 10100} }

 add_fsm_encoding \
       {mig_7series_v4_0_ddr_phy_ocd_cntlr.sm_r} \
       { }  \
       {{000 000} {001 001} {010 010} {011 011} {100 100} {101 101} {110 110} {111 111} }

 add_fsm_encoding \
       {mig_7series_v4_0_ddr_phy_dqs_found_cal.fine_adj_state_r} \
       { }  \
       {{0000 0000} {0001 0011} {0010 0100} {0011 0001} {0100 0101} {0101 0110} {0110 0111} {0111 1000} {1000 1001} {1001 1010} {1010 1111} {1011 1011} {1100 1100} {1101 1101} {1110 1110} {1111 0010} }

 add_fsm_encoding \
       {mig_7series_v4_0_ddr_phy_rdlvl.cal1_state_r} \
       { }  \
       {{000000 000000} {000001 000010} {000010 000101} {000011 010001} {000100 010100} {000101 010110} {000110 010111} {000111 011000} {001000 010011} {001001 011011} {001010 011100} {001011 011001} {001100 000100} {001101 011110} {001110 011111} {001111 100001} {010000 000110} {010001 000111} {010010 001001} {010011 001011} {010100 001000} {010101 001100} {010110 001101} {010111 001110} {011000 001111} {011001 001010} {011010 010000} {011011 100010} {011100 010101} {011101 000001} {011110 010010} {011111 000011} {100000 100000} {100001 011010} {100010 011101} {111111 100011} }

 add_fsm_encoding \
       {mig_7series_v4_0_axi_mc_r_channel.state} \
       { }  \
       {{00 11} {01 10} {10 00} {11 01} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__4.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__4.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__3.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__3.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__2.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__2.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__2.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__2.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__2.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__2.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized3.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized3.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized3__xdcDup__1.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized3__xdcDup__1.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__3.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__3.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__3.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__3.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__3.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__3.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__6.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__6.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__5.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__5.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__4.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0__xdcDup__4.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__4.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1__xdcDup__4.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__4.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2__xdcDup__4.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__7.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__xdcDup__7.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized0.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized1.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2.src_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }

 add_fsm_encoding \
       {axi_clock_converter_v2_1_14_lite_async__parameterized2.dest_state} \
       { }  \
       {{00 00} {01 01} {10 10} {11 11} }
