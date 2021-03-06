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
set_msg_config -id {HDL-1065} -limit 10000
create_project -in_memory -part xc7vx485tffg1157-1

set_param project.singleFileAddWarning.threshold 0
set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_property webtalk.parent_dir F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/edit_AXI_TX_CONTROLLER_v1_0.cache/wt [current_project]
set_property parent.project_path F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/edit_AXI_TX_CONTROLLER_v1_0.xpr [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language Verilog [current_project]
set_property ip_repo_paths {
  f:/xapp1026/kc705_axieth_150mhz_64kb/vfat3_firmware/repo
  f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/AXI_TX_CONTROLLER_1.0
  f:/simulation/send_lv1a/ip_repo/myip_1.0
} [current_project]
set_property ip_output_repo f:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/edit_AXI_TX_CONTROLLER_v1_0.cache/ip [current_project]
set_property ip_cache_permissions {read write} [current_project]
read_verilog -library xil_defaultlib {
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/AXI_TX_CONTROLLER_1.0/hdl/AXI_TX_CONTROLLER_v1_0_S00_AXI.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/edit_AXI_TX_CONTROLLER_v1_0.srcs/sources_1/new/counter_logic.v
  F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/repo/AXI_TX_CONTROLLER_1.0/hdl/AXI_TX_CONTROLLER_v1_0.v
}
# Mark all dcp files as not used in implementation to prevent them from being
# stitched into the results of this synthesis run. Any black boxes in the
# design are intentionally left as such for best results. Dcp files will be
# stitched into the design at a later time, either when this synthesis run is
# opened, or when it is stitched into a dependent implementation run.
foreach dcp [get_files -quiet -all -filter file_type=="Design\ Checkpoint"] {
  set_property used_in_implementation false $dcp
}

synth_design -top AXI_TX_CONTROLLER_v1_0 -part xc7vx485tffg1157-1


# disable binary constraint mode for synth run checkpoints
set_param constraints.enableBinaryConstraints false
write_checkpoint -force -noxdef AXI_TX_CONTROLLER_v1_0.dcp
create_report "synth_1_synth_report_utilization_0" "report_utilization -file AXI_TX_CONTROLLER_v1_0_utilization_synth.rpt -pb AXI_TX_CONTROLLER_v1_0_utilization_synth.pb"
