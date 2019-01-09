set_component Mario_Libero_FCCC_0_FCCC
# Microsemi Corp.
# Date: 2017-Mar-16 15:33:40
#

create_clock -period 50 [ get_pins { CCC_INST/XTLOSC } ]
create_generated_clock -multiply_by 128 -divide_by 125 -source [ get_pins { CCC_INST/XTLOSC } ] -phase 0 [ get_pins { CCC_INST/GL0 } ]
