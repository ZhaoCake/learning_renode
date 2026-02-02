#include <stdint.h>

// Custom RISC-V Platform Constants (defined in my_platform.repl)

// GPIO Controller Base Address (Moved to 0x40000000)
#define GPIO_BASE       0x40000000

// Offsets
#define GPIO_VALUE      0x00
#define GPIO_INPUT_EN   0x04
#define GPIO_OUTPUT_EN  0x08
#define GPIO_PORT       0x0C
#define GPIO_PUE        0x10
#define GPIO_DS         0x14
#define GPIO_RISE_IE    0x18
#define GPIO_RISE_IP    0x1C
#define GPIO_FALL_IE    0x20
#define GPIO_FALL_IP    0x24
#define GPIO_HIGH_IE    0x28
#define GPIO_HIGH_IP    0x2C
#define GPIO_LOW_IE     0x30
#define GPIO_LOW_IP     0x34
#define GPIO_IOF_EN     0x38
#define GPIO_IOF_SEL    0x3C
#define GPIO_OUT_XOR    0x40

// Register Pointers
#define REG32(base, offset) (*(volatile uint32_t *)((base) + (offset)))

#define GPIO_REG(offset) REG32(GPIO_BASE, offset)

// LEDs on HiFive1
// Green: Pin 19
// Blue:  Pin 21
// Red:   Pin 22

#define LED_GREEN_OFFSET 19
#define LED_BLUE_OFFSET  21
#define LED_RED_OFFSET   22

#define LED_GREEN   (1 << LED_GREEN_OFFSET)
#define LED_BLUE    (1 << LED_BLUE_OFFSET)
#define LED_RED     (1 << LED_RED_OFFSET)

void delay(int count) {
    while (count--) {
        __asm__("nop");
    }
}

int main(void) {
    // 1. Enable Output for LEDs
    // 对于我们自定义的平台，只需要把对应位配置为输出即可
    GPIO_REG(GPIO_OUTPUT_EN) = (LED_GREEN | LED_BLUE | LED_RED);

    // 2. Loop
    // 注意：Renode 的 Miscellaneous.LED 默认为 active-high。
    // 我们用最直观的方式：写 1 点亮，写 0 熄灭。
    
    while (1) {
        // Red ON
        GPIO_REG(GPIO_VALUE) = LED_RED;
        delay(100000);

        // Green ON
        GPIO_REG(GPIO_VALUE) = LED_GREEN;
        delay(100000);

        // Blue ON
        GPIO_REG(GPIO_VALUE) = LED_BLUE;
        delay(100000);
    }
    
    return 0;
}
