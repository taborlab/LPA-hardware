# LPA - The Light Plate Apparatus

### Introduction

The LPA is a device capable of illuminating each well of a 24-well plate with two independently time-varying light signals. An electronic board based on the Atmega328 microcontroller reads light intensity values from an SD card and controls the LEDs responsible for illumination.

This repository contains source files, needed to modify the LPA's design. To obtain the minimum necessary files for building and using an LPA, click [here](https://github.com/taborlab/LPA-hardware/releases/download/v1.1/lpa.zip).

If you use the LPA in your research, please cite [Gerhardt K.P., et al. "An open-hardware platform for optogenetics and photobiology." Scientific Reports 6 (2016)](https://www.nature.com/articles/srep35363) and/or [Gerhardt K.P., Castillo-Hair S.M., and Tabor J.T. "DIY optogenetics: Building, programming, and using the Light Plate Apparatus." Methods Enzymol. (2019)](https://www.sciencedirect.com/science/article/pii/S0076687919301338)


### Documentation

The documentation website for the LPA can be found in http://taborlab.github.io/LPA-hardware/.

### Contents:
* BOM: Bill of materials for circuit board and chassis.
* CAD Files: Design files for the different parts of the LPA. This folder contains:
  * 3D Printed Parts: Models of the structural 3D-printed elements of the LPA, in SolidWorks format.
  * Circuit Board: Schematics and layout of the electronic board, in KiCAD format.
  * Gaskets: Diagrams of the rubber gaskets, in DXF (AutoCAD) format.
* Firmware: Source files of the microcontroller's firmware in C++, organized as an Atmel Studio project.
* Calibration: Image analysis script for imager-based LED calibration, in matlab format, and examples.
