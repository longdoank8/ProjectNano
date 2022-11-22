# Terahertz Module

## Table of Contents

1. [Overview](#s1)
1. [Parameters](#s2)
1. [Integration](#s3)
1. [References](#s4)


<div id='s1'/>
## 1. Overview
This module is an external addition to ns-3 [1] for simulating signals with terahertz frequency. Specifically this module works in conjuction with the BloodVoyagerSimulator (BVS) [2] inside ns-3 to simulate nanobots flowing through the human cardiovascular system and simulate signals from in-body to an outside gateway on the human skin.


<div id='s2'/>
## 2. Parameters <a name="s2"></a>
For this project we have predetermined (through testing or research) several key parameters:
- Signal frequency: **10GHz**
- Skin thickness: **0.1mm** [3]
- Fat thickness: **1.25mm**
- Blood diameter: **2.7mm** (specifically the Radial Artery) [4]
- Depth of Radial Artery: **2.7mm** [5]

Location of the gateways:
- Right hand: **(-30, 42.73, 0)**
- Left hand: **(20, 42.73, 0)**

Additonal constants regarding the properties of the human body for the computation of the Path Loss are taken from [6] and [7].

We have also predetermines a **Cone of Communication** (CoC) which limited the communication range to make sure the path loss of the signal does not surpass **300dB**.
With our predefined parameters for frequency and blood vessel properties and preliminary tests we determined the highest possible distance to gateway from the bottom of the vessel to be **6.915mm**. This results in the start and end of transmission is **~5.605mm** to left and right of the gateway, respectively.


<div id='s3'/>
## 3. Integration <a name="s3"></a>
The insertion point of our Terahertz module happens inside the function ```TranslatePosition()``` of the  ```Bloodvessel```class of BVS with every nanobot which happens to be in the Blood Vessel IDs of the right (27) and left (33) Hand.
Now that a nanobot is in range of one of the gateways and since BVS only provides us with singular snapshots of the positions every second, we are assuming that every nanobot has pass through the CoC and at the highest possible distance at the very bottom of the blood vessel.
From there we have predefined an entry point of the CoC from where we are starting the transmission and send data according to our frequency, velocity of nanobot, and overall distance to the gateway.
The signal is being simulating using our own [modulation transceiver](../BPSK_Transceiver_C_code/modulation.cc).


<div id='s4'/>
## 4. References <a name="s5"></a>

[1] https://www.nsnam.org/

[2] https://github.com/RegineWendt/blood-voyager-s

[3] https://onlinelibrary.wiley.com/doi/epdf/10.1111/j.1365-2133.1973.tb03007.x
The thickness of the epidermis
JUDI T. WHITTON,J.D. EVERALL
First published: November 1973 https://doi.org/10.1111/j.1365-2133.1973.tb03007.x

[4] https://pubmed.ncbi.nlm.nih.gov/21381609/

[5] https://www.researchgate.net/figure/Measurement-of-the-depth-from-skin-of-radial-artery-on-the-frozen-image-in-ultrasound_fig4_305223684#:~:text=The%20average%20depth%20of%20the,%5B11%5D.%20...

[6] https://www.researchgate.net/publication/354269238_In-body_Bionanosensor_Localization_for_Anomaly_Detection_via_Inertial_Positioning_and_THz_Backscattering_Communication
Simonjan, Jennifer & Unluturk, Bige & Akyildiz, Ian. (2021). In-body Bionanosensor Localization for Anomaly Detection via Inertial Positioning and THz Backscattering Communication. 

[7] https://www.researchgate.net/publication/308919450_Terahertz_electromagnetic_field_propagation_in_human_tissues_A_study_on_communication_capabilities
Piro, Giuseppe & Bia, Pietro & Boggia, Gennaro & Caratelli, Diego & Grieco, Luigi & Mescia, Luciano. (2016). Terahertz electromagnetic field propagation in human tissues: A study on communication capabilities. Nano Communication Networks. 10. 10.1016/j.nancom.2016.07.010. 