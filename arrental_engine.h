//
//  arrental_engine.h
//  arrental_engine
//
//  Created by Jared Rand on 2017-01-13.
//  Copyright © 2017 zapix_games. All rights reserved.
//

#ifndef arrental_engine_h
#define arrental_engine_h

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

/**
 A struct containing red, green, and blue colors
 */
typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
} AE_ColorBundle;

//
//
//Start: SDL Initialization and Closing Functions
//
//

/**
 A struct containing a window, a renderer, and whether or not SDL was intitialized
 */
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_bool initSuccess;
}AE_WindowBundle;

/**
 Initializes SDL
 
 @param windowTitle The title of the window that will appear as a char string
 @param screenWidth The width of the window
 @param screenHeight The height of the window
 @param vsync_enabled A boolean for whether you want vsync to be enabled or not
 @return Struct containing a window, a renderer, and a boolean showing whether initialization was successful or not
 */
AE_WindowBundle* AE_Initialize(char* windowTitle, int screenWidth, int screenHeight, SDL_bool vsync_enabled);

/**
 Destroys an AE_WindowBundle and closes SDL
 
 @param closeWindowBundle The WindowBundle to be closed
 */
void AE_Close(AE_WindowBundle* closeWindowBundle);

/**
 Destroys an AE_WindowBundle and sets the pointer to NULL
 
 @param closeWindowBundle The WindowBundle to be closed
 */
void AE_DestroyWindowBundle(AE_WindowBundle* closeWindowBundle);

/**
 Closes SDL
 */
void AE_CloseSDL();

//
//
//End: SDL Initialization and Closing Functions
//
//

//
//
//Start: SDL Texture and Surface Functions
//
//

/**
 Gets the color of a pixel on an SDL_Surface (note: non-functional)
 
 @param surface The SDL_Surface whose pixels will be checked
 @param x The x coordinate of the pixel to be checked
 @param y The y coordinate of the pixel to be checked
 @return The color of the pixel at the surface's given x and y
 */
Uint32 AE_GetSurfacePixel(SDL_Surface* surface, int x, int y);

/**
 Loads a texture from a file
 
 @param renderer The renderer that will convert the SDL_Surface to an SDL_Texture
 @param path The pathname of the file to be opened
 @return The loaded SDL_Texture
 */
SDL_Texture* AE_LoadTextureFromFile(SDL_Renderer* renderer, char* path);

//Loads a texture from given text
//return: the text texture
/**
 Loads an SDL_Texture from given text
 
 @param renderer The renderer that will convert the text to an SDL_Surface
 @param font The font the text will be rendered in
 @param text The text to be rendered
 @param textColor The color of the text to be rendered
 @return The SDL_Texture containing the rendered text
 */

SDL_Texture* AE_LoadTextureFromText(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color textColor);

/**
 A wrapper function to get the width of an SDL_Texture
 
 @param texture The SDL_Texture whose width we would like to get
 @return The width of the SDL_Texture
 */
int AE_TextureGetWidth(SDL_Texture* texture);

/**
 A wrapper function to get the height of an SDL_Texture
 
 @param texture The SDL_Texture whose height we would like to get
 @return The height of the SDL_Texture
 */
int AE_TextureGetHeight(SDL_Texture* texture);

/**
 A wrapper function to get the format of an SDL_Texture
 
 @param texture The SDL_Texture whose format we would like to get
 @return The format of the SDL_Texture
 */
Uint32 AE_TextureGetFormat(SDL_Texture* texture);

//
//
//End: SDL Texture and Surface Functions
//
//

//
//
//Start: Linked Textures and Associated Functions
//
//

typedef struct AE_SheetLink {
    void* reference;
    struct AE_SheetLink* next;
} AE_SheetLink;

typedef struct {
    SDL_Texture* texture;
    AE_SheetLink* linkedList;
    Uint16 references;
} AE_LinkedTexture;

