new_project \
         -name {Mario_Libero} \
         -location {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero_fp} \
         -mode {chain} \
         -connect_programmers {FALSE}
add_actel_device \
         -device {M2S025} \
         -name {M2S025}
enable_device \
         -name {M2S025} \
         -enable {TRUE}
save_project
close_project