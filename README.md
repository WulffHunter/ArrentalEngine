# ArrentalEngine
An SDL2.0 2D wrapper/game engine written in pure C.

Adds simple functionality such as:


    -AE_Sprites: Images that can be color-modulated, alpha-modulated, and played as an animation (includes animation looping functionality)
  
    -AE_LinkedTextures: A safe wrapper for texture sheets that prevents all objects referencing a texture sheet from losing the original texture if one object attempts to destroy it
  
    -Wrapper functions: Functions that handle simple yet tedious SDL and game engine functions.
  
  
Please note that Arrental Engine still requires a significant understanding of SDL2.0 and will not entirely replace its functionality, merely provide a safer and less time-consuming way to use it.
