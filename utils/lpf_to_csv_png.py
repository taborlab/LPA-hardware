#!/usr/bin/env python
"""
Decodes an lpf into a csv and optionally makes plots for each step
"""

import matplotlib.pyplot as pyplot
import matplotlib.image as mpimg
import numpy as numpy
import csv

'''
 Case-specific parameters 
 Edit parameters below as required
'''

# Filename, don't include the lpf
filename = "program"

# Max steps to process, None for all
max_steps = None

# Parameters for plotting
width = 12
height = 8
n_colors = 2
colors = [(1,0,0), (0,1,0)]
color_names = ['red', 'green']

'''
 LPF processing
 Don't edit below this line
'''
MAX_INTENSITY = 4095.0

# Open file
f = open(filename + ".lpf", 'rb')

# Read header
header = map(ord, f.read(32))

# Total number of channels
number_channels_b = header[4:8]
number_channels = number_channels_b[0] + (number_channels_b[1]<<8) + (number_channels_b[2]<<16) + (number_channels_b[3]<<24)
if number_channels != width*height*n_colors:
    raise ValueError("Incorrect number of channels. Expected: %d, Read: %d"%(width*height*n_colors, number_channels))
    
# Step size
step_size_b = header[8:12]
step_size = step_size_b[0] + (step_size_b[1]<<8) + (step_size_b[2]<<16) + (step_size_b[3]<<24)

# Number of steps
number_steps_b = header[12:16]
number_steps = number_steps_b[0] + (number_steps_b[1]<<8) + (number_steps_b[2]<<16) + (number_steps_b[3]<<24)

# Print file details:
print "File %s.lpf"%(filename)
print "Total number of channels: %d"%(number_channels)
print "Step duration (s): %3.2f"%(step_size/1000.)
print "Number of steps: %d"%(number_steps)
print "Total program duration (s): %3.2f"%(step_size/1000.*number_steps)
print ""
    
# Saturate number of steps
if max_steps and max_steps < number_steps:
    number_steps = max_steps
    
# Generate steps
for n_step in range(number_steps):
    print 'Processing step %04d...'%(n_step)
    # Read data
    step_raw = map(ord, f.read(number_channels*2))
    # Get 16-bit intensity values
    step_l = step_raw[0::2]
    step_u = step_raw[1::2]
    step = [step_l_i + (step_u_i*256) for (step_l_i, step_u_i) in zip(step_l, step_u)]
    
    # Write to csv
    for color_i in range(n_colors):
        step_color = numpy.array(step[color_i::n_colors])
        step_color.shape = (height,width)
        with open('%s_%05d_%s.csv'%(filename, n_step, color_names[color_i]), 'wb') as csv_file:
            writer = csv.writer(csv_file, delimiter = ',')
            for line in step_color:
                writer.writerow(line)
    
    # Write image files
    frame = numpy.zeros([height, width, 3])
    for color_i in range(n_colors):
        color = numpy.array(colors[color_i])
        step_color = numpy.array(step[color_i::n_colors])/MAX_INTENSITY
        step_color.shape = (height,width)
        for subcolor_i in range(3):
            frame[:,:,subcolor_i] += step_color*color[subcolor_i]
    
    pyplot.imshow(frame, interpolation='none')
    pyplot.savefig('%s_frame_%05d.png'%(filename, n_step))
    pyplot.close()
    

# Close file
f.close()