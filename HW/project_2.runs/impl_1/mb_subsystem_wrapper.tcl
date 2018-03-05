# 
# Report generation script generated by Vivado
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
proc start_step { step } {
  set stopFile ".stop.rst"
  if {[file isfile .stop.rst]} {
    puts ""
    puts "*** Halting run - EA reset detected ***"
    puts ""
    puts ""
    return -code error
  }
  set beginFile ".$step.begin.rst"
  set platform "$::tcl_platform(platform)"
  set user "$::tcl_platform(user)"
  set pid [pid]
  set host ""
  if { [string equal $platform unix] } {
    if { [info exist ::env(HOSTNAME)] } {
      set host $::env(HOSTNAME)
    }
  } else {
    if { [info exist ::env(COMPUTERNAME)] } {
      set host $::env(COMPUTERNAME)
    }
  }
  set ch [open $beginFile w]
  puts $ch "<?xml version=\"1.0\"?>"
  puts $ch "<ProcessHandle Version=\"1\" Minor=\"0\">"
  puts $ch "    <Process Command=\".planAhead.\" Owner=\"$user\" Host=\"$host\" Pid=\"$pid\">"
  puts $ch "    </Process>"
  puts $ch "</ProcessHandle>"
  close $ch
}

proc end_step { step } {
  set endFile ".$step.end.rst"
  set ch [open $endFile w]
  close $ch
}

proc step_failed { step } {
  set endFile ".$step.error.rst"
  set ch [open $endFile w]
  close $ch
}


start_step init_design
set ACTIVE_STEP init_design
set rc [catch {
  create_msg_db init_design.pb
  set_param tcl.collectionResultDisplayLimit 0
  set_param xicom.use_bs_reader 1
  create_project -in_memory -part xc7k325tffg900-2
  set_property board_part xilinx.com:kc705:part0:1.0 [current_project]
  set_property design_mode GateLvl [current_fileset]
  set_param project.singleFileAddWarning.threshold 0
  set_property webtalk.parent_dir F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.cache/wt [current_project]
  set_property parent.project_path F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.xpr [current_project]
  set_property ip_cache_permissions disable [current_project]
  set_property XPM_LIBRARIES {XPM_CDC XPM_FIFO XPM_MEMORY} [current_project]
  add_files -quiet F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.runs/synth_1/mb_subsystem_wrapper.dcp
  set_msg_config -source 4 -id {BD 41-1661} -limit 0
  set_param project.isImplRun true
  add_files F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/sources_1/bd/mb_subsystem/mb_subsystem.bd
  set_param project.isImplRun false
  add_files F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.sdk/vfat3_sdk/Debug/vfat3_sdk.elf
  set_property SCOPED_TO_REF mb_subsystem [get_files -all F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.sdk/vfat3_sdk/Debug/vfat3_sdk.elf]
  set_property SCOPED_TO_CELLS microblaze_0 [get_files -all F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.sdk/vfat3_sdk/Debug/vfat3_sdk.elf]
  read_xdc F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/HW/project_2.srcs/constrs_1/new/system.xdc
  set_param project.isImplRun true
  link_design -top mb_subsystem_wrapper -part xc7k325tffg900-2
  set_param project.isImplRun false
  write_hwdef -force -file mb_subsystem_wrapper.hwdef
  close_msg_db -file init_design.pb
} RESULT]
if {$rc} {
  step_failed init_design
  return -code error $RESULT
} else {
  end_step init_design
  unset ACTIVE_STEP 
}

start_step opt_design
set ACTIVE_STEP opt_design
set rc [catch {
  create_msg_db opt_design.pb
  opt_design 
  write_checkpoint -force mb_subsystem_wrapper_opt.dcp
  create_report "impl_1_opt_report_drc_0" "report_drc -file mb_subsystem_wrapper_drc_opted.rpt -pb mb_subsystem_wrapper_drc_opted.pb -rpx mb_subsystem_wrapper_drc_opted.rpx"
  close_msg_db -file opt_design.pb
} RESULT]
if {$rc} {
  step_failed opt_design
  return -code error $RESULT
} else {
  end_step opt_design
  unset ACTIVE_STEP 
}

