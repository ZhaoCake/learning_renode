#include <stdint.h>

#define GPIO_BASE       0x40000000

#define GPIO_VALUE      0x00
#define GPIO_OUTPUT_EN  0x08

#define REG32(base, offset) (*(volatile uint32_t *)((base) + (offset)))
#define GPIO_REG(offset) REG32(GPIO_BASE, offset)

#define LED_GREEN_OFFSET 19
#define LED_BLUE_OFFSET  21
#define LED_RED_OFFSET   22

#define LED_GREEN   (1 << LED_GREEN_OFFSET)
#define LED_BLUE    (1 << LED_BLUE_OFFSET)
#define LED_RED     (1 << LED_RED_OFFSET)

static void delay(int count) {
    while (count--) {
        __asm__("nop");
    }
}

int main(void) {
    GPIO_REG(GPIO_OUTPUT_EN) = (LED_GREEN | LED_BLUE | LED_RED);

    while (1) {
        GPIO_REG(GPIO_VALUE) = LED_RED;
        delay(100000);

        GPIO_REG(GPIO_VALUE) = LED_GREEN;
        delay(100000);

        GPIO_REG(GPIO_VALUE) = LED_BLUE;
        delay(100000);
    }

    return 0;
}
