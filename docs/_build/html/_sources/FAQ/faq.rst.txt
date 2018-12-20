FAQ
======================
Roughly in order of importance and how frequently asked.


| **Q1: What should I do if my question isn't addressed in this FAQ?**
| A1: Check the [issues](https://github.com/taborlab/LPA-hardware/issues) (including closed issues) or raise a new issue on github.

| **Q2: Where can I find the latest version of the LPA hardware and firmware?**
| A2: Check the ["releases"](https://github.com/taborlab/LPA-hardware/releases) tab in the github repo for the latest version.

| **Q3: What is the difference between the latest LPA version and the version in the publication (v1.0)?**
| A3: Changes are described in the notes with each release.

| **Q4: Are the electrical components used with LPA v1.0 the same/compatible with the latest LPA version?**
| Q4: As of LPA v1.1, most of the components are the same but some have changed. With each release, the bill of materials (BOM) is updated.

| **Q5: Where can I find the list of electrical components for the latest version of the LPA?**
| A5: The bill of materials (BOM) for electrical components is located in the directory "CAD Files\Circuit Board\Production Files" of each release. The BOM and component spreadsheets are formatted so they can be sent for commercial assembly. Note that the BOM does not include the LED sockets which should be assembled and soldered manually with the LED socket aligner.

| **Q6: A component in the bill of materials is no longer available, what should I do?**
| A6: Check if anyone has raised an [issue](https://github.com/taborlab/LPA-hardware/issues) for the same part on github. If not, let us know by submitting one. When this happens the supplier often suggests a substitute or related item on the product page. If there is no obvious substitute there may be an alternative supplier.

| **Q7: Where I can find the PCB stencil file, and is there a corresponding .pdf, or .doc (or something similar) that I could open on my PC?**
| A7: PCB production files (Gerbers) are located in directory "CAD Files\Circuit Board\Production Files". In Gerber format, stencil files are specified by ".gtp" (gerber top paste) and ".gbp" (gerber bottom paste). Since we recommend only performing [surface mount component soldering](http://taborlab.github.io/LPA-hardware/) on the bottom layer, you will most likely only need the bottom stencil file. The entire PCB layout can be viewed (and modified) in KiCAD by opening the .pro file located in "CAD Files\Circuit board\KiCAD Files". You can also find free software online for opening individual gerber files.

| **Q8: How do I determine the correct indicator LED orientation?**
| A8: The most obvious feature on the indicator LEDs is a "T" shape on the back which indicates polarity. Even though all three indicator LEDs are from the same manufacturer, the graphic is not consistent in indicating polarity. For these LEDs, the bottom of the "T" feature indicates:

| Green("on", 475-1410-1-ND): cathode (GND)
| Red("err", 475-1415-1-ND): anode
| Yellow("fin", 475-2560-1-ND): cathode (GND)

In LPA v1.0, GND is marked by a small nub in the silkscreen. Therefore, the bottom of the "T" should point toward (green), away from (red), and toward (yellow) the silkscreen nub.

In LPA v1.1, the inconsistency is taken into account, and the silkscreen nub indicates the correct side of the "T" feature. Therefore, the bottom of the "T" should point toward the silkscreen nub for all three LEDs.

| **Q9: LPA v1.1 indicator LEDs are located on top of the circuit board, how should I solder these?**
| A9: Even if you are outsourcing the SMC soldering to a company, you will likely solder the indicator LEDs yourself since keeping assembly to one side of the board significantly reduces cost. Depending on your circumstances, there are multiple ways of doing it:

*Soldering iron method*: Choose this method if you are not set up to perform reflow soldering, or if your board has already been commercial assembled on the back and you don't want to risk messing it up. First, use tweezers to place the indicator LED (link!). Take your hot soldering iron (the smaller the tip the better) and get a small drop of solder on the very tip. While holding the indicator LED in place with tweezers, transfer the small drop of solder to one terminal of the LED. Once one terminal is even slightly soldered in place, you can solder the other terminal relatively easily. Ensure good conductance between LED terminal and pad by heating both areas with iron and re-melting the solder.

*Reflow (oven) method*: Two successive rounds of reflow soldering can be performed to get all SMCs soldered on both sides of the circuit board. There are YouTube videos demonstrating how to do it. By this method, you perform reflow soldering on one side of the board, let it cool, and then perform reflow soldering on the second side. In the second round, the board needs to be raised off the oven rack so that it's not resting on any components. As long as the SMCs which are facing down are not too heavy, surface tension will keep them in place. If you try this method, keep the back side of the circuit board facing up during the second round of reflow. This way only the LED indicators fall off in the worst case scenario.

| **Q10: I'm having trouble programming the firmware, is there a set of quick instructions I can follow?**
| A10:

1. Download/install Atmel Studio 7.0  (web installer)
2. Plug in AVRISP mkii (programmer) to computer usb (wait ~10s for computer to recognize device)
3. Power LPA and connect to programmer, programmer indicator LED should change from red to green
4. In Atmel Studio go to Tools>Device Programming. Select AVRISP mkII as tool, ATmega328 as device, ISP as interface. Press read device signature and voltage (3.3 V).
5. Click on the "Fuses" tab. Use the settings below (also found in Firmware>Compiled>fuses.txt) and then click Program.

     LPA v1.0: uncheck CKDIV8 box, check CKOUT box, and set SUT_CKSEL to EXTXOSC_8MHZ_XX_1KCK_14CK_65MS.

     LPA v1.1: uncheck CKDIV8 box, check CKOUT box, and set SUT_CKSEL to EXTXOSC_8MHZ_XX_16KCK_14CK_65MS.

6. Click on the "Production File" tab. Click the "..." button under "Program device from ELF production file". Select fimware.elf located in Firmware>Compiled. Activate the "Flash", "Erase memory before programming", and "Verify programmed content" boxes. Click the "Program" button. Your LPA should now be programmed.

| **Q11: I cannot get my computer/Atmel Studio 7 to recognize and display my programmer, what do I do?**
| A11: Most likely, this is either an issue with the drivers on your computer (easiest thing to do is try another computer) or an incompatible programmer. We strongly recommend using an AVRISP mkII, or AVR dragon.

| **Q12: Atmel Studio recognizes my programmer, but it does not read the microcontroller or I get an error message when trying to program the device:**
| A12: There are several possible reasons to encounter this problem which all point to not having good/clean soldering job. In this order ensure that the following is true:

1. The power regulator is receiving 5 V DC and outputing 3.3 V to the microcontroller VCC pins.
2. Voltage between microcontroller VCC pins and GND pins reads 3.3 V.
3. Every pin of every component (especially the microcontroller pins) aligns and conducts with it's corresponding pad on the circuit board.
4. There are no bridged connections anywhere on the board.
5. There is no solder debris (e.g. lead flakes) causing a short anywhere on the board or on any component.

| **Q13: Once the firmware has been programmed, does the LPA need to be connected to a computer in order to run?**
| A13: No, once the firmware has been programmed the LPA requires no external connection other than a 5V power supply.

| **Q14: Some LEDs (possibly on large sections of the board) are not lighting up, what is causing this?**
| A14: Most likely bad soldering or bad alignment between pins on one or more LED driver and the circuit board. To solve, check for conductance between each LED driver pin and corresponding circuit board pad. If you find a bad connection, re-melt and spread around the solder on that pin/pad until you get a good connection. You may need to add additional solder (which can be tricky).

| **Q15: The circuit board component parts list calls for a quantity of 0.15 breakaway headers per LPA. What does that mean?**
| A15: The [breakaway headers](http://taborlab.github.io/LPA-hardware/) are supplied as strips, 40 headers long, which you must manually break off to the desired length. The LPA requires two rows of three headers, for a total of six (0.15 x 40) headers.

| **Q16: Why does the [Supplementary Information](https://media.nature.com/original/nature-assets/srep/2016/161102/srep35363/extref/srep35363-s1.pdf) state there are 7 PCB production files (6 gerber, 1 drill) when there seem to be 8 files (7 gerber, 1 drill)?**
| A16: We made a typo there. It should have stated 7 gerbers, 1 drill at that time. Luckily you cannot fabricate the board if any of those files is missing. If you're fabricating with OSHPark, you can literally just zip the production files folder and drag and drop it into their website. As of release x.xx we also include the back solder paste stencil (.gbp) file.

| **Q17: Does the microcontroller need to be programmed first in order for the LEDs to light up?**
| A17: Yes, the LEDs should not light up until the microcontroller has been programmed with the firmware.

| **Q18: What files need to be on the SD card to run the LPA?**
| A18: There need to be three files: dc.txt, gcal.txt, and program.lpf. The LPA will ignore any other files on the SD card.

| **Q19: What capacity SD card do I need for the LPA?**
| A19: Three files must be on the SD card to run the LPA: gcal.txt (100-148 bytes), dc.txt (100-196 bytes), and program.lpf (variable). The size of program.lpf (generated by Iris) depends the length of experiment, where:

| Size (MB) = 0.3456 \* time (h), time =< 12
| Size (MB) = 0.03456 \* time (h), time > 12

| **Q20: How do I compensate LEDs across multiple LPAs to output the same intensity?**
| A20: There are multiple approaches:

* DC/GS compensation: Calibrate (link!) your LEDs to output the same intensity across multiple devices. To do this you need to measure the LED intensities across multiple devices by the same method, and then calibrate all LEDs to the single lowest LED ouput.

* Iris compensation: Compensate the IRIS intensities so that you get the same LED intensities. e.g. if you have 3 LEDs in 3 devices with max intensities 10, 5, and 2.5 umol/m^2/s, you could use IRIS intensities of 1000, 2000, 4000 to make them produce the same amount of light.

Evan Olson wrote some software for his MSB paper to automate the program generation given different LED outputs.

| **Q21: Where can I find a 1:1 scale image of the gaskets or their dimensions?**
| A21: Gasket files are contained in the directory: CAD Files\Gaskets. Individual gasket files are in .dxf format, which can be opened in many graphics editors and can easily be converted to other file formats. A complete gasket set is also provided in .ai format which meets the specifications of the laser cutting company Pololu. [Gasket fabrication](http://taborlab.github.io/LPA-hardware/) is described in the documentation.

| **Q22: The company assembling LPA v1.1 for me has warned there is a mismatch between the power connector (KLDHCX-0202-A-LT) pins, which are thin and rectangular, and the PCB footprint, which consists of circular through holes. Is this actually an issue and what should I do?**
| A22: The rounded holes were made because our recommended PCB manufacturing company, OSHPark, didn't allow for non-circular holes at the time. However, soldering this component by hand is trivial. If for some reason you must have a company assemble this component, and they will not proceed with the existing design, you can modify the footprint in KiCAD to what your manufacturing company recommends. Feel free to contribute your modified design to the github repo so others may use it.

| **Q23: What is your protocol for reusing LPA culture plates?**
| A23: For bacteria and yeast we do the following:

1. After previous experiment, dispose cultures from wells and rinse with DI H2O
2. Submerge plate in a ~75% EtOH bath for >=30mins
3. Remove from EtOH bath, vigorously shake off residual liquid
4. Dry under flame until all liquid is evaporated (~10-15 min)
5. Dry plates can be stored by wrapping in aluminum foil.

| For mammalian cells:
| There was also a PLL coating step (see papers).

| **Q24: The power supply is not listed in the BOM, what should I use?**
| A24: The BOM only includes PCB components. The power supply should be a 5V DC switching power supply. See documentation.

| **Q25: The company assembling LPA v1.0 for me noted that the TAB pin on the voltage regulator (LD1117S33TR) is internally connected to VOUT, but the corresponding pad on the circuit board is not routed to anything. Would you recommend keeping it this way?**
| A25: We do not know why the company pointed this out. Soldering that pin is only for structural support and heat dissipation. We do not recommend modifying the routing of that pad.

| **Q26: What LEDs should I use with the LPA?**
| A26: Any standard 5 mm LED can be used with the LPA. The wavelength you use depends on your optogenetic system and experiment. For examples see:

[Gerhardt, et al. "An open-hardware platform for optogenetics and photobiology." Scientific Reports 6 (2016)](https://www.nature.com/articles/srep35363)

[Olson, et al. "A photoconversion model for full spectral programming and multiplexing of optogenetic systems." Molecular Systems Biology 13(4) (2017)](http://msb.embopress.org/content/13/4/926)

| **Q27: It looks like the LPA LEDs are underneath the plate, but the paper mentions a top and bottom configuration.  Which is it?**
| A27: Each well has two LEDs which sit underneath the culture plate. When looking at an LPA well from a top-down perspective (as in [Figure 1a.](https://www.nature.com/articles/srep35363)) we refer to these as the top and bottom LED positions.

| **Q28: What shaking incubators is the LPA compatible with?**
| A28: Unless your shaker platform has the same hole pattern as ours (Shel Labs SI9/SI9R), you will need to modify the mounting plate (CAD Files>3D Printed Parts>LPA Parts>mounting plate) to your shaker platform specifications. Otherwise, the LPA is compatible with any shaker.

| **Q29: Do you have plans to release the LPA with other well formats (e.g. 12-well, 96-well)?**
| A29: Not currently. We were working on a 96-well prototype, but found it produced too much heat to be useful. Building and running multiple LPAs in parallel has worked well for us.

| **Q30: Can I use an alternative plate/well format with the LPA?**
| A30: You can try, but unfortunately the LPA is designed around one specific culture plate. If you use a different plate, the wells may not align with the LEDs and the outer plate dimensions may not fit the plate adapter. You are likely to get non-uniform illumination and cross-illumination between wells. We encourage users to modify the LPA designs to their own specifications and share on github.

| **Q31: I cannot compile the firmware because I'm missing the file "SPI.cpp". Do you know where I can get this file?**
| A31: Unless you're planning on modifying the firmware, you don't need to recompile any source code. You should go through the "Firmware Programming" procedure, section 2: "Programming the device", in the SI of the [paper](https://images.nature.com/original/nature-assets/srep/2016/161102/srep35363/extref/srep35363-s1.pdf). In addition, open file "fuses.txt" and set the fuses as specified there (this replaces step e. in the instructions).

If you really need to recompile the firmware, follow the instructions in the section "Firmware compiling". The procedure is rather complicated, and involves making Atmel Studio use some files from the Arduino library

| **Q32: Does the resonator (535-10008-1-ND) used in LPA v1.0 have directionality?**
| A32: No, as long as the middle terminal gets soldered to the middle pad, the end terminals can be on either side.

| **Q33: When calibrating the LEDs with the spectrophotometer probe, were you concerned at all about getting measurements from various points throughout the well?**
| A33: For the spectrophotometer probe we used adapters to align the probe (documentation!) with either the top or bottom LED at the Z-height of the culture plate. Design files for these adapters are located in CAD Files>3D Printed Parts>auxiliary parts.

| **Q34: I'm having trouble finding the compiled firmwawre (firmware.elf) file to program the microcontroller. Where can I find it?**
| A34: The file is located in "Firmware>Compiled".

| **Q35: When using the LED socket aligner or when installing the LED spacer, it is difficult to fit the sockets into the socket holes. Do you have any good suggestions for that?**
| A35: Yes. With your existing socket aligner or LED spacer, try (1) using a pair of plyers to straighten the walls between socket holes and/or (2) use an implement such as a small flathead screwdriver to etch out any plastic debris or holes that look particularly small (3D printing is not that precise). If that fails, you may need to compensate the design files to give you the right size socket holes. However, you want the fit to be snug. The socket holes will also get looser after a few uses/installations. Also see supplementary Videos 1 and 2.

| **Q36:  The parts table for LPA v1.0 lists a 47k ohm resistor (stated as a 50 kohm, QTY 5), but it is not listed on the LPA_BOM or LPA_Component. Is this component used?**
| A36: Those resistors were originally intended to go on the top layer of the board (empty pads in Supplementary Figure S1a) but they turn out to not be necessary. Whether or not they are soldered, they won't affect the circuit.
