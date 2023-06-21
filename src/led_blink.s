.section .data
    .align 3
    .global gpio_base
gpio_base:
    .quad 0xFE200000   // Base address of GPIO registers

.section .text
    .global _start
_start:
    // Set GPIO pin 16 as an output
    ldr x0, =gpio_base
    ldr x1, [x0, #0x4]   // Read the current value of the GPFSEL1 register
    bic x1, x1, #0xF0000 // Clear bits 18-20 to set as an output
    orr x1, x1, #0x10000 // Set bit 16 to enable output on GPIO pin 16
    str x1, [x0, #0x4]   // Write the updated value back to the GPFSEL1 register

loop:
    // Turn on the LED
    str x1, [x0, #0x1C]  // Write a high value to the GPSET0 register (GPIO pin 16)

    mov x0, 1000000     // Wait for approximately 1 second
wait_loop:
    subs x0, x0, #1
    bne wait_loop

    // Turn off the LED
    str x1, [x0, #0x28] // Write a high value to the GPCLR0 register (GPIO pin 16)

    mov x0, 1000000     // Wait for approximately 1 second
wait_loop2:
    subs x0, x0, #1
    bne wait_loop2

    b loop              // Jump back to the start of the loop
