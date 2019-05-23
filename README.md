# PumaTX

# Introduction:
This is a try to build my own FPV controller based on ESP32 (RC03).
I want to make an alternative to OpenTX (it won't be as versatile of course) based on a DJI Mavic controller GL200A.

Which is supposed to be cheaper (less than 50€ is my goal but actual controller is more expensive so I'll have to try 3D print it)

# Required specs:
-Read data from the hall effect MLX sensors from the controller

-Have 10 channels (can't do more cause of the limitation of the controller)

-Output SBus (to plug into R9M)

-Input SPort to read telemetry data (From R9M)

-Have a screen for minimal setup

-Have a Soft Power capabilty (using the power button from the controller)


This is the bare minimum. But I do have way more ideas coming.

# Folders:
In the "PumaTX_RC" folder you will find the different Release Candidate (Higher is better but could be unstable, read description for more details).

In the "Other" folder you will find different kind of "documentation" it is mostly here for my personnal use, as a reference.
