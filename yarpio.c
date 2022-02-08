/* Yet Another Raspberry Pi GPIO Library YARPIO 
 * <yarpio.c>
 *
 * Copyright (c) 2021, legale <legale.legale@gmail.com>
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h> /* integer types uint* */
#include <time.h> /* clock() */
#include <string.h>
#include "yarpio.h"


inline void nsleep(uint32_t ns){
    /*  struct timespec {
     *      time_t tv_sec;  // seconds 
     *      long   tv_nsec; // nanoseconds
     *  }; */
    struct timespec ts = { 0, (long)ns };

    nanosleep(&ts, &ts);
}

void all_in(void *gpio){ 
	for (int i = 0; i < 28; i++){
		GPIO_IN(gpio, i);
	} 
}

void all_out(void *gpio){
        for (int i = 0; i < 28; i++){
        GPIO_LOW(gpio, i);
		GPIO_OUT(gpio, i);
	}
}

void speed_test(void *gpio){
	int g = 3;
	int r, rounds;
	r = rounds = 1e6;
	clock_t begin = clock();
	GPIO_OUT(gpio, g);

	while(r--){
		GPIO_HIGH(gpio, g);
		GPIO_LOW(gpio, g);
	}

	clock_t end = clock();

	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("rounds: %d in %f sec.\n", rounds, time_spent);
	printf("Speed in MHZ: %f\n", 1 / time_spent);

}

void blink(void *gpio, uint8_t pin, uint64_t delay){
	printf("blinking pin: %d with delay: %lu\n", pin, delay);
	GPIO_OUT(gpio, pin);
	while(1){
		GPIO_SET(gpio, pin, 1);
		W(delay);
		GPIO_SET(gpio, pin, 0);
		W(delay);
	}
    GPIO_LOW(gpio, pin);
}

void pud_off(void *gpio, uint8_t pin){
	reg32_t *p = gpio + GPPUD;
    reg32_t *clk = pin < 32 ? gpio + GPPUDCLK0 : gpio + GPPUDCLK1;
	*p &= ~3;
	*p |= 1 << 1; /* set bit 1 eq 1 0b01  */
    W(150);
    BIT_HIGH(*clk, pin);
    W(150);
    *p &= ~3;
    BIT_LOW(*clk, pin);
}

void pud_up(void *gpio, uint8_t pin){	
	reg32_t *p = gpio + GPPUD;
    reg32_t *clk = pin < 32 ? gpio + GPPUDCLK0 : gpio + GPPUDCLK1;
	*p &= ~3;
	*p |= 1 << 1; /* set bit 1 eq 1 0b10  */
    W(150);
    BIT_HIGH(*clk, pin);
    W(150);
    *p &= ~3;
    BIT_LOW(*clk, pin);
}

void pud_down(void *gpio, uint8_t pin){	
	reg32_t *p = gpio + GPPUD;
    reg32_t *clk = pin < 32 ? gpio + GPPUDCLK0 : gpio + GPPUDCLK1;
	*p &= ~3; 
	*p |= 1; /* set bit 0 eq 1 0b1 */
    W(150);
    BIT_HIGH(*clk, pin);
    W(150);
    *p &= ~3;
    BIT_LOW(*clk, pin);
}

void gpio_in(void *gpio, uint8_t pin){
	GPIO_IN(gpio, pin);
}

void gpio_out(void *gpio, uint8_t pin){
	GPIO_OUT(gpio, pin);
}

void gpio_high(void *gpio, uint8_t pin){
	GPIO_HIGH(gpio, pin);
	printf("pin: %d value: %d\n", pin,  GPIO_GET(gpio, pin));
}

void gpio_low(void *gpio, uint8_t pin){
	GPIO_LOW(gpio, pin);
	printf("pin: %d value: %d\n", pin,  GPIO_GET(gpio, pin));
}

void gpio_switch(void *gpio, uint8_t pin){
    GPIO_SET(gpio, pin, !GPIO_GET(gpio, pin));
}

