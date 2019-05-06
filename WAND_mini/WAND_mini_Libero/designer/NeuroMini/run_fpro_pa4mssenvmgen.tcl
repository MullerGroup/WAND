set_device \
    -fam SmartFusion2 \
    -die PA4M2500_N \
    -pkg fcs325
set_input_cfg \
	-path {P:/Libero/NeuroMini_golden/component/work/NeuroMini_MSS/ENVM.cfg}
set_output_efc \
    -path {P:\Libero\NeuroMini_golden\designer\NeuroMini\NeuroMini.efc}
set_proj_dir \
    -path {P:\Libero\NeuroMini_golden}
gen_prg -use_init false
