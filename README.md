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

# Changelog
* Unified everything into a *single* header
* some functions had written dualshock as 'Dual**s**hock' where others were 'Dual**S**hock'
changed all functions to be the latter
* Changed names of structures so that they don't use reserved identifiers
