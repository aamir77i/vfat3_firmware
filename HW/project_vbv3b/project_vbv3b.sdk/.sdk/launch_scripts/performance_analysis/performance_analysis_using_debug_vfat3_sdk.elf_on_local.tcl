connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent JTAG-SMT1 210203A0FB48A"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent JTAG-SMT1 210203A0FB48A"} -index 0
dow F:/XAPP1026/KC705_AxiEth_150MHZ_64KB/vfat3_firmware/HW/project_vbv3b/project_vbv3b.sdk/vfat3_sdk/Debug/vfat3_sdk.elf
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent JTAG-SMT1 210203A0FB48A"} -index 0
con
