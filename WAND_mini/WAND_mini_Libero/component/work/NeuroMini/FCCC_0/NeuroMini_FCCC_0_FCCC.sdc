set_component NeuroMini_FCCC_0_FCCC
# Microsemi Corp.
# Date: 2018-Jun-20 15:35:13
#

create_clock -period 50 [ get_pins { CCC_INST/XTLOSC } ]
create_generated_clock -multiply_by 128 -divide_by 125 -source [ get_pins { CCC_INST/XTLOSC } ] -phase 0 [ get_pins { CCC_INST/GL0 } ]