/**
 Creates a new linkedTexture from an SDL_Texture with an empty list of referencing objects
 
 @param texture The SDL_Texture to create the linkedTexture from
 @return The linkedTexture with the SDL_Texture as its texture and an empty list of referencing objects
 */
AE_LinkedTexture* AE_CreateLinkedTexture(SDL_Texture* texture);

/**
 Adds an object to the list of objects referencing a linkedTexture
 
 @param linkedTexture The linkedTexture that the object would like to reference
 @param stakeholder_object The object that will reference the linkedTexture
 @return Whether the object joined the list of objects referencing the linkedTexture or not
 */
SDL_bool AE_LinkedTexture_Join(AE_LinkedTexture* linkedTexture, void* stakeholder_object);
/**
 Removes an object from the list of objects that reference a linkedTexture
 
 @param linkedTexture The linked texture that the object is removing itself from
 @param stakeholder_object The onject that will no longer reference the linkedTexture
 @return Whether the object was removed from the referencing list or not
 */
SDL_bool AE_LinkedTexture_Leave(AE_LinkedTexture* linkedTexture, void* stakeholder_object);

/**
 Gets the number of objects referencing a linkedTexture
 
 @param linkedTexture The linkedTexture whose reference count will be found
 @return The number of ojects referencing the linked texture
 */
Uint16 AE_LinkedTexture_GetReferenceCount(AE_LinkedTexture* linkedTexture);

/**
 Gets the width of the texture in a linkedTexture
 
 @param linkedTexture The texture whose width will be retrieved
 @return The width of the texture
 */
int AE_LinkedTexture_GetWidth(AE_LinkedTexture* linkedTexture);

/**
 Gets the height of the texture in a linkedTexture
 
 @param linkedTexture The texture whose height will be retrieved
 @return The height of the texture
 */
int AE_LinkedTexture_GetHeight(AE_LinkedTexture* linkedTexture);

/**
 Gets the format of the texture in a linkedTexture
 
 @param linkedTexture The texture whose format will be retrieved
 @return The format of the texture
 */
Uint32 AE_LinkedTexture_GetFormat(AE_LinkedTexture* linkedTexture);

/**
 Destroys a linkedTexture only if nothing references it
 
 @param linkedTexture The linkedTexture to destroy
 @return Whether the linkedTexture was destroyed or not
 */
SDL_bool AE_DestroyLinkedTexture(AE_LinkedTexture* linkedTexture);

/**
 Destroys a linkedTexture and the list of objects referencing it, regardless of whether objects are referencing it or not
 
 @param linkedTexture The linkedTexture to destroy
 */
void AE_DestroyLinkedTexture_Unsafe(AE_LinkedTexture* linkedTexture);

//
//
//End: Linked Textures and Associated Functions
//
//

//
//
//Start: Sprites and Sprite Functions
//
//

/**
 A sprite, complete with size and frames
 */
typedef struct {
    AE_LinkedTexture* spriteSheet;
    int width;
    int height;
    
    int frameCount;
    SDL_Rect* frames;
    float currentFrame;
    float frameSpeed;
    
    SDL_Point pivot;
    double angle;
    SDL_RendererFlip flip;
    
    float x_scale;
    float y_scale;
    SDL_Rect drawRect;
    
    AE_ColorBundle color;
    Uint8 alpha;
}AE_Sprite;

typedef enum {
    AE_SPRITE_KEEP_NONE,
    AE_SPRITE_DEFAULT,
    AE_SPRITE_KEEP_X,
    AE_SPRITE_KEEP_Y,
    AE_SPRITE_KEEP_WIDTH,
    AE_SPRITE_KEEP_HEIGHT,
    AE_SPRITE_KEEP_R,
    AE_SPRITE_KEEP_G,
    AE_SPRITE_KEEP_B
}AE_Flag;

