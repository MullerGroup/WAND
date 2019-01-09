new_project \
         -name {Mario_Libero} \
         -location {C:\Users\subnets\Documents\NHP\Mario_Libero_v2\designer\Mario_Libero\Mario_Libero_fp} \
         -mode {chain} \
         -connect_programmers {FALSE}
add_actel_device \
         -device {M2S060T} \
         -name {M2S060T}
enable_device \
         -name {M2S060T} \
         -enable {TRUE}
save_project
close_project
