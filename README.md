# SteamCMD
This application automate install & run steam games. 

## How to run
- Install SteamCMD program: https://developer.valvesoftware.com/wiki/SteamCMD to C:\SteamCMD
- Find appID of the needed game: http://api.steampowered.com/ISteamApps/GetAppList/v0002/
- Add 'steam_login' & 'steam_pass' system variables
- Edit parameters in the Main.cpp (appID)
- Edit config file (game name)

## TODO
- move hardcoded params from code to program arguments
- add Steam API support to clear editting path to the game
