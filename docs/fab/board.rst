.. _board:

Circuit board fabrication and assembly
=========================================

The purpose of this procedure is describe the steps to assemble a functioning LPA circuit board from parts listed in Supplementary Table S1 using equipment listed in Supplementary Table S2. There are two major steps: 1) Fabrication of the unpopulated LPA Printed Circuit Board (PCB), and 2) Assembly, or soldering of the surface mounted and through-hole components. Step 1 is performed via a commercial supplier. For step 2, we have provided do-it-yourself and commercial protocols.

1. Fabrication
------------------
The unpopulated LPA PCB can be ordered from a company by providing the six gerber (.gxx) files and one drill (.drl) file in the Supplementary Files. There are many companies that provide this service. Companies will manufacture PCBs at different costs per PCB depending on the order size. We have utilized OSH Park (Portland, OR), which has fabricated three LPA PCBs (minimum order) for $82.20. OSH Park does not perform PCB component assembly (see below).

2 (option A). Do-it-yourself assembly
--------------------------------------------
Surface mounted component soldering:

1.	Tape the printed circuit board (PCB) to a benchtop, and align and tape PCB stencil on top. Smear solder paste over the stencil.
2.	Peel the stencil up, leaving solder paste over electrical contacts.
3.	Use tweezers to place surface mount components, following the component placement diagram (Supplementary Files).
4.	Place the populated PCB into a toaster oven and adjust the oven temperature setting according to the following sequence: 150°C for 2 min; 180°C for 1 min; 200°C for 30 s; and 230°C until solder is melted (usually 1-1.5 min). Times indicate when to make the temperature setting adjustment, not when the oven temperature is reached.
5.	Open the oven door, pull out the rack and cool for 1 min before moving the board.
6.	Inspect the solder connections on the board for bridged or failed connections. Use the soldering kit to add or remove solder where necessary (an inspection microscope is helpful for this step).

Through-hole component soldering
-----------------------------------
LED socket alignment and soldering is demonstrated in Supplementary Video S1. Briefly:

1.	Use the 3D printed LED socket aligner (Supplementary Fig. S7) to make sure that the LED sockets are positioned and oriented evenly. Use tape to secure LED sockets in place.

2.	Invert the circuit board and solder LED sockets to the circuit board using a soldering kit.

Once all components are soldered, use a digital multimeter (DMM) to check the following: (a) there should be no connection between power and ground, (b) the ground pin on the power connector should be conducting with the ground pins on the microcontroller and each of the LED drivers, (c) the 3.3V output of the voltage regulator should be conducting with the power pins on the microcontroller and each of the LED drivers, (d) LED driver pins and associated PCB pads should conduct (good alignment can be tricky).

2 (option B). Commercial assembly
-------------------------------------
Many companies will assemble surface and through hole components. If you choose this option, you would typically have the same company fabricate and assemble the circuit board. Cost for these services reduces dramatically with order size. For example, Screaming Circuits (Canby, OR) has quoted us fabrication and assembly cost at approximately $950 for one or $1200 for ten boards.

In addition to providing the information necessary for board fabrication (see above), a bill of materials (BOM) and component placement file must be provided (Supplementary Files). Companies will charge significantly more for performing assembly on two sides of the board. Because there are only two electrical components on the front layer of the LPA board (excluding LED sockets), we recommend only ordering assembly on the back layer. The male molex connector and reset switch can easily be soldered by hand. In any case, due to the need for the custom LED socket aligner tool, we do not recommend having a company solder the LED sockets. This should be done by hand as described above.

3. Powering the circuit board
-----------------------------------------
Once the circuit board components have been soldered and inspected, the device can be powered on:

1.	Construct the barrel-jack power adapter by stripping two ~8 in lengths of multicore wire.
2.	Use the molex terminal crimper to attach terminals to the wire, and insert the terminals into the female molex housing.
3.	Strip and insert the other ends of the wire into the barrel jack connector, and screw down the terminals to keep the wires fixed.
4.	Plug in the AC wall adapter and connect it to the power adapter you have just constructed (but not yet connected to the circuit board).
5.	**Important**: Make sure that power and ground are in the correct positions with a DMM. The positive terminal has a square pad on the back side of the PCB.
6.	Connect the power adapter to the circuit board. Use the back of your index finger to test whether any of the components are heating up. If the components remain cool, then the board has been built properly.
