

# Files

* *model.skp* is a [Sketchup](http://www.sketchup.com/) file, used to find dimensions and locations for the board and enclosure cutouts. *model.pdf* is an export of this file.
* *schematic.fzz* is a [Fritzing](http://fritzing.org/) file with the circuit schematic. *schematic.pdf* is an export of this file.
* *firmware.ino* is a text file containing the source code. It can be uploaded to the chip over USB using either the [Arduino IDE](http://arduino.cc/en/main/software) or from the command line using [Ino](http://inotool.org/).
* *bom.pdf* is the bill of materials.


# Circuit notes

The following components are not included in the schematic:

1. The USB receptacle's metal shielding is connected to ground through a 1MΩ resistor and a 10nF capacitor.
2. Another capacitor is placed between Gnd and Vcc.

Also, note that the resistors and capacitors are not included in the bill of materials.
