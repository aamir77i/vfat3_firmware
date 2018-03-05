# 
# Synthesis run script generated by Vivado
# 

proc create_report { reportName command } {
  set status "."
  append status $reportName ".fail"
  if { [file exists $status] } {
    eval file delete [glob $status]
  }
  send_msg_id runtcl-4 info "Executing : $command"
  set retval [eval catch { $command } msg]
  if { $retval != 0 } {
    set fp [open $status w]
    close $fp
    send_msg_id runtcl-5 warning "$msg"
  }
}
set_param tcl.collectionResultDisplayLimit 0
set_param xicom.use_bs_reader 1
create_project -in_memory -part xc7k325tffg900-2

set_param project.singleFileAddWarning.threshold 0
set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_msg_config -source 4 -id {IP_Flow 19-2162} -severity warning -new_severity info
set_property webtalk.parent_dir F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.cache/wt [current_project]
set_property parent.project_path F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.xpr [current_project]
set_property XPM_LIBRARIES {XPM_CDC XPM_FIFO XPM_MEMORY} [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language Verilog [current_project]
set_property board_part xilinx.com:kc705:part0:1.0 [current_project]
set_property ip_cache_permissions disable [current_project]
add_files F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.sdk/vfat3_sdk/Debug/vfat3_sdk.elf
set_property SCOPED_TO_REF mb_subsystem [get_files -all F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.sdk/vfat3_sdk/Debug/vfat3_sdk.elf]
set_property SCOPED_TO_CELLS microblaze_0 [get_files -all F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.sdk/vfat3_sdk/Debug/vfat3_sdk.elf]
read_verilog -library xil_defaultlib {
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/imports/new/DIFF_To_single.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/imports/new/AP_Generator.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/new/bit_slip_Generator.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/new/inverse_reverse.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/imports/new/S_to_diff.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/imports/bitslip/F1_F2_FILTER.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/hdl/mb_subsystem_wrapper.v
}
add_files F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/mb_subsystem.bd
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_microblaze_0_0/mb_subsystem_microblaze_0_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_microblaze_0_0/mb_subsystem_microblaze_0_0_ooc_debug.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_microblaze_0_0/mb_subsystem_microblaze_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_mig_7series_0_0/mb_subsystem_mig_7series_0_0/user_design/constraints/mb_subsystem_mig_7series_0_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_mig_7series_0_0/mb_subsystem_mig_7series_0_0/user_design/constraints/mb_subsystem_mig_7series_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_mig_7series_0_0/mb_subsystem_mig_7series_0_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_dlmb_v10_0/mb_subsystem_dlmb_v10_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_dlmb_v10_0/mb_subsystem_dlmb_v10_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ilmb_v10_1/mb_subsystem_ilmb_v10_1.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ilmb_v10_1/mb_subsystem_ilmb_v10_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_dlmb_bram_if_cntlr_0/mb_subsystem_dlmb_bram_if_cntlr_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ilmb_bram_if_cntlr_1/mb_subsystem_ilmb_bram_if_cntlr_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_microblaze_0_axi_intc_0/mb_subsystem_microblaze_0_axi_intc_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_microblaze_0_axi_intc_0/mb_subsystem_microblaze_0_axi_intc_0_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_microblaze_0_axi_intc_0/mb_subsystem_microblaze_0_axi_intc_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_mdm_1_0/mb_subsystem_mdm_1_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_mdm_1_0/mb_subsystem_mdm_1_0_ooc_trace.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_proc_sys_reset_1_0/mb_subsystem_proc_sys_reset_1_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_proc_sys_reset_1_0/mb_subsystem_proc_sys_reset_1_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_proc_sys_reset_1_0/mb_subsystem_proc_sys_reset_1_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_timer_0_0/mb_subsystem_axi_timer_0_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_timer_0_0/mb_subsystem_axi_timer_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_uartlite_0_0/mb_subsystem_axi_uartlite_0_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_uartlite_0_0/mb_subsystem_axi_uartlite_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_uartlite_0_0/mb_subsystem_axi_uartlite_0_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_bram_ctrl_0_0/mb_subsystem_axi_bram_ctrl_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/ip/ip_0/bd_1ed2_eth_buf_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/ip/ip_0/synth/bd_1ed2_eth_buf_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/ip/ip_1/synth/bd_1ed2_mac_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/ip/ip_1/synth/bd_1ed2_mac_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/ip/ip_1/synth/bd_1ed2_mac_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/ip/ip_1/synth/bd_1ed2_mac_0_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/bd_0/bd_1ed2_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/mb_subsystem_axi_ethernet_0_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_0/synth/mb_subsystem_axi_ethernet_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_dma_0/mb_subsystem_axi_ethernet_0_dma_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_dma_0/mb_subsystem_axi_ethernet_0_dma_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_ethernet_0_dma_0/mb_subsystem_axi_ethernet_0_dma_0_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_clk_wiz_0_2/mb_subsystem_clk_wiz_0_2_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_clk_wiz_0_2/mb_subsystem_clk_wiz_0_2.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_clk_wiz_0_2/mb_subsystem_clk_wiz_0_2_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_clk_wiz_1_0/mb_subsystem_clk_wiz_1_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_clk_wiz_1_0/mb_subsystem_clk_wiz_1_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_clk_wiz_1_0/mb_subsystem_clk_wiz_1_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_selectio_wiz_0_0/mb_subsystem_selectio_wiz_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_selectio_wiz_0_0/mb_subsystem_selectio_wiz_0_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_fifo_mm_s_0_0/mb_subsystem_axi_fifo_mm_s_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_SERIALIZER_0/mb_subsystem_TX_SERIALIZER_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_SERIALIZER_0/mb_subsystem_TX_SERIALIZER_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_1/mb_subsystem_axi_gpio_0_1_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_1/mb_subsystem_axi_gpio_0_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_1/mb_subsystem_axi_gpio_0_1.xdc]
set_property used_in_synthesis false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_0/mb_subsystem_axi_clock_converter_0_0_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_0/mb_subsystem_axi_clock_converter_0_0_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_0/mb_subsystem_axi_clock_converter_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axis_clock_converter_0_1/mb_subsystem_axis_clock_converter_0_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axis_clock_converter_0_1/mb_subsystem_axis_clock_converter_0_1/mb_subsystem_axis_clock_converter_0_1.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axis_clock_converter_0_1/mb_subsystem_axis_clock_converter_0_1/mb_subsystem_axis_clock_converter_0_1_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axis_clock_converter_0_0/mb_subsystem_axis_clock_converter_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axis_clock_converter_0_0/mb_subsystem_axis_clock_converter_0_0/mb_subsystem_axis_clock_converter_0_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axis_clock_converter_0_0/mb_subsystem_axis_clock_converter_0_0/mb_subsystem_axis_clock_converter_0_0_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_2/mb_subsystem_axi_gpio_0_2_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_2/mb_subsystem_axi_gpio_0_2_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_2/mb_subsystem_axi_gpio_0_2.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_3/mb_subsystem_axi_gpio_0_3_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_3/mb_subsystem_axi_gpio_0_3_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_3/mb_subsystem_axi_gpio_0_3.xdc]
set_property used_in_synthesis false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_2/mb_subsystem_axi_clock_converter_0_2_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_2/mb_subsystem_axi_clock_converter_0_2_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_2/mb_subsystem_axi_clock_converter_0_2_ooc.xdc]
set_property used_in_synthesis false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_1/mb_subsystem_axi_clock_converter_0_1_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_1/mb_subsystem_axi_clock_converter_0_1_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_1/mb_subsystem_axi_clock_converter_0_1_ooc.xdc]
set_property used_in_synthesis false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_4/mb_subsystem_axi_clock_converter_0_4_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_4/mb_subsystem_axi_clock_converter_0_4_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_4/mb_subsystem_axi_clock_converter_0_4_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_invert_1/mb_subsystem_TX_invert_1_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_invert_1/mb_subsystem_TX_invert_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_invert_1/mb_subsystem_TX_invert_1.xdc]
set_property used_in_synthesis false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_3/mb_subsystem_axi_clock_converter_0_3_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_3/mb_subsystem_axi_clock_converter_0_3_clocks.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_clock_converter_0_3/mb_subsystem_axi_clock_converter_0_3_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_invert_0/mb_subsystem_TX_invert_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_invert_0/mb_subsystem_TX_invert_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_TX_invert_0/mb_subsystem_TX_invert_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_rst_clk_wiz_0_125M_0/mb_subsystem_rst_clk_wiz_0_125M_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_rst_clk_wiz_0_125M_0/mb_subsystem_rst_clk_wiz_0_125M_0.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_rst_clk_wiz_0_125M_0/mb_subsystem_rst_clk_wiz_0_125M_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_0/mb_subsystem_axi_gpio_0_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_0/mb_subsystem_axi_gpio_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_gpio_0_0/mb_subsystem_axi_gpio_0_0.xdc]
set_property used_in_synthesis false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ila_0_1/ila_v6_2/constraints/ila_impl.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ila_0_1/ila_v6_2/constraints/ila_impl.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ila_0_1/ila_v6_2/constraints/ila.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_ila_0_1/mb_subsystem_ila_0_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_iic_0_0/mb_subsystem_axi_iic_0_0_board.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_axi_iic_0_0/mb_subsystem_axi_iic_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_blk_mem_gen_0_0/mb_subsystem_blk_mem_gen_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_lmb_bram_1/mb_subsystem_lmb_bram_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_xbar_1/mb_subsystem_xbar_1_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_xbar_0/mb_subsystem_xbar_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_m01_regslice_0/mb_subsystem_m01_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_m00_regslice_0/mb_subsystem_m00_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s04_regslice_0/mb_subsystem_s04_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s04_data_fifo_0/mb_subsystem_s04_data_fifo_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s03_regslice_0/mb_subsystem_s03_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s03_data_fifo_0/mb_subsystem_s03_data_fifo_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s02_regslice_0/mb_subsystem_s02_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s02_data_fifo_0/mb_subsystem_s02_data_fifo_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s01_regslice_0/mb_subsystem_s01_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s01_data_fifo_0/mb_subsystem_s01_data_fifo_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s00_regslice_0/mb_subsystem_s00_regslice_0_ooc.xdc]
set_property used_in_implementation false [get_files -all f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/ip/mb_subsystem_s00_data_fifo_0/mb_subsystem_s00_data_fifo_0_ooc.xdc]
set_property used_in_implementation false [get_files -all F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/mb_subsystem_ooc.xdc]

# Mark all dcp files as not used in implementation to prevent them from being
# stitched into the results of this synthesis run. Any black boxes in the
# design are intentionally left as such for best results. Dcp files will be
# stitched into the design at a later time, either when this synthesis run is
# opened, or when it is stitched into a dependent implementation run.
foreach dcp [get_files -quiet -all -filter file_type=="Design\ Checkpoint"] {
  set_property used_in_implementation false $dcp
}
read_xdc F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/constrs_1/new/system.xdc
set_property used_in_implementation false [get_files F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/constrs_1/new/system.xdc]

read_xdc dont_touch.xdc
set_property used_in_implementation false [get_files dont_touch.xdc]

synth_design -top mb_subsystem_wrapper -part xc7k325tffg900-2


# disable binary constraint mode for synth run checkpoints
set_param constraints.enableBinaryConstraints false
write_checkpoint -force -noxdef mb_subsystem_wrapper.dcp
create_report "synth_1_synth_report_utilization_0" "report_utilization -file mb_subsystem_wrapper_utilization_synth.rpt -pb mb_subsystem_wrapper_utilization_synth.pb"
