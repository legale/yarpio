/* Yet Another Raspberry Pi GPIO Library YARPIO 
 * <yarpio.h>
 *
 * Copyright (c) 2021, legale <legale.legale@gmail.com>
 */

#include <stdint.h> /* integer types  */

/* 0x20000000 peripherals base address for Raspberry 1
 * 0x3F000000 for Raspberry 2, 3
 * 0xFE000000 for Raspberry 4
 * `cat /proc/iomem | grep gpio` to get proper gpio address
 *     
 */

#define GPIO_OFFSET 0x200000 /* is GPIO registers offset */
#define GPIO_SIZE 0xB4 /*from 0x200000 to 0x2000B4 */

#ifndef RPI_MODEL
    #define GPIO_BASE 0x3F000000 + GPIO_OFFSET 
#elif RPI_MODEL = 1
    #define GPIO_BASE 0x20000000 + GPIO_OFFSET    
#elif RPI_MODEL == 2 || RPI_MODEL == 3
    #define GPIO_BASE 0x3F000000 + GPIO_OFFSET 
#elif RPI_MODEL == 4 
    #define GPIO_BASE 0xFE000000 + GPIO_OFFSET 
#else
    #error "Unknown Raspberry model"
#endif

/*
 * ****************************************************************************************************
 * Raspberry BCM2835 Datasheet Peripherals offsets
 * "https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf"
 *    ["name"]        ["offset"]              ["description"]                 ["size"]   ["read/write"]
 * ****************************************************************************************************
 */
#define GPFSEL0         0x00        /*      GPIO Function Select 0                  32      R/W     */
#define GPFSEL1         0x04        /*      GPIO Function Select 1                  32      R/W     */
#define GPFSEL2         0x08        /*      GPIO Function Select 2                  32      R/W     */
#define GPFSEL3         0x0C        /*      GPIO Function Select 3                  32      R/W     */
#define GPFSEL4         0x10        /*      GPIO Function Select 4                  32      R/W     */
#define GPFSEL5         0x14        /*      GPIO Function Select 5                  32      R/W     */
/* 0x18  Reserved  */
#define GPSET0          0x1C        /*      GPIO Pin Output Set 0                   32      W       */
#define GPSET1          0x20        /*      GPIO Pin Output Set 1                   32      W       */
/* 0x24     -       Reserved */
#define GPCLR0          0x28        /*      GPIO Pin Output Clear 0                 32      W       */
#define GPCLR1          0x2C        /*      GPIO Pin Output Clear 1                 32      W       */
/* 0x30     -       Reserved */
#define GPLEV0          0x34        /*      GPIO Pin Level 0                        32      R       */
#define GPLEV1          0x38        /*      GPIO Pin Level 1                        32      R       */
/* 0x3C     -       Reserved */
#define GPEDS0          0x40        /*      GPIO Pin Event Detect Status 0          32      R/W     */
#define GPEDS1          0x44        /*      GPIO Pin Event Detect Status 1          32      R/W     */
/* 0x48     -       Reserved */
#define GPREN0          0x4C        /*      GPIO Pin Rising Edge Detect Enable 0    32      R/W     */
#define GPREN1          0x50        /*      GPIO Pin Rising Edge Detect Enable 1    32      R/W     */
/* 0x54     -       Reserved */
#define GPFEN0          0x58        /*      GPIO Pin Falling Edge Detect Enable 0   32      R/W     */
#define GPFEN1          0x5C        /*      GPIO Pin Falling Edge Detect Enable 1   32      R/W     */
/* 0x60     -       Reserved */
#define GPHEN0          0x64        /*      GPIO Pin High Detect Enable 0           32      R/W     */
#define GPHEN1          0x68        /*      GPIO Pin High Detect Enable 1           32      R/W     */
/* 0x6C     -       Reserved */
#define GPLEN0          0x70        /*      GPIO Pin Low Detect Enable 0            32      R/W     */
#define GPLEN1          0x74        /*      GPIO Pin Low Detect Enable 1            32      R/W     */
/* 0x78     -       Reserved */
#define GPAREN0         0x7C        /*      GPIO Pin Async. Rising Edge Detect 0    32      R/W     */
#define GPAREN1         0x80        /*      GPIO Pin Async. Rising Edge Detect 1    32      R/W     */
/* 0x84     -       Reserved */
#define GPAFEN0         0x88        /*      GPIO Pin Async. Falling Edge Detect 0   32      R/W     */
#define GPAFEN1         0x8C        /*      GPIO Pin Async. Falling Edge Detect 1   32      R/W     */
/* 0x90     -       Reserved */
#define GPPUD           0x94        /*      GPIO Pin Pull-up/down Enable            32      R/W     */
#define GPPUDCLK0       0x98        /*      GPIO Pin Pull-up/down Enable Clock 0    32      R/W     */
#define GPPUDCLK1       0x9C        /*      GPIO Pin Pull-up/down Enable Clock 1    32      R/W     */
/* 0xA0     -       Reserved */
/* 0xB0                                     Test                                    4       R/W     */

/* bit manipulations macros */
#define BIT(pos) ( 1<<(pos) )
#define BIT_HIGH(i,b) ( i |= BIT(b) )
#define BIT_LOW(i,b) ( i &= ~(BIT(b)) )
#define BIT_SET(i,b,v) ( v ? BIT_HIGH(i,b) : BIT_LOW(i,b) ) 
#define BIT_GET(i,b) ( (i>>(b)) & 1 )

