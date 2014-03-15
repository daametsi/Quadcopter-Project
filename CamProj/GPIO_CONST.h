//GPIO base addresses
#define GPIO0_BASE 0x44E07000
#define GPIO1_BASE 0x4804C000
#define GPIO2_BASE 0x481AC000
#define GPIO3_BASE 0x481AE000

//GPIO0
#define BIT_OE 10
#define BIT_WE 11
#define BIT_RCK 9
#define BIT_PWDN 8

//GPIO1
//NONE

//GPIO2
#define BIT_RRST 23
#define BIT_WRST 25
#define BIT_STR 17
#define BIT_RST 16
#define BIT_D00 14
#define BIT_D01 15
#define BIT_D02 12
#define BIT_D03 13
#define BIT_D04 10
#define BIT_D05 11
#define BIT_D06 8
#define BIT_D07 9
#define BIT_HREF 6
#define BIT_VSYNC 7

//GPIO size thing
#define GPIO_SIZE  0x00000FFF

// OE: 0 is output, 1 is input
#define GPIO_OE 0x14d
#define GPIO_IN 0x14e
#define GPIO_OUT 0x14f

// Direction constants
#define GPIO_DIR_IN 0
#define GPIO_DIR_OUT 1
