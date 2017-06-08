# LPA - The Light Plate Apparatus

The LPA is a device capable of illuminating each well of a 24-well plate with two independently time-varying light signals. An electronic board based on the Atmega328 microcontroller reads light intensity values from an SD card and controls the LEDs responsible for illumination.

This repository contains source files, needed to modify the LPA's design. To obtain the minimum necessary files for building and using an LPA, click [here](https://github.com/taborlab/LPA-hardware/releases/download/v1.1/lpa.zip).

If you use the LPA in your research, please cite [Gerhardt, et al. "An open-hardware platform for optogenetics and photobiology." Scientific Reports 6 (2016)](https://www.nature.com/articles/srep35363).

### Contents:
* CAD Files: Design files for the different parts of the LPA. This folder contains:
  * 3D Printed Parts: Models of the structural 3D-printed elements of the LPA, in SolidWorks format.
  * Circuit Board: Schematics and layout of the electronic board, in KiCAD format.
  * Gaskets: Diagrams of the rubber gaskets, in DXF (AutoCAD) format.
* Firmware: Source files of the microcontroller's firmware in C++, organized as an Atmel Studio project.
* Image analysis script: Scripts for imager-based LED calibration, in matlab format, and examples.