/**
 Creates a new AE_Sprite from a preexisting LinkedTexture
 
 @param spriteSheet The preexisting LinkedTexture that will be used as the sprite sheet
 @param reference_x The x of the sprite on the spritesheet
 @param reference_y The y of the sprite on the spritesheet
 @param frameCount The number of frames in the sprite
 @param width The width of a single sprite frame
 @param height The height of a single sprite frame
 @param pivot_x The x of the sprite pivot point
 @param pivot_y The y of the sprite pivot point
 @return The initialized AE_Sprite, with all other parameters set to default
 */
AE_Sprite* AE_CreateSprite(AE_LinkedTexture* spriteSheet, int reference_x, int reference_y, int frameCount, int width, int height, int pivot_x, int pivot_y);

/**
 Guts out and fills a sprite with the given data
 
 @param spriteSheet The preexisting LinkedTexture that will be used as the sprite sheet
 @param reference_x The x of the sprite on the spritesheet
 @param reference_y The y of the sprite on the spritesheet
 @param frameCount The number of frames in the sprite
 @param width The width of a single sprite frame
 @param height The height of a single sprite frame
 @param pivot_x The x of the sprite pivot point
 @param pivot_y The y of the sprite pivot point
 */
void AE_FillSprite(AE_Sprite* sprite, AE_LinkedTexture* spriteSheet, int reference_x, int reference_y, int frameCount, int width, int height, int pivot_x, int pivot_y);

/**
 Resets the frame sprite sheet
 
 @param sprite The AE_Sprite whose spritesheet will be reset
 @param spriteSheet The spritesheet that the AE_Sprite will now reference
 @param frameCount The number of frames in the sprite
 @param reference_x The x of the desired sprite on the spritesheet
 @param reference_y The y of the desired sprite on the spritesheet
 @param frame_width The width of the new frames
 @param frame_height The height of the new frames
 @param dataToKeep Whether to keep the previous width or height, or neither
 */
void AE_SpriteSetSpriteSheet(AE_Sprite* sprite, AE_LinkedTexture* spriteSheet, int frameCount, int reference_x, int reference_y, int frame_width, int frame_height, AE_Flag dataToKeep);

/**
 Sets the number of frames in a sprite, and creates an array of frames for the sprite
 
 @param sprite The sprite whose frame count will be expanded
 @param frameCount The number of sprite frames
 @param reference_x The x of the sprite on its spritesheet
 @param reference_y The y of the sprite on its spritesheet
 */
void AE_SpriteSetFrames(AE_Sprite* sprite, int frameCount, int reference_x, int reference_y, int frame_width, int frame_height, AE_Flag dataToKeep);

/**
 Sets the current pivot
 
 @param sprite The sprite whose pivot will be set
 @param pivot_x The x of the new pivot
 @param pivot_y The y of the new pivot
 @param dataToKeep Which pivot point attributes will be overwritten: x, y, both, or sets the pivot to the default position (centerpoint)
 */
void AE_SpriteSetPivot(AE_Sprite* sprite, int pivot_x, int pivot_y, AE_Flag dataToKeep);

/**
 Sets the sprite angle
 
 @param sprite The sprite whose angle will be set
 @param angle The angle of the sprite
 */
void AE_SpriteSetAngle(AE_Sprite* sprite, double angle);

/**
 Sets the rendererflip of the sprite
 
 @param sprite The sprite whose rendererflip will be set
 @param flip The desired rendererflip type
 */
void AE_SpriteSetFlip(AE_Sprite* sprite, SDL_RendererFlip flip);

/**
 Sets the sprite scale
 
 @param sprite The sprite whose scale will be set
 @param x_scale The new scale on the x axis
 @param y_scale The new scale on the y axis
 @param dataToKeep Which scale attributes will be overwritten: x scale, y scale, both, or sets the both scales to the default size (1)
 */
void AE_SpriteSetScale(AE_Sprite* sprite, float x_scale, float y_scale, AE_Flag dataToKeep);

/**
 Gets the current loop frame of the sprite
 
 @param sprite The sprite whose frame will be retrieved
 @return The current loop frame
 */
int AE_SpriteGetSavedFrame(AE_Sprite* sprite);

