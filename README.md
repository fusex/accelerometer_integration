# Accelerometer Integration
Trial on getting velocities and positions from an accelerometer with absolute frame of reference, drift and noise correction.

# Project map
* integrator.py -> Gathers acceleration and quaternion data, rotates acceleration according to absolute reference coordinates, 
removes gravity offset and tries to filter noise/drift using low/high-pass filters
* math_analysis/spectral_analysis.m -> Tool designed to visualize acceleration/velocity spectrums both on time and frequency domain
* math_analysis/plot_*.m -> Auxiliar functions to spectral_analysis.m script
* raw_data/stationary.csv -> IMU stationary with gravity mainly on z-axis. Evaluates integrator rejection to drift.
* raw_data/y_acc.csv -> Acceleration applied on the y-axis of the accelerometer, minimal displacement on x-axis, no displacement on z.
* get_sensor_data/src/* -> code intended to work with an AltIMU-10 v5 connected to a RaspberryPi. Only the cross compilation is working at the moment.  

# Status
Project was not able to produce a satisfactory result on y_acc.csv scenario. Cause is probably hard to remove noise 
on the accelerometer, imprecision and latency on quaternion data.

It would be better to have scenarios with IMU on low vibration and movement limited to only one axis, such as measuring
displacement of a wagon on rails.