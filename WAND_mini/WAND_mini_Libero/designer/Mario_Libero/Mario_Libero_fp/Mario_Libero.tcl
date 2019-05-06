open_project -project {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero_fp\Mario_Libero.pro}\
         -connect_programmers {FALSE}
if { [catch {load_programming_data \
    -name {M2S025} \
    -fpga {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero.map} \
    -header {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero.hdr} \
    -envm {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero.efc}  \
    -spm {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero.spm} \
    -dca {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero.dca} } return_val] } {
save_project
close_project
exit }
set_programming_file -name {M2S025} -no_file
save_project
close_project
