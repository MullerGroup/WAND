set_device \
    -fam SmartFusion2 \
    -die PA4M6000 \
    -pkg fcs325
set_input_cfg \
	-path {C:/Users/subnets/Desktop/Mario_SF2/Mario_SF2/component/work/Mario_Libero_MSS/ENVM.cfg}
set_output_efc \
    -path {C:\Users\subnets\Desktop\Mario_SF2\Mario_SF2\designer\Mario_Libero\Mario_Libero.efc}
set_proj_dir \
    -path {C:\Users\subnets\Desktop\Mario_SF2\Mario_SF2}
gen_prg -use_init false