void status(void *gpio){
	void *i = gpio;
	printf("GPIO function select registers GPFSEL:\n");
	printf("GPFSEL0 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPFSEL0, INT32_2_BIN(*(uint32_t *)(i + GPFSEL0), 0));
	printf("GPFSEL1 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPFSEL1, INT32_2_BIN(*(uint32_t *)(i + GPFSEL1), 0));
	printf("GPFSEL2 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPFSEL2, INT32_2_BIN(*(uint32_t *)(i + GPFSEL2), 0));
	printf("GPFSEL3 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPFSEL3, INT32_2_BIN(*(uint32_t *)(i + GPFSEL3), 0));
	printf("GPFSEL4 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPFSEL4, INT32_2_BIN(*(uint32_t *)(i + GPFSEL4), 0));
	printf("GPFSEL5 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPFSEL5, INT32_2_BIN(*(uint32_t *)(i + GPFSEL5), 0));
	printf("GPPUDCLK0 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPPUDCLK0, INT32_2_BIN(*(uint32_t *)(i + GPPUDCLK0), 0));
	printf("GPPUDCLK1 0x%X:\n"INT32_PAT(%s)"\n", GPIO_BASE + GPPUDCLK1, INT32_2_BIN(*(uint32_t *)(i + GPPUDCLK1), 0));

  
	printf("GPIO Pull-up/down register GPPUD 0x%X:\n", GPIO_OFFSET + GPPUD); 
	printf(INT32_PAT(%s)"\n", INT32_2_BIN(*(uint32_t *)(i + GPPUD), 0));
	          

	printf( "GPIO pins values:\n"
    "        _____    \n"
	"       |  |  |   \n"
	"     3V|%s|%s|5V \n"
	"     2 |%s|%s|5V \n"
	"     3 |%s|%s| G \n"
	"     4 |%s|%s|14 \n"
	"     G |%s|%s|15 \n"
	"     17|%s|%s|18 \n"
	"     27|%s|%s| G \n"
	"     22|%s|%s|23 \n"
	"     3V|%s|%s|24 \n"
	"     10|%s|%s| G \n"
	"     9 |%s|%s|25 \n"
	"     11|%s|%s| 8 \n"
	"     G |%s|%s| 7 \n"
	"     0 |%s|%s| 1 \n"
	"     5 |%s|%s| G \n"
	"     6 |%s|%s|12 \n"
	"     13|%s|%s| G \n"
	"     19|%s|%s|16 \n"
	"     26|%s|%s|20 \n"
	"     G |%s|%s|21 \n"
	"       |__|__|   \n"

	, 
	                  G"1 "NC       ,                   G" 2"NC       ,
	GPIO_GET(i, 2) ? RT"3 "NC : "3 ",                   G" 4"NC       ,
	GPIO_GET(i, 3) ? RT"5 "NC : "5 ",                LT Y" 6"NC       ,
	GPIO_GET(i, 4) ? RT"7 "NC : "7 ", GPIO_GET(i,14) ? RT" 8"NC : " 8",
	               LT Y"9 "NC       , GPIO_GET(i,15) ? RT"10"NC : "10",
	GPIO_GET(i,17) ? RT"11"NC : "11", GPIO_GET(i,18) ? RT"12"NC : "12",
	GPIO_GET(i,27) ? RT"13"NC : "13",                LT Y"14"NC       ,
	GPIO_GET(i,22) ? RT"15"NC : "15", GPIO_GET(i,23) ? RT"16"NC : "16",
	                  G"17"NC       , GPIO_GET(i,24) ? RT"18"NC : "18",
	GPIO_GET(i,10) ? RT"19"NC : "19",                LT Y"20"NC       ,
	GPIO_GET(i, 9) ? RT"21"NC : "21", GPIO_GET(i,25) ? RT"22"NC : "22",
	GPIO_GET(i,11) ? RT"23"NC : "23", GPIO_GET(i, 8) ? RT"24"NC : "24",
	               LT Y"25"NC       , GPIO_GET(i, 7) ? RT"26"NC : "26",
	GPIO_GET(i, 0) ? RT"27"NC : "27", GPIO_GET(i, 1) ? RT"28"NC : "28",
	GPIO_GET(i, 5) ? RT"29"NC : "29",                LT Y"30"NC       ,
	GPIO_GET(i, 6) ? RT"31"NC : "31", GPIO_GET(i,12) ? RT"32"NC : "32",
	GPIO_GET(i,13) ? RT"33"NC : "33",                LT Y"34"NC       ,
	GPIO_GET(i,19) ? RT"35"NC : "35", GPIO_GET(i,16) ? RT"36"NC : "36",
	GPIO_GET(i,26) ? RT"37"NC : "37", GPIO_GET(i,20) ? RT"38"NC : "38",
	               LT Y"39"NC       , GPIO_GET(i,21) ? RT"40"NC : "40"

			     );

}


/*
 * Set up a memory regions to access GPIO
 */
void setup_io(void **gpio)
{
  int mem_fd = open("/dev/mem", O_RDWR|O_SYNC); /* memory file descriptor */
  if (mem_fd < 0) {
    printf("can't open /dev/mem %d\n", mem_fd);
    exit(-1);
  }
  /* mmap GPIO */
  *gpio = mmap(
    NULL,             /* Any adddress in our space will do */
    GPIO_SIZE,       /* Map length */
    PROT_READ|PROT_WRITE,/* Enable reading & writting to mapped memory */
    MAP_SHARED,       /* Shared with other processes */
    mem_fd,           /* File to map */
    GPIO_BASE         /* Offset to GPIO peripheral */
  );

  close(mem_fd); /* No need to keep mem_fd open after mmap */

  if (*gpio == MAP_FAILED) {
    printf("mmap error %ld\n", (int64_t)*gpio);//errno also set!
    exit(-1);
  }

}

