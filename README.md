# PumaTX

# Introduction:
This is a try to build my own FPV controller based on ESP32 (RC03).
I want to make an alternative to OpenTX (it won't be as versatile of course) based on a DJI Mavic controller GL200A.

Which is supposed to be cheaper than conventional controllers such as a Taranis (less than 50€ is my goal but actual controller is more expensive so I'll have to try 3D print it)

# Release Candidates:

RC04: 
	Software:
		SBus
		Hall Effect
		Pots reading
		Arm reading
		Pre reading
		Power on / off via sport switch
		basic screen use + vbat + splash screen
		
	Hardware:
		Micro USB (from controller)
		2S LiPo
		external RF (R9M)
		
RC05:
	Software:
		Interupts for buttons (so code can run faster)
		MavLink (only rc, no telemetry) & choice between SBus and MavLink when compiling
		SoftPower
		LED for power
		
	Hardware:
		1S battery + USB charger + boost converter
		MavLink module
		
RC06:
	Software:
		Menu Navigation
		
	Hardware:
		Turn on / off stuff (transistor & MOSFET)
		Fan
		Haptic
		
RC07:
	Software:
		Bluetooth HID
		
RC08:

RC09:

RC10:
	Hardware:
		Homemade PCB
		
		

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
