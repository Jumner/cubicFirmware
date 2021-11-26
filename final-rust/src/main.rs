#![no_std]
#![no_main]
#![feature(abi_avr_interrupt)]

use arduino_hal::prelude::*;
use arduino_hal::*;
use panic_halt as _;
use ufmt::uwriteln;

// #[arduino_hal::entry]
fn main() -> ! {
	let dp = Peripherals::take().unwrap();
	let pins = pins!(dp);
	let mut serial = default_serial!(dp, pins, 57600);

	pins.d9.into_output();
	pins.d10.into_output();
	pins.d11.into_output();
	let inp = pins.d5.into_floating_input();

	let int = dp.EXINT; // Get the interrupt register
	int.pcicr.write(|w| w.pcie().bits(0b100)); // Enables pcie0
	int.pcmsk2.write(|w| w.pcint().bits(0b11100000)); // Enables pins 7,6,5 (pcint 23,22,21)

	let tc1 = dp.TC1; // Get tc1 timer
	tc1 // https://www.gammon.com.au/images/Arduino/Timer_1.png
		.tccr1a // Get the tccr1a register
		.write(|w| w.wgm1().bits(0b01).com1a().bits(0b11).com1b().bits(0b11)); // Write half of fast pwm to 255, inverted d9,d10 outputs
	tc1
		.tccr1b // Get the tccr1b register
		.write(|w| w.wgm1().bits(0b01).cs1().bits(0b001)); // Write other half of fast pwm to 255, prescaler of 1 (~30khz)
	uwriteln!(&mut serial, "Configured TC1!\r").void_unwrap();

	let tc2 = dp.TC2; // Get tc2 timer
	tc2 // https://www.gammon.com.au/images/Arduino/Timer_2.png
		.tccr2a // Get the tccr2a register
		.write(|w| w.wgm2().bits(0b11).com2a().bits(0b11)); // Write 2 lsb of fast pwm to 255, d11 inverted output
	tc2
		.tccr2b // Get the tccr2b register
		.write(|w| w.wgm22().bit(false).cs2().bits(0b001)); // Write 1 msb of fast pwm to 255, prescaler of 1 (~30khz)
	uwriteln!(&mut serial, "Configured TC2!\r").void_unwrap();

	// That is 2 lines in arduino 😅

	let mut n: u8 = 0;
	loop {
		n = if n == 255 { 0 } else { n + 1 };
		delay_ms(10);
		// uwriteln!(&mut serial, "{}", n);
		uwriteln!(&mut serial, "{}", inp.is_high());

		tc1.ocr1a.write(|w| unsafe { w.bits(n as u16) });
		tc1.ocr1b.write(|w| unsafe { w.bits(n as u16) });
		tc2.ocr2a.write(|w| unsafe { w.bits(n) });
	}
}