start_step place_design
set ACTIVE_STEP place_design
set rc [catch {
  create_msg_db place_design.pb
  implement_debug_core 
  place_design 
  write_checkpoint -force mb_subsystem_wrapper_placed.dcp
  create_report "impl_1_place_report_io_0" "report_io -file mb_subsystem_wrapper_io_placed.rpt"
  create_report "impl_1_place_report_utilization_0" "report_utilization -file mb_subsystem_wrapper_utilization_placed.rpt -pb mb_subsystem_wrapper_utilization_placed.pb"
  create_report "impl_1_place_report_control_sets_0" "report_control_sets -verbose -file mb_subsystem_wrapper_control_sets_placed.rpt"
  close_msg_db -file place_design.pb
} RESULT]
if {$rc} {
  step_failed place_design
  return -code error $RESULT
} else {
  end_step place_design
  unset ACTIVE_STEP 
}

start_step route_design
set ACTIVE_STEP route_design
set rc [catch {
  create_msg_db route_design.pb
  route_design 
  write_checkpoint -force mb_subsystem_wrapper_routed.dcp
  create_report "impl_1_route_report_drc_0" "report_drc -file mb_subsystem_wrapper_drc_routed.rpt -pb mb_subsystem_wrapper_drc_routed.pb -rpx mb_subsystem_wrapper_drc_routed.rpx"
  create_report "impl_1_route_report_methodology_0" "report_methodology -file mb_subsystem_wrapper_methodology_drc_routed.rpt -pb mb_subsystem_wrapper_methodology_drc_routed.pb -rpx mb_subsystem_wrapper_methodology_drc_routed.rpx"
  create_report "impl_1_route_report_power_0" "report_power -file mb_subsystem_wrapper_power_routed.rpt -pb mb_subsystem_wrapper_power_summary_routed.pb -rpx mb_subsystem_wrapper_power_routed.rpx"
  create_report "impl_1_route_report_route_status_0" "report_route_status -file mb_subsystem_wrapper_route_status.rpt -pb mb_subsystem_wrapper_route_status.pb"
  create_report "impl_1_route_report_timing_summary_0" "report_timing_summary -max_paths 10 -file mb_subsystem_wrapper_timing_summary_routed.rpt -rpx mb_subsystem_wrapper_timing_summary_routed.rpx -warn_on_violation "
  create_report "impl_1_route_report_incremental_reuse_0" "report_incremental_reuse -file mb_subsystem_wrapper_incremental_reuse_routed.rpt"
  create_report "impl_1_route_report_clock_utilization_0" "report_clock_utilization -file mb_subsystem_wrapper_clock_utilization_routed.rpt"
  close_msg_db -file route_design.pb
} RESULT]
if {$rc} {
  write_checkpoint -force mb_subsystem_wrapper_routed_error.dcp
  step_failed route_design
  return -code error $RESULT
} else {
  end_step route_design
  unset ACTIVE_STEP 
}

start_step write_bitstream
set ACTIVE_STEP write_bitstream
set rc [catch {
  create_msg_db write_bitstream.pb
  set_property XPM_LIBRARIES {XPM_CDC XPM_FIFO XPM_MEMORY} [current_project]
  catch { write_mem_info -force mb_subsystem_wrapper.mmi }
  catch { write_bmm -force mb_subsystem_wrapper_bd.bmm }
  write_bitstream -force mb_subsystem_wrapper.bit 
  catch { write_sysdef -hwdef mb_subsystem_wrapper.hwdef -bitfile mb_subsystem_wrapper.bit -meminfo mb_subsystem_wrapper.mmi -file mb_subsystem_wrapper.sysdef }
  catch {write_debug_probes -quiet -force mb_subsystem_wrapper}
  catch {file copy -force mb_subsystem_wrapper.ltx debug_nets.ltx}
  close_msg_db -file write_bitstream.pb
} RESULT]
if {$rc} {
  step_failed write_bitstream
  return -code error $RESULT
} else {
  end_step write_bitstream
  unset ACTIVE_STEP 
}

