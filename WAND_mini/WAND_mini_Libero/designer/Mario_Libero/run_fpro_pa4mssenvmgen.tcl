set_device \
    -fam SmartFusion2 \
    -die PA4M2500_N \
    -pkg fcs325
set_input_cfg \
	-path {C:/Users/flb/Desktop/NeuroMini_golden/component/work/Mario_Libero_MSS/ENVM.cfg}
set_output_efc \
    -path {C:\Users\flb\Desktop\NeuroMini_golden\designer\Mario_Libero\Mario_Libero.efc}
set_proj_dir \
    -path {C:\Users\flb\Desktop\NeuroMini_golden}
gen_prg -use_init false
