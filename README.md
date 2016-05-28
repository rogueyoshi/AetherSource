# LuaSource
Lua-based source filter for DirectShow-compatible applications. Mainly for use with XSplit, FFSplit and Open Broadcaster Software.

Compiles under Microsoft Visual Studio 2015.

# Usage
Install the DirectX End-user Runtime @ https://www.microsoft.com/en-us/download/details.aspx?id=35&nowin10
Open a command prompt with Administrator rights.
CD to "LuaSource/bin".
Run "regsvr32 LuaSource_Win32_Debug.dll" to register the DLL (and use the -U argument to unregister between modifications).
Open your favorite streaming or chat app and add the LuaSource device.

# Compiling
Follow the steps at https://github.com/Microsoft/DirectXTK/wiki/Adding-the-DirectX-Tool-Kit-for-Audio#xaudio-27 to install the XAudio before compiling.
