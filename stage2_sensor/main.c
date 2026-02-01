#include <stdint.h>

#define UART_BASE       0x10013000
#define UART_TXDATA     0x00
#define UART_TXCTRL     0x08
#define UART_DIV        0x18

#define I2C_BASE        0x10030000
#define I2C_PRER_LO     0x00
#define I2C_PRER_HI     0x04
#define I2C_CTR         0x08
#define I2C_TXR         0x0C
#define I2C_RXR         0x0C
#define I2C_CR          0x10
#define I2C_SR          0x10

#define I2C_CTR_EN      0x80
#define I2C_CR_STA      0x80
#define I2C_CR_STO      0x40
#define I2C_CR_RD       0x20
#define I2C_CR_WR       0x10
#define I2C_CR_IACK     0x01

#define I2C_SR_RXNACK   0x80
#define I2C_SR_IF       0x01

#define GPIO_BASE       0x10012000
#define GPIO_VALUE      0x00
#define GPIO_INPUT_EN   0x04
#define GPIO_OUTPUT_EN  0x08
#define GPIO_RISE_IE    0x18
#define GPIO_RISE_IP    0x1C

#define PLIC_BASE       0x0C000000
#define PLIC_PRIORITY(id)  (PLIC_BASE + 4u * (id))
#define PLIC_ENABLE0       (PLIC_BASE + 0x2000)
#define PLIC_THRESHOLD     (PLIC_BASE + 0x200000)
#define PLIC_CLAIM         (PLIC_BASE + 0x200004)

#define IRQ_GPIO0      8
#define BUTTON_PIN     0

#define TMP108_ADDR    0x48
#define TMP108_REG_TEMP 0x00

#define REG32(addr) (*(volatile uint32_t *)(addr))
#define REG8(addr)  (*(volatile uint8_t *)(addr))

static volatile uint32_t button_count = 0;

static inline uint32_t read_csr_mcause(void)
{
    uint32_t v;
    __asm__ volatile("csrr %0, mcause" : "=r"(v));
    return v;
}

static inline void enable_interrupts(void)
{
    /* MEIE (bit 11) */
    __asm__ volatile("csrs mie, %0" :: "r"(1u << 11));
    /* MIE (bit 3) */
    __asm__ volatile("csrs mstatus, %0" :: "r"(1u << 3));
}

static void uart_putc(char c)
{
    while (REG32(UART_BASE + UART_TXDATA) & 0x80000000u) {
        /* wait */
    }
    REG32(UART_BASE + UART_TXDATA) = (uint32_t)c;
}

static void uart_puts(const char *s)
{
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}

static void i2c_init(void)
{
    REG8(I2C_BASE + I2C_PRER_LO) = 0x30;
    REG8(I2C_BASE + I2C_PRER_HI) = 0x00;
    REG8(I2C_BASE + I2C_CTR) = I2C_CTR_EN;
}

static void i2c_wait_if(void)
{
    for (int i = 0; i < 10000; i++) {
        if (REG8(I2C_BASE + I2C_SR) & I2C_SR_IF) {
            REG8(I2C_BASE + I2C_CR) = I2C_CR_IACK;
            break;
        }
    }
}

static uint8_t i2c_read_reg(uint8_t addr, uint8_t reg)
{
    /* START + address (write) */
    REG8(I2C_BASE + I2C_TXR) = (uint8_t)((addr << 1) | 0);
    REG8(I2C_BASE + I2C_CR) = I2C_CR_STA | I2C_CR_WR;
    i2c_wait_if();

    /* write register pointer */
    REG8(I2C_BASE + I2C_TXR) = reg;
    REG8(I2C_BASE + I2C_CR) = I2C_CR_WR;
    i2c_wait_if();

    /* repeated START + address (read) */
    REG8(I2C_BASE + I2C_TXR) = (uint8_t)((addr << 1) | 1);
    REG8(I2C_BASE + I2C_CR) = I2C_CR_STA | I2C_CR_WR;
    i2c_wait_if();

    /* read one byte */
    REG8(I2C_BASE + I2C_CR) = I2C_CR_RD;
    i2c_wait_if();
    uint8_t value = REG8(I2C_BASE + I2C_RXR);

    /* STOP */
    REG8(I2C_BASE + I2C_CR) = I2C_CR_STO;

    return value;
}

static void plic_init(void)
{
    REG32(PLIC_PRIORITY(IRQ_GPIO0)) = 1;
    REG32(PLIC_ENABLE0) = (1u << IRQ_GPIO0);
    REG32(PLIC_THRESHOLD) = 0;
}

static void gpio_init(void)
{
    /* SiFive_GPIO model requires Write-enabled pin for external button driving */
    REG32(GPIO_BASE + GPIO_OUTPUT_EN) |= (1u << BUTTON_PIN);
    REG32(GPIO_BASE + GPIO_INPUT_EN) |= (1u << BUTTON_PIN);
    REG32(GPIO_BASE + GPIO_RISE_IE) |= (1u << BUTTON_PIN);
    REG32(GPIO_BASE + GPIO_RISE_IP) = (1u << BUTTON_PIN);
}

static void handle_external_interrupt(void)
{
    uint32_t claim = REG32(PLIC_CLAIM);
    if (claim == IRQ_GPIO0) {
        button_count++;
        REG32(GPIO_BASE + GPIO_RISE_IP) = (1u << BUTTON_PIN);
        uart_puts("[IRQ] Button pressed, count = ");

        /* print small decimal */
        char buf[12];
        uint32_t v = button_count;
        int i = 0;
        if (v == 0) {
            buf[i++] = '0';
        } else {
            char tmp[12];
            int j = 0;
            while (v > 0 && j < 10) {
                tmp[j++] = (char)('0' + (v % 10));
                v /= 10;
            }
            while (j > 0) {
                buf[i++] = tmp[--j];
            }
        }
        buf[i++] = '\n';
        buf[i] = '\0';
        uart_puts(buf);

        int8_t temp = (int8_t)i2c_read_reg(TMP108_ADDR, TMP108_REG_TEMP);
        uart_puts("[IRQ] Temp = ");
        char tbuf[8];
        int ti = 0;
        if (temp < 0) {
            tbuf[ti++] = '-';
            temp = (int8_t)(-temp);
        }
        if (temp >= 10) {
            tbuf[ti++] = (char)('0' + (temp / 10));
        }
        tbuf[ti++] = (char)('0' + (temp % 10));
        tbuf[ti++] = ' ';
        tbuf[ti++] = 'C';
        tbuf[ti++] = '\n';
        tbuf[ti] = '\0';
        uart_puts(tbuf);
    }
    REG32(PLIC_CLAIM) = claim;
}

void trap_handler_c(void)
{
    uint32_t mcause = read_csr_mcause();
    if (mcause == 0x8000000Bu) {
        handle_external_interrupt();
    }
}

int main(void)
{
    /* UART init */
    REG32(UART_BASE + UART_TXCTRL) = 1;   /* enable TX */
    REG32(UART_BASE + UART_DIV) = 27;     /* arbitrary divider */

    uart_puts("stage2_sensor: init\n");

    gpio_init();
    plic_init();
    i2c_init();
    enable_interrupts();

    uart_puts("stage2_sensor: ready, press button in Renode\n");

    while (1) {
        __asm__ volatile("wfi");
    }

    return 0;
}
