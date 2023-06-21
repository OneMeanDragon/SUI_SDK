# SUI_SDK
SDK to my standard user interface.

## Interface.SUI & its Example
  - is mainly what this repo is for.

## GfxInterface.SUI & its Example (these are here just to be here)
  - Currently does nothing but create the SDL2 window
  - ESC || X: will close the SDL2 interface.
  - ..

``Actual SDL2 bug: If you kick the shit out of your pc apparently SDL2 will just freeze your application (SDL_PollEvent stops working causing the freeze).
If this ever happens try restarting your pc before reporting a window frozen bug.``

### About the menu (Interface.SUI only)
  - Menu items including the submenu names currently must have a unique name.
