# BPSK Modulation

## Table of contents

1. [Overview](#s1)
1. [Parameters](#s2)
1. [Transceiver](#s3)
1. [References](#s4)
    
<div id='s1'/>
### 1.Overview
This module was important to establish a communication between gateway and nanobots.
In order to send signals or messages the Binary-Phase-Shift-Keying modulation was used. 
The BPSK modulation was implemented in a transmitter and a receiver part.

<div id='s2'/>
### 2.Parameters
For the transmitter following parameters are neccesary:
- random generated bit sequence (for simulation purposes)
- number of bits per signals
- number of data points per cosine waves
- number of cosine/sine waves per bit
- carrier frequency (in which the message gets send with)

For the receiver side the send following parameters are neccesary:
- received signal from transmitter
- additional vector for convolution

<div id='s3'/>
### 3. Transceiver
The transmitter consists of the transmitter, responsible for modulating the signals and sending it between gateway and nanobots, and the receiver, responsible for demodulating the received signal. 


<div id='s4'/>
### References
[1] https://stackoverflow.com/questions/24518989/how-to-perform-1-dimensional-valid-convolution
[2] Viswanathan, M. (2019), Digital Modulations using Python, independently published