# DS4WindowsApi
this is a fork of Tom Appelman's 
Unofficial Dualshock 4 api for windows
(inspired by the xinput api made by microsoft)

I want to make it clear that I am not the original author of this library
that honor goes to Tom Appelman[^1] all the changes I have made are shown in the changelog in this file 
[^1]: https://github.com/TomAppelman

# Working features:
- Compatible with Windows 10 and works even with the anniversary update;
- Fully working Dualshock 4 api for intergration without external software;
- Can be used as seperate DLL or as Static library;
- Be able to connection Wireless by the use of a bluetooth connection;
- Be able top changing the led light color;
- Having a special color per controller index;
- Touchpad positions for 2 points;
- Vibration functionality;
- Support for both the Dualshock 4 models(CUH-ZTC1 & CUH-ZTC2);
- Includes source for a xinput api wrapper(not fully functional in all games);

# Usage
the easiest way to use this is through the global `DualShock4Init` function as seen in the example,
this will create a window in the background that keeps track of the different controllers.

if however you want to keep it in the same thread as the window then you can use the alternative interface.
this interface is the same as is used in the backend, this can be seen in the `SampleAppAlternative.cpp` example

# Changelog
* Unified everything into a *single* header
* some functions had written dualshock as 'Dual**s**hock' where others were 'Dual**S**hock'
changed all functions to be the latter
* Changed names of structures so that they don't use reserved identifiers
* Fixed a minor bug in Dualshock4.hpp
* Added a DualShockDisconnectAll function
* Made window-proc callbacks public
* Added example for alternative interface
