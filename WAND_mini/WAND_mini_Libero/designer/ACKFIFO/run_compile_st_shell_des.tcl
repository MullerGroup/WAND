set_device \
    -family  SmartFusion2 \
    -die     PA4M2500_N \
    -package fcs325 \
    -speed   STD \
    -tempr   {COM} \
    -voltr   {COM}
set_def {VOLTAGE} {1.2}
set_def {VCCI_1.2_VOLTR} {COM}
set_def {VCCI_1.5_VOLTR} {COM}
set_def {VCCI_1.8_VOLTR} {COM}
set_def {VCCI_2.5_VOLTR} {COM}
set_def {VCCI_3.3_VOLTR} {COM}
set_def USE_TCGEN 1
set_def NETLIST_TYPE EDIF
set_name ACKFIFO
set_workdir {C:\Users\flb\Desktop\NeuroMini_golden\designer\ACKFIFO}
set_log     {C:\Users\flb\Desktop\NeuroMini_golden\designer\ACKFIFO\ACKFIFO_sdc.log}
set_design_state pre_layout
