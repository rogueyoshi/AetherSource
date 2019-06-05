# AetherSource
Programmable script-based source filter for DirectShow-compatible applications. Mainly for use with **XSplit, FFSplit and Open Broadcaster Software** or anything else that supports webcams. Currently supports **Lua** but will have support for more scripting languages soon.

This plug-in predates the advent of Browser-Source in OBS, but is the same concept, only applied to scripting languages instead of HTML5.

It also serves as an example of **how to create a Virtual Web Cam / Capture Device** (this part of the code is very lightweight and easily portable).

Compiles under Microsoft Visual Studio 2015.

# Usage
To use, you must first register the dll in windows...

CD to "AetherSource/bin".
Run "regsvr32 AetherSource_Win32_Debug.dll" with Administrator privileges to register the DLL (and use the -u argument to unregister between modifications).

Then, open your favorite streaming or chat app and add the AetherSource device.

(I don't currently work on this project anymore, but if people ask, I will create an installer.)

# Examples
Live weather/music widgets, input display, stream interaction, alerts, etc.