/* GPIO macros */
typedef uint32_t volatile reg32_t; /* this type is BCM2835 register size  32bit */
#define GPIO_RESET_MODE(i,g)         	*(reg32_t *)(i + GPFSEL0 + ((g)/10*4)) &= ~(7<<(((g)%10)*3)) /* 7 is 0b111 */
#define GPIO_IN(i,g)    GPIO_RESET_MODE(i,g)
#define GPIO_OUT(i,g)   GPIO_RESET_MODE(i,g); 	*(reg32_t *)(i + GPFSEL0 + ((g)/10*4)) |=  (1<<(((g)%10)*3)) /* 1 is 0b1  */
#define GPIO_IN_BYTE(i,offset) GPIO_IN(i,offset); GPIO_IN(i,offset+1); GPIO_IN(i,offset+2); GPIO_IN(i,offset+3); \
    GPIO_IN(i,offset+4); GPIO_IN(i,offset+5); GPIO_IN(i,offset+6); GPIO_IN(i,offset+7)
#define GPIO_OUT_BYTE(i,offset) GPIO_OUT(i,offset); GPIO_OUT(i,offset+1); GPIO_OUT(i,offset+2); GPIO_OUT(i,offset+3); \
    GPIO_OUT(i,offset+4); GPIO_OUT(i,offset+5); GPIO_OUT(i,offset+6); GPIO_OUT(i,offset+7)

/* sets bits which are 1 ignores bits which are 0 */
#define GPIO_HIGH(i,g)	(g) < 32 ? (*(reg32_t *)(i + GPSET0) = 1<<(g)) : (*(reg32_t *)(i + GPSET1) = 1<<(g))  

/* clears bits which are 1 ignores bits which are 0 */
#define GPIO_LOW(i,g)   (g) < 32 ? (*(reg32_t *)(i + GPCLR0) = 1<<(g)) : (*(reg32_t *)(i + GPCLR1) = 1<<(g))

#define GPIO_SET(i,g,v) 	( (v) ? GPIO_HIGH(i,g) : GPIO_LOW(i,g) )
#define GPIO_GET(i,g) ( (g) < 32 ? BIT_GET(*(reg32_t *)(i + GPLEV0), (g)) : BIT_GET(*(reg32_t *)(i + GPLEV1), (g)) ) /* 0 if LOW, 1 if HIGH */

/* get gpio 1 byte data starting from n-th GPIO */
#define GPIO_GET_BYTE(i,offset) (uint8_t)((offset) < 32 ? (*(reg32_t *)(i + GPLEV0) >> (offset)) & 0xFF : (*(reg32_t *)(i + GPLEV1) >> (offset)) & 0xFF  )

/* set gpio 1 byte data starting from n-th GPIO */
#define GPIO_SET_BYTE(i, offset, byte)  *(reg32_t *)(i + GPSET0 + (offset)/32) = (reg32_t)byte << ((offset)%32)  
#define GPIO_CLR(i)  *(reg32_t *)(i + GPCLR0) = ~0; *(reg32_t *)(i + GPCLR1) = ~0  /* set gpio low from 0 to 32 */
#define GPIO_CLR_BYTE(i,offset) *(reg32_t *)(i + GPCLR0 + (offset)/32) = 0xFF<<((offset)%32)  /* set gpio 1 byte low starting from offset */


/*int to binary */
#define INT8_PAT(t) #t #t #t #t #t #t #t #t 
#define INT16_PAT(t)  INT8_PAT(t) " " INT8_PAT(t) " "
#define INT32_PAT(t)  INT16_PAT(t) INT16_PAT(t) 
#define INT64_PAT(t)  INT32_PAT(t) INT32_PAT(t) 

#define INT4_2_BIN(i, n)  \
	BIT_GET(i, 0 + n) ? RT"1"NC : "0", \
	BIT_GET(i, 1 + n) ? RT"1"NC : "0", \
	BIT_GET(i, 2 + n) ? RT"1"NC : "0", \
	BIT_GET(i, 3 + n) ? RT"1"NC : "0" 
#define INT8_2_BIN(i, n)	 INT4_2_BIN(i, n + 0),	INT4_2_BIN(i, n + 4) 
#define INT16_2_BIN(i, n)	 INT8_2_BIN(i, n + 0),	INT8_2_BIN(i, n + 8) 
#define INT32_2_BIN(i, n)	 INT16_2_BIN(i, n + 0), INT16_2_BIN(i, n + 16) 
#define INT64_2_BIN(i, n)	 INT32_2_BIN(i, 0),	    INT32_2_BIT(i, 32) 

/* console text and bg colors */
/* background */
#define R "\033[41m"        /* red */
#define G "\033[42m"        /* green */
#define B "\033[44m"        /* blue */
#define Y "\033[43m"        /* yellow */

/* text */
#define RT "\033[1;31m"     /* red */
#define GT "\033[1;32m"     /* green */
#define BT "\033[1;34m"     /* blue */
#define YT "\033[1;33m"     /* yellow */
#define LT "\033[1;30m"     /* black */

#define NC "\033[40m\033[0;37m" /* default text and bg */

/* delay in cycles macro */
#define W(t) for(uint64_t i = clock() + (t); i > clock(); ){__asm__ volatile("nop"); }

void all_in(void *gpio);
void all_out(void *gpio);
void blink(void *gpio, uint8_t pin, uint64_t delay);
void gpio_in(void *gpio, uint8_t pin);
void gpio_out(void *gpio, uint8_t pin);
void gpio_high(void *gpio, uint8_t pin);
void gpio_low(void *gpio, uint8_t pin);
void gpio_switch(void *gpio, uint8_t pin);
void setup_io(void **gpio);
void stat(void *gpio);
void speed_test(void *gpio);
void nsleep(uint32_t ns);
void pud_off(void *gpio, uint8_t pin);	
void pud_up(void *gpio, uint8_t pin);
void pud_down(void *gpio, uint8_t pin);	

