# Accelerometer Integration
Trial on getting velocities and positions from an accelerometer with absolute frame of reference, drift and noise correction.

# Project map
* integrator.py -> Gathers acceleration and quaternion data, rotates acceleration according to absolute reference coordinates, 
removes gravity offset and tries to filter noise/drift using low/high-pass filters
* spectral_analysis.m -> Tool designed to visualize acceleration/velocity spectrums both on time and frequency domain
* plot_acc/fft.m -> Auxiliar functions to spectral_analysis.m script
* stationary.csv -> IMU stationary with gravity mainly on z-axis. Evaluates integrator rejection to drift.
* y_acc.csv -> Acceleration applied on the y-axis of the accelerometer, minimal displacement on x-axis, no displacement on z.

# Status
Project was not able to produce a satisfactory result on y_acc.csv scenario. Cause is probably hard to remove noise 
on the accelerometer, imprecision and latency on quaternion data.

It would be better to have scenarios with IMU on low vibration and movement limited to only one axis, such as measuring
displacement of a wagon on rails.