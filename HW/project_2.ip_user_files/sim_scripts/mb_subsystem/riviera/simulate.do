onbreak {quit -force}
onerror {quit -force}

asim -t 1ps +access +r +m+mb_subsystem -L xil_defaultlib -L xpm -L microblaze_v10_0_5 -L lmb_v10_v3_0_9 -L lmb_bram_if_cntlr_v4_0_14 -L axi_lite_ipif_v3_0_4 -L axi_intc_v4_1_10 -L xlconcat_v2_1_1 -L mdm_v3_2_12 -L lib_cdc_v1_0_2 -L proc_sys_reset_v5_0_12 -L lib_pkg_v1_0_2 -L axi_timer_v2_0_17 -L lib_srl_fifo_v1_0_2 -L axi_uartlite_v2_0_19 -L blk_mem_gen_v8_3_6 -L axi_bram_ctrl_v4_0_13 -L fifo_generator_v13_2_1 -L blk_mem_gen_v8_4_1 -L lib_bmg_v1_0_10 -L axi_ethernet_buffer_v2_0_17 -L xbip_utils_v3_0_8 -L xbip_pipe_v3_0_4 -L xbip_bram18k_v3_0_4 -L mult_gen_v12_0_13 -L tri_mode_ethernet_mac_v9_0_10 -L c_reg_fd_v12_0_4 -L c_mux_bit_v12_0_4 -L c_shift_ram_v12_0_11 -L xbip_dsp48_wrapper_v3_0_4 -L xbip_dsp48_addsub_v3_0_4 -L xbip_addsub_v3_0_4 -L c_addsub_v12_0_11 -L c_gate_bit_v12_0_4 -L xbip_counter_v3_0_4 -L c_counter_binary_v12_0_11 -L xlconstant_v1_1_3 -L util_vector_logic_v2_0_1 -L lib_fifo_v1_0_10 -L axi_datamover_v5_1_17 -L axi_sg_v4_1_8 -L axi_dma_v7_1_16 -L axi_fifo_mm_s_v4_1_12 -L interrupt_control_v3_1_4 -L axi_gpio_v2_0_17 -L axi_infrastructure_v1_1_0 -L axi_clock_converter_v2_1_14 -L axis_infrastructure_v1_1_0 -L axis_data_fifo_v1_1_16 -L axis_clock_converter_v1_1_16 -L xlslice_v1_0_1 -L axi_iic_v2_0_18 -L generic_baseblocks_v2_1_0 -L axi_register_slice_v2_1_15 -L axi_data_fifo_v2_1_14 -L axi_crossbar_v2_1_16 -L axi_protocol_converter_v2_1_15 -L unisims_ver -L unimacro_ver -L secureip -O5 xil_defaultlib.mb_subsystem xil_defaultlib.glbl

do {wave.do}

view wave
view structure

do {mb_subsystem.udo}

run -all

endsim

quit -force
