# rfid
Arduino RFID cloner code

I used a generic RFID reader I bought from eBay to use as the reader for the Snooper half of the circuit.

Snooper code and circuit work perfectly fine but the Spoofer code and circuit kind of work with my snooper but are not even registered by HID readers.

The important part of my snooper code that makes it able to read the HID cards is that it checks for a preamble of 00011101 which is what HID uses in the line of cards I am trying to read.

The spoofer code is a mess that should be cleaned up, but I would rather make it work than make it pretty.
