# PumaTX

THIS README IS NOT UP TO DATE

# Introduction:
This is a try to build my own FPV remote controller based on ESP32 (nRF52 later on) and DJI GL200A for its compactness and format.


# Release Candidates:

RC04: 
	Software:
		PPM
		SBus
		Hall Effect
		Pots reading (Left pot mode switch left = down ; Right = up ; RTH = overwrite)
		Arm reading
		Pre reading
		RTH reading
		Power on / off via sport switch
		basic screen use + vbat + splash screen
		
	Hardware:
		Micro USB (from controller)
		2S LiPo
		external RF (R9M)
		USB 2.0 (or 3.0 if I find one,) to plug in module (gnd, 5v, VBat, Tx)
		Support for JR module in phone holder
		
RC05:
	Software:
		Interupts for buttons (so code can run faster)
		MavLink (only rc, no telemetry) 
§		MavLink Bluetooth Passthrough (Uses to much memory, will have to sort it out for another release)
§		Choice between SBus and MavLink when compiling
		SoftPower use only interrupt
§		LED for power
		
	Hardware:
§		1S battery + USB charger + boost converter
§		MavLink module
		
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
	Software:
		MavLink telemetry screen

RC09:
	Software:
		CVBS Screen
		
	Hardware:
		USB 3.0 (5v, gnd, tx, rx, vid, 1, 2, VBat)
	
RC10:
	Hardware:
		Homemade PCB
		C1, C2
		Haptic
		
Other ideas:
	SPort
	FrSky X for bind / Rx num
	Sound output
	External bluetooth joystick
	Fix the calibration issue
	Remove screen loop
	eepron save
	
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
