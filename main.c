#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRD = 0xFF;

    while (1) {
        PORTD = 0xFF;
        _delay_ms(500);
        PORTD = 0x00;
        _delay_ms(500);
    }
}
