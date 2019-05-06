//////////////////////////////////////////////////////////////////////
// Created by SmartDesign Wed Jun 20 15:35:16 2018
// Version: v11.8 SP3 11.8.3.6
//////////////////////////////////////////////////////////////////////

`timescale 1ns / 100ps

// NeuroMini
module NeuroMini(
    // Inputs
    MMUART_RXD,
    N2C_DATA,
    SPI_0_DI,
    XTL,
    // Outputs
    C2N_DATA,
    C2N_VALID,
    GPIO0,
    GPIO0_R,
    GPIO1,
    GPIO1_R,
    GPIO2,
    GPIO2_R,
    GPIO3,
    GPIO3_R,
    GPIO4,
    MMUART_TXD,
    NM_CLK,
    SPI_0_DO,
    // Inouts
    SPI_0_CLK,
    SPI_0_SS0
);

//--------------------------------------------------------------------
// Input
//--------------------------------------------------------------------
input  MMUART_RXD;
input  N2C_DATA;
input  SPI_0_DI;
input  XTL;
//--------------------------------------------------------------------
// Output
//--------------------------------------------------------------------
output C2N_DATA;
output C2N_VALID;
output GPIO0;
output GPIO0_R;
output GPIO1;
output GPIO1_R;
output GPIO2;
output GPIO2_R;
output GPIO3;
output GPIO3_R;
output GPIO4;
output MMUART_TXD;
output NM_CLK;
output SPI_0_DO;
//--------------------------------------------------------------------
// Inout
//--------------------------------------------------------------------
inout  SPI_0_CLK;
inout  SPI_0_SS0;
//--------------------------------------------------------------------
// Nets
//--------------------------------------------------------------------
wire   [0:0]  C2N_DATA_1;
wire   [0:0]  C2N_VALID_1;
wire          CoreAPB3_0_APBmslave0_PENABLE;
wire   [31:0] CoreAPB3_0_APBmslave0_PRDATA;
wire          CoreAPB3_0_APBmslave0_PREADY;
wire          CoreAPB3_0_APBmslave0_PSELx;
wire          CoreAPB3_0_APBmslave0_PSLVERR;
wire   [31:0] CoreAPB3_0_APBmslave0_PWDATA;
wire          CoreAPB3_0_APBmslave0_PWRITE;
wire          FCCC_0_GL0;
wire          FCCC_0_LOCK;
wire          GPIO0_net_0;
wire          GPIO0_R_net_0;
wire          GPIO1_net_0;
wire          GPIO1_R_net_0;
wire          GPIO2_net_0;
wire          GPIO2_R_net_0;
wire          GPIO3_net_0;
wire          GPIO3_R_net_0;
wire          GPIO4_net_0;
wire   [31:0] Mario_Libero_MSS_0_FIC_0_APB_MASTER_PADDR;
wire          Mario_Libero_MSS_0_FIC_0_APB_MASTER_PENABLE;
wire   [31:0] Mario_Libero_MSS_0_FIC_0_APB_MASTER_PRDATA;
wire          Mario_Libero_MSS_0_FIC_0_APB_MASTER_PREADY;
wire          Mario_Libero_MSS_0_FIC_0_APB_MASTER_PSELx;
wire          Mario_Libero_MSS_0_FIC_0_APB_MASTER_PSLVERR;
wire   [31:0] Mario_Libero_MSS_0_FIC_0_APB_MASTER_PWDATA;
wire          Mario_Libero_MSS_0_FIC_0_APB_MASTER_PWRITE;
wire          Mario_Libero_MSS_0_MSS_RESET_N_M2F;
wire          MMUART_RXD;
wire          MMUART_TXD_net_0;
wire          N2C_DATA;
wire          NM_CLK_net_0;
wire          OSC_0_XTLOSC_CCC_OUT_XTLOSC_CCC;
wire          SPI_0_CLK;
wire          SPI_0_DI;
wire          SPI_0_DO_1;
wire          SPI_0_SS0;
wire          XTL;
wire          NM_CLK_net_1;
wire          SPI_0_DO_1_net_0;
wire          GPIO0_net_1;
wire          GPIO1_net_1;
wire          GPIO2_net_1;
wire          GPIO3_net_1;
wire          GPIO4_net_1;
wire          MMUART_TXD_net_1;
wire          C2N_DATA_1_net_0;
wire          C2N_VALID_1_net_0;
wire          GPIO0_R_net_1;
wire          GPIO1_R_net_1;
wire          GPIO2_R_net_1;
wire          GPIO3_R_net_1;
wire   [1:1]  C2N_DATA_slice_0;
wire   [1:1]  C2N_VALID_slice_0;
wire   [1:0]  N2C_DATA_net_0;
wire   [1:0]  C2N_DATA_net_0;
wire   [1:0]  C2N_VALID_net_0;
//--------------------------------------------------------------------
// TiedOff Nets
//--------------------------------------------------------------------
wire          GND_net;
wire          VCC_net;
wire   [31:0] IADDR_const_net_0;
wire   [7:2]  PADDR_const_net_0;
wire   [7:0]  PWDATA_const_net_0;
wire   [31:0] PRDATAS1_const_net_0;
wire   [31:0] PRDATAS2_const_net_0;
wire   [31:0] PRDATAS3_const_net_0;
wire   [31:0] PRDATAS4_const_net_0;
wire   [31:0] PRDATAS5_const_net_0;
wire   [31:0] PRDATAS6_const_net_0;
wire   [31:0] PRDATAS7_const_net_0;
wire   [31:0] PRDATAS8_const_net_0;
wire   [31:0] PRDATAS9_const_net_0;
wire   [31:0] PRDATAS10_const_net_0;
wire   [31:0] PRDATAS11_const_net_0;
wire   [31:0] PRDATAS12_const_net_0;
wire   [31:0] PRDATAS13_const_net_0;
wire   [31:0] PRDATAS14_const_net_0;
wire   [31:0] PRDATAS15_const_net_0;
wire   [31:0] PRDATAS16_const_net_0;
//--------------------------------------------------------------------
// Bus Interface Nets Declarations - Unequal Pin Widths
//--------------------------------------------------------------------
wire   [31:0] CoreAPB3_0_APBmslave0_PADDR;
wire   [6:0]  CoreAPB3_0_APBmslave0_PADDR_0_6to0;
wire   [6:0]  CoreAPB3_0_APBmslave0_PADDR_0;
//--------------------------------------------------------------------
// Constant assignments
//--------------------------------------------------------------------
assign GND_net               = 1'b0;
assign VCC_net               = 1'b1;
assign IADDR_const_net_0     = 32'h00000000;
assign PADDR_const_net_0     = 6'h00;
assign PWDATA_const_net_0    = 8'h00;
assign PRDATAS1_const_net_0  = 32'h00000000;
assign PRDATAS2_const_net_0  = 32'h00000000;
assign PRDATAS3_const_net_0  = 32'h00000000;
assign PRDATAS4_const_net_0  = 32'h00000000;
assign PRDATAS5_const_net_0  = 32'h00000000;
assign PRDATAS6_const_net_0  = 32'h00000000;
assign PRDATAS7_const_net_0  = 32'h00000000;
assign PRDATAS8_const_net_0  = 32'h00000000;
assign PRDATAS9_const_net_0  = 32'h00000000;
assign PRDATAS10_const_net_0 = 32'h00000000;
assign PRDATAS11_const_net_0 = 32'h00000000;
assign PRDATAS12_const_net_0 = 32'h00000000;
assign PRDATAS13_const_net_0 = 32'h00000000;
assign PRDATAS14_const_net_0 = 32'h00000000;
assign PRDATAS15_const_net_0 = 32'h00000000;
assign PRDATAS16_const_net_0 = 32'h00000000;
//--------------------------------------------------------------------
// Top level output port assignments
//--------------------------------------------------------------------
assign NM_CLK_net_1      = NM_CLK_net_0;
assign NM_CLK            = NM_CLK_net_1;
assign SPI_0_DO_1_net_0  = SPI_0_DO_1;
assign SPI_0_DO          = SPI_0_DO_1_net_0;
assign GPIO0_net_1       = GPIO0_net_0;
assign GPIO0             = GPIO0_net_1;
assign GPIO1_net_1       = GPIO1_net_0;
assign GPIO1             = GPIO1_net_1;
assign GPIO2_net_1       = GPIO2_net_0;
assign GPIO2             = GPIO2_net_1;
assign GPIO3_net_1       = GPIO3_net_0;
assign GPIO3             = GPIO3_net_1;
assign GPIO4_net_1       = GPIO4_net_0;
assign GPIO4             = GPIO4_net_1;
assign MMUART_TXD_net_1  = MMUART_TXD_net_0;
assign MMUART_TXD        = MMUART_TXD_net_1;
assign C2N_DATA_1_net_0  = C2N_DATA_1[0];
assign C2N_DATA          = C2N_DATA_1_net_0;
assign C2N_VALID_1_net_0 = C2N_VALID_1[0];
assign C2N_VALID         = C2N_VALID_1_net_0;
assign GPIO0_R_net_1     = GPIO0_R_net_0;
assign GPIO0_R           = GPIO0_R_net_1;
assign GPIO1_R_net_1     = GPIO1_R_net_0;
assign GPIO1_R           = GPIO1_R_net_1;
assign GPIO2_R_net_1     = GPIO2_R_net_0;
assign GPIO2_R           = GPIO2_R_net_1;
assign GPIO3_R_net_1     = GPIO3_R_net_0;
assign GPIO3_R           = GPIO3_R_net_1;
//--------------------------------------------------------------------
// Slices assignments
//--------------------------------------------------------------------
assign C2N_DATA_1[0]        = C2N_DATA_net_0[0:0];
assign C2N_VALID_1[0]       = C2N_VALID_net_0[0:0];
assign C2N_DATA_slice_0[1]  = C2N_DATA_net_0[1:1];
assign C2N_VALID_slice_0[1] = C2N_VALID_net_0[1:1];
//--------------------------------------------------------------------
// Concatenation assignments
//--------------------------------------------------------------------
assign N2C_DATA_net_0 = { 1'b0 , N2C_DATA };
//--------------------------------------------------------------------
// Bus Interface Nets Assignments - Unequal Pin Widths
//--------------------------------------------------------------------
assign CoreAPB3_0_APBmslave0_PADDR_0_6to0 = CoreAPB3_0_APBmslave0_PADDR[6:0];
assign CoreAPB3_0_APBmslave0_PADDR_0 = { CoreAPB3_0_APBmslave0_PADDR_0_6to0 };

//--------------------------------------------------------------------
// Component instances
//--------------------------------------------------------------------
//--------cmam_if_wrap
cmam_if_wrap cmam_if_wrap_0(
        // Inputs
        .PCLK      ( FCCC_0_GL0 ),
        .PENABLE   ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSEL      ( CoreAPB3_0_APBmslave0_PSELx ),
        .PRESETN   ( Mario_Libero_MSS_0_MSS_RESET_N_M2F ),
        .PWRITE    ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PADDR     ( CoreAPB3_0_APBmslave0_PADDR_0 ),
        .PWDATA    ( CoreAPB3_0_APBmslave0_PWDATA ),
        .N2C_DATA  ( N2C_DATA_net_0 ),
        // Outputs
        .PREADY    ( CoreAPB3_0_APBmslave0_PREADY ),
        .PSLVERR   ( CoreAPB3_0_APBmslave0_PSLVERR ),
        .DEBUG     (  ),
        .C2A_VALID (  ),
        .C2A_DATA  (  ),
        .A2C_DATA  (  ),
        .PRDATA    ( CoreAPB3_0_APBmslave0_PRDATA ),
        .C2N_DATA  ( C2N_DATA_net_0 ),
        .C2N_VALID ( C2N_VALID_net_0 ) 
        );

//--------CoreAPB3   -   Actel:DirectCore:CoreAPB3:4.1.100
CoreAPB3 #( 
        .APB_DWIDTH      ( 32 ),
        .APBSLOT0ENABLE  ( 1 ),
        .APBSLOT1ENABLE  ( 0 ),
        .APBSLOT2ENABLE  ( 0 ),
        .APBSLOT3ENABLE  ( 0 ),
        .APBSLOT4ENABLE  ( 0 ),
        .APBSLOT5ENABLE  ( 0 ),
        .APBSLOT6ENABLE  ( 0 ),
        .APBSLOT7ENABLE  ( 0 ),
        .APBSLOT8ENABLE  ( 0 ),
        .APBSLOT9ENABLE  ( 0 ),
        .APBSLOT10ENABLE ( 0 ),
        .APBSLOT11ENABLE ( 0 ),
        .APBSLOT12ENABLE ( 0 ),
        .APBSLOT13ENABLE ( 0 ),
        .APBSLOT14ENABLE ( 0 ),
        .APBSLOT15ENABLE ( 0 ),
        .FAMILY          ( 19 ),
        .IADDR_OPTION    ( 0 ),
        .MADDR_BITS      ( 20 ),
        .SC_0            ( 0 ),
        .SC_1            ( 0 ),
        .SC_2            ( 0 ),
        .SC_3            ( 0 ),
        .SC_4            ( 0 ),
        .SC_5            ( 0 ),
        .SC_6            ( 0 ),
        .SC_7            ( 0 ),
        .SC_8            ( 0 ),
        .SC_9            ( 0 ),
        .SC_10           ( 0 ),
        .SC_11           ( 0 ),
        .SC_12           ( 0 ),
        .SC_13           ( 0 ),
        .SC_14           ( 0 ),
        .SC_15           ( 0 ),
        .UPR_NIBBLE_POSN ( 6 ) )
CoreAPB3_0(
        // Inputs
        .PRESETN    ( GND_net ), // tied to 1'b0 from definition
        .PCLK       ( GND_net ), // tied to 1'b0 from definition
        .PWRITE     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PWRITE ),
        .PENABLE    ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PENABLE ),
        .PSEL       ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PSELx ),
        .PREADYS0   ( CoreAPB3_0_APBmslave0_PREADY ),
        .PSLVERRS0  ( CoreAPB3_0_APBmslave0_PSLVERR ),
        .PREADYS1   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS1  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS2   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS2  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS3   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS3  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS4   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS4  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS5   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS5  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS6   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS6  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS7   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS7  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS8   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS8  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS9   ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS9  ( GND_net ), // tied to 1'b0 from definition
        .PREADYS10  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS10 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS11  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS11 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS12  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS12 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS13  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS13 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS14  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS14 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS15  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS15 ( GND_net ), // tied to 1'b0 from definition
        .PREADYS16  ( VCC_net ), // tied to 1'b1 from definition
        .PSLVERRS16 ( GND_net ), // tied to 1'b0 from definition
        .PADDR      ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PADDR ),
        .PWDATA     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PWDATA ),
        .PRDATAS0   ( CoreAPB3_0_APBmslave0_PRDATA ),
        .PRDATAS1   ( PRDATAS1_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS2   ( PRDATAS2_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS3   ( PRDATAS3_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS4   ( PRDATAS4_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS5   ( PRDATAS5_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS6   ( PRDATAS6_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS7   ( PRDATAS7_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS8   ( PRDATAS8_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS9   ( PRDATAS9_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS10  ( PRDATAS10_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS11  ( PRDATAS11_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS12  ( PRDATAS12_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS13  ( PRDATAS13_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS14  ( PRDATAS14_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS15  ( PRDATAS15_const_net_0 ), // tied to 32'h00000000 from definition
        .PRDATAS16  ( PRDATAS16_const_net_0 ), // tied to 32'h00000000 from definition
        .IADDR      ( IADDR_const_net_0 ), // tied to 32'h00000000 from definition
        // Outputs
        .PREADY     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PREADY ),
        .PSLVERR    ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PSLVERR ),
        .PWRITES    ( CoreAPB3_0_APBmslave0_PWRITE ),
        .PENABLES   ( CoreAPB3_0_APBmslave0_PENABLE ),
        .PSELS0     ( CoreAPB3_0_APBmslave0_PSELx ),
        .PSELS1     (  ),
        .PSELS2     (  ),
        .PSELS3     (  ),
        .PSELS4     (  ),
        .PSELS5     (  ),
        .PSELS6     (  ),
        .PSELS7     (  ),
        .PSELS8     (  ),
        .PSELS9     (  ),
        .PSELS10    (  ),
        .PSELS11    (  ),
        .PSELS12    (  ),
        .PSELS13    (  ),
        .PSELS14    (  ),
        .PSELS15    (  ),
        .PSELS16    (  ),
        .PRDATA     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PRDATA ),
        .PADDRS     ( CoreAPB3_0_APBmslave0_PADDR ),
        .PWDATAS    ( CoreAPB3_0_APBmslave0_PWDATA ) 
        );

//--------NeuroMini_FCCC_0_FCCC   -   Actel:SgCore:FCCC:2.0.200
NeuroMini_FCCC_0_FCCC FCCC_0(
        // Inputs
        .XTLOSC ( OSC_0_XTLOSC_CCC_OUT_XTLOSC_CCC ),
        // Outputs
        .GL0    ( FCCC_0_GL0 ),
        .LOCK   ( FCCC_0_LOCK ) 
        );

//--------INV
INV INV_0(
        // Inputs
        .A ( FCCC_0_GL0 ),
        // Outputs
        .Y ( NM_CLK_net_0 ) 
        );

//--------NeuroMini_MSS
NeuroMini_MSS NeuroMini_MSS_0(
        // Inputs
        .MCCC_CLK_BASE          ( FCCC_0_GL0 ),
        .MCCC_CLK_BASE_PLL_LOCK ( FCCC_0_LOCK ),
        .FIC_0_APB_M_PREADY     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PREADY ),
        .FIC_0_APB_M_PSLVERR    ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PSLVERR ),
        .SPI_0_DI               ( SPI_0_DI ),
        .MMUART_0_RXD_F2M       ( MMUART_RXD ),
        .FIC_0_APB_M_PRDATA     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PRDATA ),
        // Outputs
        .FIC_0_APB_M_PSEL       ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PSELx ),
        .FIC_0_APB_M_PWRITE     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PWRITE ),
        .FIC_0_APB_M_PENABLE    ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PENABLE ),
        .MSS_RESET_N_M2F        ( Mario_Libero_MSS_0_MSS_RESET_N_M2F ),
        .SPI_0_DO               ( SPI_0_DO_1 ),
        .GPIO_0_M2F             ( GPIO0_net_0 ),
        .GPIO_1_M2F             ( GPIO1_net_0 ),
        .GPIO_2_M2F             ( GPIO2_net_0 ),
        .GPIO_3_M2F             ( GPIO3_net_0 ),
        .GPIO_4_M2F             ( GPIO4_net_0 ),
        .MMUART_0_TXD_M2F       ( MMUART_TXD_net_0 ),
        .GPIO_5_M2F             ( GPIO0_R_net_0 ),
        .GPIO_6_M2F             ( GPIO1_R_net_0 ),
        .GPIO_7_M2F             ( GPIO2_R_net_0 ),
        .GPIO_8_M2F             ( GPIO3_R_net_0 ),
        .FIC_0_APB_M_PADDR      ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PADDR ),
        .FIC_0_APB_M_PWDATA     ( Mario_Libero_MSS_0_FIC_0_APB_MASTER_PWDATA ),
        // Inouts
        .SPI_0_CLK              ( SPI_0_CLK ),
        .SPI_0_SS0              ( SPI_0_SS0 ) 
        );

//--------NeuroMini_OSC_0_OSC   -   Actel:SgCore:OSC:2.0.101
NeuroMini_OSC_0_OSC OSC_0(
        // Inputs
        .XTL                ( XTL ),
        // Outputs
        .RCOSC_25_50MHZ_CCC (  ),
        .RCOSC_25_50MHZ_O2F (  ),
        .RCOSC_1MHZ_CCC     (  ),
        .RCOSC_1MHZ_O2F     (  ),
        .XTLOSC_CCC         ( OSC_0_XTLOSC_CCC_OUT_XTLOSC_CCC ),
        .XTLOSC_O2F         (  ) 
        );


endmodule
