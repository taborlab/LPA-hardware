Folder contents
===============

Main files
----------
- lpa.pro : KiCAD project
- lpa.sch : Main schematic diagram
- tlcs_1-3.sch: LED driver schematic diagram
- lpa.kicad_pcb : PCB layout diagram

Libraries
---------
- libraries : folder with schematic diagram part libraries
- LPA_parts.pretty: folder with PCB layout diagram part libraries
- fp-lib-table: file listing the PCB layout diagram part libraries used by this project
- lpa-cache.lib: Part library cache, from which parts can be loaded if not found in the standard libraries.

How to use
==========
Electronic CAD files were created on KiCAD, which can be downloaded from http://kicad-pcb.org/.
To see the CAD files, open KiCAD, go to File -> Open Project, and select "lpa.pro". KiCAD will show a list of files on its left panel. The most important are lpa.sch (schematic diagram) and lpa.kicad_pcb (printed circuit board (PCB) diagram). Double click on each to open.

About gerber files
==================
Gerber files for production were generated according to these instructions: http://docs.oshpark.com/design-tools/kicad/generating-kicad-gerbers/.