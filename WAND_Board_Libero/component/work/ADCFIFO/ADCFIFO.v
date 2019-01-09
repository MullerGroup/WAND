//////////////////////////////////////////////////////////////////////
// Created by SmartDesign Wed Jan 20 11:39:30 2016
// Version: v11.6 11.6.0.34
//////////////////////////////////////////////////////////////////////

`timescale 1ns / 100ps

// ADCFIFO
module ADCFIFO(
    // Inputs
    CLK,
    DATA,
    RE,
    RESET,
    WE,
    // Outputs
    AEMPTY,
    AFULL,
    EMPTY,
    FULL,
    Q
);

//--------------------------------------------------------------------
// Input
//--------------------------------------------------------------------
input         CLK;
input  [15:0] DATA;
input         RE;
input         RESET;
input         WE;
//--------------------------------------------------------------------
// Output
//--------------------------------------------------------------------
output        AEMPTY;
output        AFULL;
output        EMPTY;
output        FULL;
output [15:0] Q;
//--------------------------------------------------------------------
// Nets
//--------------------------------------------------------------------
wire          AEMPTY_net_0;
wire          AFULL_net_0;
wire          CLK;
wire   [15:0] DATA;
wire          EMPTY_net_0;
wire          FULL_net_0;
wire   [15:0] Q_net_0;
wire          RE;
wire          RESET;
wire          WE;
wire          FULL_net_1;
wire          EMPTY_net_1;
wire          AFULL_net_1;
wire          AEMPTY_net_1;
wire   [15:0] Q_net_1;
//--------------------------------------------------------------------
// TiedOff Nets
//--------------------------------------------------------------------
wire          GND_net;
wire   [15:0] MEMRD_const_net_0;
//--------------------------------------------------------------------
// Constant assignments
//--------------------------------------------------------------------
assign GND_net           = 1'b0;
assign MEMRD_const_net_0 = 16'h0000;
//--------------------------------------------------------------------
// Top level output port assignments
//--------------------------------------------------------------------
assign FULL_net_1   = FULL_net_0;
assign FULL         = FULL_net_1;
assign EMPTY_net_1  = EMPTY_net_0;
assign EMPTY        = EMPTY_net_1;
assign AFULL_net_1  = AFULL_net_0;
assign AFULL        = AFULL_net_1;
assign AEMPTY_net_1 = AEMPTY_net_0;
assign AEMPTY       = AEMPTY_net_1;
assign Q_net_1      = Q_net_0;
assign Q[15:0]      = Q_net_1;
//--------------------------------------------------------------------
// Component instances
//--------------------------------------------------------------------
//--------ADCFIFO_ADCFIFO_0_COREFIFO   -   Actel:DirectCore:COREFIFO:2.4.100
ADCFIFO_ADCFIFO_0_COREFIFO #( 
        .AE_STATIC_EN   ( 1 ),
        .AEVAL          ( 65 ),
        .AF_STATIC_EN   ( 1 ),
        .AFVAL          ( 958 ),
        .CTRL_TYPE      ( 2 ),
        .ESTOP          ( 1 ),
        .FAMILY         ( 19 ),
        .FSTOP          ( 1 ),
        .FWFT           ( 0 ),
        .OVERFLOW_EN    ( 0 ),
        .PIPE           ( 1 ),
        .PREFETCH       ( 0 ),
        .RCLK_EDGE      ( 1 ),
        .RDCNT_EN       ( 0 ),
        .RDEPTH         ( 1024 ),
        .RE_POLARITY    ( 0 ),
        .READ_DVALID    ( 0 ),
        .RESET_POLARITY ( 0 ),
        .RWIDTH         ( 16 ),
        .SYNC           ( 1 ),
        .UNDERFLOW_EN   ( 0 ),
        .WCLK_EDGE      ( 1 ),
        .WDEPTH         ( 1024 ),
        .WE_POLARITY    ( 0 ),
        .WRCNT_EN       ( 0 ),
        .WRITE_ACK      ( 0 ),
        .WWIDTH         ( 16 ) )
ADCFIFO_0(
        // Inputs
        .CLK       ( CLK ),
        .WCLOCK    ( GND_net ), // tied to 1'b0 from definition
        .RCLOCK    ( GND_net ), // tied to 1'b0 from definition
        .RESET     ( RESET ),
        .WE        ( WE ),
        .RE        ( RE ),
        .DATA      ( DATA ),
        .MEMRD     ( MEMRD_const_net_0 ), // tied to 16'h0000 from definition
        // Outputs
        .FULL      ( FULL_net_0 ),
        .EMPTY     ( EMPTY_net_0 ),
        .AFULL     ( AFULL_net_0 ),
        .AEMPTY    ( AEMPTY_net_0 ),
        .OVERFLOW  (  ),
        .UNDERFLOW (  ),
        .WACK      (  ),
        .DVLD      (  ),
        .MEMWE     (  ),
        .MEMRE     (  ),
        .Q         ( Q_net_0 ),
        .WRCNT     (  ),
        .RDCNT     (  ),
        .MEMWADDR  (  ),
        .MEMRADDR  (  ),
        .MEMWD     (  ) 
        );


endmodule
