open_project -project {P:\OMNI\Libero\Mario-CM_Flex-EMG_golden\Mario_SF2\designer\Mario_Libero\Mario_Libero_fp\Mario_Libero.pro}
set_programming_file -name {M2S060T} -file {P:\OMNI\Libero\Mario-CM_Flex-EMG_golden\Mario_SF2\designer\Mario_Libero\Mario_Libero.ipd}
enable_device -name {M2S060T} -enable 1
set_programming_action -action {PROGRAM} -name {M2S060T} 
run_selected_actions
set_programming_file -name {M2S060T} -no_file
save_project
close_project
