open_project -project {P:\projects\OMNI\Libero\NeuroMini_golden\designer\NeuroMini\NeuroMini_fp\NeuroMini.pro}
set_programming_file -name {M2S025} -file {P:\projects\OMNI\Libero\NeuroMini_golden\designer\NeuroMini\NeuroMini.ipd}
enable_device -name {M2S025} -enable 1
set_programming_action -action {PROGRAM} -name {M2S025} 
run_selected_actions
set_programming_file -name {M2S025} -no_file
save_project
close_project
