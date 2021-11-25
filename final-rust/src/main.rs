#![no_std]
#![no_main]

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

	let tc1 = dp.TC1;
	tc1
		.tccr1a
		.write(|w| w.wgm1().bits(0b01).com1a().bits(0b11).com1b().bits(0b11));
	tc1.tccr1b.write(|w| w.wgm1().bits(0b01).cs1().bits(0b001));
	uwriteln!(&mut serial, "Configured TC1!\r").void_unwrap();

	let tc2 = dp.TC2;
	tc2.tccr2a.write(|w| w.wgm2().bits(0b11).com2a().bits(0b11));
	tc2.tccr2b.write(|w| w.wgm22().bit(false).cs2().bits(0b001));
	uwriteln!(&mut serial, "Configured TC2!\r").void_unwrap();
	let mut n: u8 = 0;
	loop {
		n = if n == 255 { 0 } else { n + 1 };
		delay_ms(10);
		uwriteln!(&mut serial, "{}", n);

		tc1.ocr1a.write(|w| unsafe { w.bits(n as u16) });
		tc1.ocr1b.write(|w| unsafe { w.bits(n as u16) });
		tc2.ocr2a.write(|w| unsafe { w.bits(n) });
	}
}