/**
 Sets the current loop frame of the sprite
 
 @param sprite The sprite whose current loop frame will be set
 @param desiredFrame The frame the loop will be set to
 */
void AE_SpriteSetSavedFrame(AE_Sprite* sprite, int desiredFrame);

/**
 Sets the frame speed of the sprite
 
 @param sprite The sprite whose frame speed will be set
 @param frameSpeed The speed the sprite will be set to
 */
void AE_SpriteSetFrameSpeed(AE_Sprite* sprite, float frameSpeed);

/**
 Sets the color of the sprite
 
 @param sprite The AE_Sprite whose color will be set
 @param color The color the AE_Sprite will be set to
 @param dataToKeep Which preexisting color values to keep (red, green, blue) or set the color values to default color white
 */
void AE_SpriteSetColor(AE_Sprite* sprite, AE_ColorBundle* color, AE_Flag dataToKeep);

/**
 Gets the color modulation of a sprite
 
 @param sprite The AE_Sprite whose color will be retrieved
 @return A pointer to an AE_ColorBundle containing the colors of the AE_Sprite
 */
AE_ColorBundle* AE_SpriteGetColor(AE_Sprite* sprite);

/**
 Sets the color of the sprite
 
 @param sprite The AE_Sprite whose alpha will be set
 @param alpha The alpha the AE_Sprite will be set to
 */
void AE_SpriteSetAlpha(AE_Sprite* sprite, Uint8 alpha);

/**
 Gets the alpha modulation of a sprite
 
 @param sprite The AE_Sprite whose alpha will be retrieved
 @return The alpha modulation of the sprite
 */
Uint8 AE_SpriteGetAlpha(AE_Sprite* sprite);


/**
 Renders the sprite on the given renderer
 
 @param sprite The sprite to be rendered
 @param renderer The renderer the sprite will be rendered on
 @param x The x coordinate the sprite will be drawn at
 @param y The y coordinate the sprite will be drawn at
 @param currentFrame The current frame the sprite will be drawn on (-1 or less for an animation loop)
 @param step The timestep the sprite will be drawn at
 @return Whether the sprite was successfully rendered or not
 */
SDL_bool AE_SpriteRender(AE_Sprite* sprite, SDL_Renderer* renderer, int x, int y, int currentFrame, float step);

/**
 Frees and destroys an AE_Sprite
 
 @param sprite The sprite to be destroyed
 */
void AE_DestroySprite(AE_Sprite* sprite);

//
//
//End: Sprites and Sprite Functions
//
//

//
//
//Start: Timer functions
//
//

typedef struct {
    Uint32 startTime;
    Uint32 pauseTime;
    SDL_bool isPaused;
    SDL_bool isStarted;
}AE_Timer;

/**
 Creates a new AE_Timer
 
 @return A pointer to the new AE_Timer
 */
AE_Timer* AE_Create_Timer();

/**
 Starts (and unpauses) an AE_Timer
 
 @param timer The timer to start
 */
void AE_Timer_Start(AE_Timer* timer);

/**
 Pauses an AE_Timer
 
 @param timer The AE_Timer to pause
 */
void AE_Timer_Pause(AE_Timer* timer);

/**
 Resumes a paused AE_Timer
 
 @param timer The AE_Timer to resume
 */
void AE_Timer_Resume(AE_Timer* timer);

/**
 Stops a paused AE_Timer
 
 @param timer The AE_Timer to stop
 */
void AE_Timer_Stop(AE_Timer* timer);

/**
 Gets the amount of time in an AE_Timer
 
 @param timer The AE_Timer to get the time from
 @return The current time on the AE_Timer
 */
Uint32 AE_Timer_GetTime(AE_Timer* timer);

/**
 Returns whether an AE_Timer is paused or not
 
 @param timer The AE_Timer to be determined whether it is paused or not
 @return Whether the AE_Timer is paused or not
 */
SDL_bool AE_Timer_IsPaused(AE_Timer* timer);

