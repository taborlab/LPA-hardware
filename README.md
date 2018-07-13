# LPA - The Light Plate Apparatus

### Introduction

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

### FAQ:
[Where can I find a 1:1 scale image of the gaskets or their dimensions?](######Q:-Where-can-I-find-a-1:1-scale-image-of-the-gaskets-or-their dimensions?)  
[The circuit board component parts list calls for a quantity of 0.15 breakaway headers per LPA. What does that mean?](######Q:-The-circuit-board-component-parts-list-calls-for-a-quantity-of-0.15-breakaway-headers-per-LPA.-What-does-that-mean?)

###### Q: Where can I find a 1:1 scale image of the gaskets or their dimensions?  
A: Answer

###### Q: The [circuit board component parts list](http://taborlab.github.io/LPA-hardware/) calls for a quantity of 0.15 breakaway headers per LPA. What does that mean?  
A: The breakaway headers are supplied as strips, 40 headers long, which you must manually break off to the desired length. The LPA requires two rows of three headers, for a total of six (0.15 x 40) headers.  

###### Q: Where I can find the PCB stencil file, and is there a corresponding .pdf, or .doc (or something similar) that I could open on my PC?
A: Answer
