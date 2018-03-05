#!/bin/bash -f
# Vivado (TM) v2017.2 (64-bit)
#
# Filename    : axi_iic_0.sh
# Simulator   : Xilinx Vivado Simulator
# Description : Simulation script for compiling, elaborating and verifying the project source files.
#               The script will automatically create the design libraries sub-directories in the run
#               directory, add the library logical mappings in the simulator setup file, create default
#               'do/prj' file, execute compilation, elaboration and simulation steps.
#
# Generated by Vivado on Wed Nov 08 11:58:46 +0100 2017
# IP Build 1909766 on Thu Jun 15 19:58:00 MDT 2017 
#
# usage: axi_iic_0.sh [-help]
# usage: axi_iic_0.sh [-lib_map_path]
# usage: axi_iic_0.sh [-noclean_files]
# usage: axi_iic_0.sh [-reset_run]
#
# ********************************************************************************************************

xv_path="C:/Xilinx/Vivado/2017.2"

# Command line options
xvlog_opts="-m64 --relax"
xvhdl_opts="-m64 --relax"


# Script info
echo -e "axi_iic_0.sh - Script generated by export_simulation (Vivado v2017.2 (64-bit)-id)\n"

# Main steps
run()
{
  check_args $# $1
  setup $1 $2
  compile
  elaborate
  simulate
}

# RUN_STEP: <compile>
compile()
{
  # Compile design files
  $xv_path/bin/xvlog $xvlog_opts -prj vlog.prj 2>&1 | tee compile.log
  $xv_path/bin/xvhdl $xvhdl_opts -prj vhdl.prj 2>&1 | tee compile.log

}

# RUN_STEP: <elaborate>
elaborate()
{
  $xv_path/bin/xelab --relax --debug typical --mt auto -L xil_defaultlib -L xpm -L lib_pkg_v1_0_2 -L lib_cdc_v1_0_2 -L axi_lite_ipif_v3_0_4 -L interrupt_control_v3_1_4 -L axi_iic_v2_0_16 -L unisims_ver -L unimacro_ver -L secureip --snapshot axi_iic_0 xil_defaultlib.axi_iic_0 xil_defaultlib.glbl -log elaborate.log
}

# RUN_STEP: <simulate>
simulate()
{
  $xv_path/bin/xsim axi_iic_0 -key {Behavioral:sim_1:Functional:axi_iic_0} -tclbatch cmd.tcl -log simulate.log
}

# STEP: setup
setup()
{
  case $1 in
    "-lib_map_path" )
      if [[ ($2 == "") ]]; then
        echo -e "ERROR: Simulation library directory path not specified (type \"./axi_iic_0.sh -help\" for more information)\n"
        exit 1
      fi
    ;;
    "-reset_run" )
      reset_run
      echo -e "INFO: Simulation run files deleted.\n"
      exit 0
    ;;
    "-noclean_files" )
      # do not remove previous data
    ;;
    * )
  esac

  # Add any setup/initialization commands here:-

  # <user specific commands>

}

# Delete generated data from the previous run
reset_run()
{
  files_to_remove=(xelab.pb xsim.jou xvhdl.log xvlog.log compile.log elaborate.log simulate.log xelab.log xsim.log run.log xvhdl.pb xvlog.pb axi_iic_0.wdb xsim.dir)
  for (( i=0; i<${#files_to_remove[*]}; i++ )); do
    file="${files_to_remove[i]}"
    if [[ -e $file ]]; then
      rm -rf $file
    fi
  done
}

# Check command line arguments
check_args()
{
  if [[ ($1 == 1 ) && ($2 != "-lib_map_path" && $2 != "-noclean_files" && $2 != "-reset_run" && $2 != "-help" && $2 != "-h") ]]; then
    echo -e "ERROR: Unknown option specified '$2' (type \"./axi_iic_0.sh -help\" for more information)\n"
    exit 1
  fi

  if [[ ($2 == "-help" || $2 == "-h") ]]; then
    usage
  fi
}

# Script usage
usage()
{
  msg="Usage: axi_iic_0.sh [-help]\n\
Usage: axi_iic_0.sh [-lib_map_path]\n\
Usage: axi_iic_0.sh [-reset_run]\n\
Usage: axi_iic_0.sh [-noclean_files]\n\n\
[-help] -- Print help information for this script\n\n\
[-lib_map_path <path>] -- Compiled simulation library directory path. The simulation library is compiled\n\
using the compile_simlib tcl command. Please see 'compile_simlib -help' for more information.\n\n\
[-reset_run] -- Recreate simulator setup files and library mappings for a clean run. The generated files\n\
from the previous run will be removed. If you don't want to remove the simulator generated files, use the\n\
-noclean_files switch.\n\n\
[-noclean_files] -- Reset previous run, but do not remove simulator generated files from the previous run.\n\n"
  echo -e $msg
  exit 1
}

# Launch script
run $1 $2