/**
 Returns whether an AE_Timer is started
 
 @param timer The AE_Timer to determine whether it is started or not
 @return Whether the AE_Timer is started or not
 */
SDL_bool AE_Timer_IsStarted(AE_Timer* timer);

//
//
//End: Timer Functions
//
//

//
//
//Start: General Functions
//
//

/**
 Blends two Uint8's together, based on the strength of the second Uint8 that should be used. To be used to combine individual R, G, or B channels
 
 @param col1 The base color
 @param col2 The color to be blended into the base color
 @param percentage The percentage of the blend (50 is halfway between the colors, 0 is the base color, 100 is the blended color)
 @return The blended color
 */
Uint8 AE_BlendColorChannel(Uint8 col1, Uint8 col2, Uint8 percentage);

/**
 Gets a random number between two numbers
 
 @param min The minimun number the output can be
 @param max The maximum number the output can be
 @return The random number
 */
int AE_Random(int min, int max);

/**
 Gets a random seed number
 
 @return The random seed number
 */
Uint64 AE_RandomSeed();

/**
 Creates a pseudorandom Uint64 from 4 Uint64. Used for creating random numbers from seeds.
 
 @param seednum_1 A manipulated Uint64
 @param seednum_2 A manipulated Uint64
 @param seednum_3 A manipulated Uint64
 @param seednum_4 A manipulated Uint64
 @return A Uint64 created by shifting and manipulating the 4 given Uint64's
 */
Uint64 AE_CreateFinalSeed(Uint64 seednum_1, Uint64 seednum_2, Uint64 seednum_3, Uint64 seednum_4);

/**
 Gets a pseudorandom number between two numbers based on a given seed. Used for procedural generation: if given the same input numbers, the output will always be the same
 
 @param seed The seed number to be manipulated
 @param x The x upon which the seed number will be manipulated
 @param y The y upon which the seed number will be manipulated
 @param set A value that manipulates the outcome so that multiple different numbers can be generated for the same x and y
 @param min The min number the output can be
 @param max The max number the output can be
 @return The pseudorandom number based on the seed, x, and y
 */
int AE_PseudoRandomFromSeed_Int(Uint64 seed, int x, int y, Uint64 set, int min, int max);

/**
 Gets a pseudorandom number between two numbers based on a given seed. Used for procedural generation: if given the same input numbers, the output will always be the same
 
 @param seed The seed number to be manipulated
 @param x The x upon which the seed number will be manipulated
 @param y The y upon which the seed number will be manipulated
 @param set A value that manipulates the outcome so that multiple different numbers can be generated for the same x and y
 @param min The min number the output can be
 @param max The max number the output can be
 @return The pseudorandom number based on the seed, x, and y
 */
int AE_PseudoRandomFromSeed_Uint64(Uint64 seed, Uint64 x, Uint64 y, Uint64 set, int min, int max);

/**
 Returns the distance between two points as an int
 
 @param x1 The x of the first point
 @param y1 The y of the first point
 @param x2 The x of the second point
 @param y2 The y of the second point
 @return The distance between two points as an int
 */
int AE_PointDistance(int x1, int y1, int x2, int y2);

/**
 Returns a float rounded to floor or ceil depending on if it's less than 0
 
 @param input The original float
 @return The float, rounded to the nearest floor or ceiling
 */
float AE_FloatBase(float input);

/**
 Returns the horizontal length of a line of a given length on a given angle
 
 @param length The length of the line whose horizontal component will be found
 @param direction The angle of the line whose horizontal component will be found
 @return The precise length of the horizontal component of the given line
 */
long double AE_Lengthdir_X(int length, float direction);

/**
 Returns the vertical length of a line of a given length on a given angle
 
 @param length The length of the line whose vertical component will be found
 @param direction The angle of the line whose vertical component will be found
 @return The precise length of the vertical component of the given line
 */
long double AE_Lengthdir_Y(int length, float direction);

//
//
//End: General Functions
//
//

#endif /* arrental_engine_h */
