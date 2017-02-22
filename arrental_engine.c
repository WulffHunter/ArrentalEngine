//
//  arrental_engine.c
//  arrental_engine
//
//  Created by Jared Rand on 2017-01-12.
//  Copyright © 2017 zapix_games. All rights reserved.
//

#include "arrental_engine.h"
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <tgmath.h>

//
//
//Start: SDL Initialization and Closing Functions
//
//

/**
 Initializes SDL

 @param windowTitle The title of the window that will appear as a char string
 @param screenWidth The width of the window
 @param screenHeight The height of the window
 @param vsync_enabled A boolean for whether you want vsync to be enabled or not
 @return Struct containing a window, a renderer, and a boolean showing whether initialization was successful or not
 */
AE_WindowBundle* AE_Initialize(char* windowTitle, int screenWidth, int screenHeight, bool vsync_enabled)
{
    AE_WindowBundle* output = malloc(sizeof(AE_WindowBundle));
    output->initSuccess = true;
    
    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        //SDL could not be initialized, print a warning and change the initSuccess of output to false
        printf("SDL could not be initialized. Error: %s\n", SDL_GetError());
        output->initSuccess = false;
    }
    else
    {
        //Linear texture filtering
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Linear texture filtering could not be enabled! Error: %s\n", SDL_GetError());
        }
        
        //Create the output's window
        output->window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        
        if (output->window == NULL)
        {
            printf("Window could not be initialized! Error: %s\n", SDL_GetError());
            output->initSuccess = false;
        }
        else
        {
            //Create window which is vsynced or not depending on the vsync option
            if (vsync_enabled)
            {
                output->renderer = SDL_CreateRenderer(output->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            }
            else
            {
                output->renderer = SDL_CreateRenderer(output->window, -1, SDL_RENDERER_ACCELERATED);
            }
            
            if (output->renderer == NULL)
            {
                printf("Renderer could not be created! Error: %s\n", SDL_GetError());
                output->initSuccess = false;
            }
            else
            {
                //Set renderer draw color
                SDL_SetRenderDrawColor(output->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
            
            //Initialize SDL_image loading
            int initImg = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
            if (!(IMG_Init(initImg) & initImg))
            {
                printf("SDL_image could not be initialized! Error: %s\n", IMG_GetError());
                output->initSuccess = false;
            }
            
            //Initialize SDL_ttf
            if (TTF_Init() == -1)
            {
                printf("SDL_ttf could not be initialized! Error: %s\n", TTF_GetError());
                output->initSuccess = false;
            }
        }
    }
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    
    return output;
}

/**
 Closes SDL

 @param closeWindowBundle The WindowBundle to be closed
 */
void AE_Close(AE_WindowBundle* closeWindowBundle)
{
    //Close the window and renderer
    SDL_DestroyRenderer(closeWindowBundle->renderer);
    closeWindowBundle->renderer = NULL;
    
    SDL_DestroyWindow(closeWindowBundle->window);
    closeWindowBundle->window = NULL;
    
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

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
Uint32 AE_GetSurfacePixel(SDL_Surface* surface, int x, int y)
{
    return ((Uint32*) surface->pixels)[y*(surface->pitch/sizeof(Uint32))+x];
}

/**
 Loads a texture from a file

 @param renderer The renderer that will convert the SDL_Surface to an SDL_Texture
 @param path The pathname of the file to be opened
 @return The loaded SDL_Texture
 */
SDL_Texture* AE_LoadTextureFromFile(SDL_Renderer* renderer, char* path)
{   
    //Load a surface from the given path
    SDL_Surface* loaded = IMG_Load(path);
    SDL_Texture* output = NULL;
    
    if (loaded == NULL)
    {
        printf("Cannot load image at %s! Error: %s\n", path, IMG_GetError());
    }
    else
    {
        //Color key the image based on the bottom-left pixel
        if (SDL_SetColorKey(loaded, SDL_TRUE, SDL_MapRGB(loaded->format, 0, 255, 255)) < 0)
        {
            printf("Failed to color key. Error: %s\n", IMG_GetError());
        }
        //Create a texture from the surface
        output = SDL_CreateTextureFromSurface(renderer, loaded);
        if (output == NULL)
        {
            printf("Surface could not be converted to texture from image %s. Error: %s\n", path, IMG_GetError());
        }
        //Destroy original surface
        SDL_FreeSurface(loaded);
    }
    return output;
}

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
SDL_Texture* AE_LoadTextureFromText(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color textColor)
{
    //Render the text onto a surface
    SDL_Surface* loaded = TTF_RenderText_Solid(font, text, textColor);
    
    //Create an output texture
    SDL_Texture* output = NULL;
    //If nothing was loaded
    if (loaded == NULL)
    {
        printf("Cannot load texture from text! Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create a texture from the loaded surface
        output = SDL_CreateTextureFromSurface(renderer, loaded);
        //If no texture was created
        if (output == NULL)
        {
            printf("Texture could not be created from text. Error: %s\n", SDL_GetError());
        }
    }
    //Free the original surface
    SDL_FreeSurface(loaded);
    return output;
}

/**
 A wrapper function to get the width of an SDL_Texture

 @param texture The SDL_Texture whose width we would like to get
 @return The width of the SDL_Texture
 */
int AE_TextureGetWidth(SDL_Texture* texture)
{
    //Create the output variable
    int output;
    
    //Fill the output variable with the necessary data
    SDL_QueryTexture(texture, NULL, NULL, &output, NULL);
    
    //Return the output variable
    return output;
}

/**
 A wrapper function to get the height of an SDL_Texture
 
 @param texture The SDL_Texture whose height we would like to get
 @return The height of the SDL_Texture
 */
int AE_TextureGetHeight(SDL_Texture* texture)
{
    //Create the output variable
    int output;
    
    //Fill the output variable with the necessary data
    SDL_QueryTexture(texture, NULL, NULL, NULL, &output);
    
    //Return the output variable
    return output;
}

/**
 A wrapper function to get the format of an SDL_Texture
 
 @param texture The SDL_Texture whose format we would like to get
 @return The format of the SDL_Texture
 */
Uint32 AE_TextureGetFormat(SDL_Texture* texture)
{
    //Create the output variable
    Uint32 output;
    
    //Fill the output variable with the necessary data
    SDL_QueryTexture(texture, &output, NULL, NULL, NULL);
    
    //Return the output variable
    return output;
}

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

/**
 Creates a new linkedTexture from an SDL_Texture with an empty list of referencing objects

 @param texture The SDL_Texture to create the linkedTexture from
 @return The linkedTexture with the SDL_Texture as its texture and an empty list of referencing objects
 */
AE_LinkedTexture* AE_CreateLinkedTexture(SDL_Texture* texture)
{
    AE_LinkedTexture* output = malloc(sizeof(AE_LinkedTexture));
    output->texture = texture;
    output->linkedList = NULL;
    output->references = 0;
    return output;
}

/**
 Adds an object to the list of objects referencing a linkedTexture

 @param linkedTexture The linkedTexture that the object would like to reference
 @param stakeholder_object The object that will reference the linkedTexture
 @return Whether the object joined the list of objects referencing the linkedTexture or not
 */
bool AE_LinkedTexture_Join(AE_LinkedTexture* linkedTexture, void* stakeholder_object)
{
    //Whether creating a new link to the linkedTexture was successful or not
    bool success = false;
    
    //If there are no objects referencing a linkedTexture yet
    if (linkedTexture->linkedList == NULL)
    {
        //Make the root reference into the current stakeholder object
        if ((linkedTexture->linkedList = malloc(sizeof(AE_SheetLink))))
        {
            success = true;
        }
        linkedTexture->linkedList->reference = stakeholder_object;
        linkedTexture->linkedList->next = NULL;
        
        //Increment the number of references
        linkedTexture->references++;
    }
    else
    {
        AE_SheetLink* tempSheetLink = linkedTexture->linkedList;
        
        //Skip through the list of objects until you reach the end
        while (tempSheetLink->next != NULL)
        {
            tempSheetLink = tempSheetLink->next;
        }
        //Create the new reference at the end of the list
        if ((tempSheetLink->next = malloc(sizeof(AE_SheetLink))))
        {
            success = true;
        }
        tempSheetLink->next->reference = stakeholder_object;
        tempSheetLink->next->next = NULL;
        
        //Increment the number of references
        linkedTexture->references++;
    }
    
    return success;
}

/**
 Removes an object from the list of objects that reference a linkedTexture

 @param linkedTexture The linked texture that the object is removing itself from
 @param stakeholder_object The onject that will no longer reference the linkedTexture
 @return Whether the object was removed from the referencing list or not
 */
bool AE_LinkedTexture_Leave(AE_LinkedTexture* linkedTexture, void* stakeholder_object)
{
    if (linkedTexture->linkedList != NULL)
    {
        
        AE_SheetLink* tempSheetLink = linkedTexture->linkedList;
        AE_SheetLink* prevSheetLink = NULL;
        
        while (tempSheetLink != NULL)
        {
            if (tempSheetLink->reference == stakeholder_object)
            {
                if (prevSheetLink == NULL)
                {
                    linkedTexture->linkedList = tempSheetLink->next;
                }
                else
                {
                    prevSheetLink->next = tempSheetLink->next;
                }
                free(tempSheetLink);
                linkedTexture->references--;
                return true;
            }
            else
            {
                prevSheetLink = tempSheetLink;
                tempSheetLink = tempSheetLink->next;
            }
        }
    }
    
    return false;
}

/**
 Gets the number of objects referencing a linkedTexture

 @param linkedTexture The linkedTexture whose reference count will be retrieved
 @return The number of ojects referencing the linked texture
 */
Uint16 AE_LinkedTexture_GetReferenceCount(AE_LinkedTexture* linkedTexture)
{
    return linkedTexture->references;
}

/**
 Gets the width of the texture in a linkedTexture

 @param linkedTexture The texture whose width will be retrieved
 @return The width of the texture
 */
int AE_LinkedTexture_GetWidth(AE_LinkedTexture* linkedTexture)
{
    //Create the output variable
    int output;
    
    //Fill the output variable with the necessary data
    SDL_QueryTexture(linkedTexture->texture, NULL, NULL, &output, NULL);
    
    //Return the output variable
    return output;
}

/**
 Gets the height of the texture in a linkedTexture
 
 @param linkedTexture The texture whose height will be retrieved
 @return The height of the texture
 */
int AE_LinkedTexture_GetHeight(AE_LinkedTexture* linkedTexture)
{
    //Create the output variable
    int output;
    
    //Fill the output variable with the necessary data
    SDL_QueryTexture(linkedTexture->texture, NULL, NULL, NULL, &output);
    
    //Return the output variable
    return output;
}

/**
 Gets the format of the texture in a linkedTexture
 
 @param linkedTexture The texture whose format will be retrieved
 @return The format of the texture
 */
Uint32 AE_LinkedTexture_GetFormat(AE_LinkedTexture* linkedTexture)
{
    //Create the output variable
    Uint32 output;
    
    //Fill the output variable with the necessary data
    SDL_QueryTexture(linkedTexture->texture, &output, NULL, NULL, NULL);
    
    //Return the output variable
    return output;
}


/**
 Destroys a linkedTexture only if nothing references it

 @param linkedTexture The linkedTexture to destroy
 @return Whether the linkedTexture was destroyed or not
 */
bool AE_DestroyLinkedTexture(AE_LinkedTexture* linkedTexture)
{
    bool success = false;
    
    //If nothing is referencing the linkedTexture
    if (linkedTexture->linkedList == NULL && linkedTexture->references == 0)
    {
        //Destroy the texture completely
        SDL_DestroyTexture(linkedTexture->texture);
        free(linkedTexture);
        success = true;
    }
    return success;
}

/**
 Destroys a linkedTexture and the list of objects referencing it, regardless of whether objects are referencing it or not

 @param linkedTexture The linkedTexture to destroy
 */
void AE_DestroyLinkedTexture_Unsafe(AE_LinkedTexture* linkedTexture)
{
    AE_SheetLink* tempSheetLink;
    while (linkedTexture->references > 0)
    {
        tempSheetLink = linkedTexture->linkedList;
        while (tempSheetLink->next != NULL)
        {
            tempSheetLink = tempSheetLink->next;
        }
        free(tempSheetLink);
        linkedTexture->references--;
    }
    SDL_DestroyTexture(linkedTexture->texture);
    free(linkedTexture);
}


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
 Creates a new AE_Sprite from a preexisting SDL_Texture
 
 @param spriteSheet The preexisting SDL_Texture that will be used as the sprite sheet
 @param reference_x The x of the sprite on the spritesheet
 @param reference_y The y of the sprite on the spritesheet
 @param frameCount The number of frames in the sprite
 @param width The width of a single sprite frame
 @param height The height of a single sprite frame
 @param pivot_x The x of the sprite pivot point
 @param pivot_y The y of the sprite pivot point
 @return The initialized AE_Sprite, with all other parameters set to default
 */
AE_Sprite* AE_CreateSprite(AE_LinkedTexture* spriteSheet, int reference_x, int reference_y, int frameCount, int width, int height, int pivot_x, int pivot_y)
{
    AE_Sprite* output = malloc(sizeof(AE_Sprite));
    
    output->frames = NULL;
    output->width = width;
    output->height = height;
    
    AE_SpriteSetSpriteSheet(output, spriteSheet, frameCount, reference_x, reference_y, width, height, AE_SPRITE_KEEP_NONE);
    
    AE_SpriteSetPivot(output, pivot_x, pivot_y, AE_SPRITE_KEEP_NONE);
    
    AE_SpriteSetAngle(output, 0.0);
    AE_SpriteSetFlip(output, SDL_FLIP_NONE);
    
    output->drawRect.x = 0;
    output->drawRect.y = 0;
    AE_SpriteSetScale(output, 1, 1, AE_SPRITE_DEFAULT);
    
    //Default frame speed is 30
    AE_SpriteSetFrameSpeed(output, 30);
    
    AE_SpriteSetColor(output, NULL, AE_SPRITE_DEFAULT);
    output->alpha = 255;
    
    return output;
}

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
void AE_SpriteSetSpriteSheet(AE_Sprite* sprite, AE_LinkedTexture* spriteSheet, int frameCount, int reference_x, int reference_y, int frame_width, int frame_height, AE_Flag dataToKeep)
{
    sprite->spriteSheet = spriteSheet;
    AE_LinkedTexture_Join(spriteSheet, sprite);
    
    //Offload the work to the SetFrames function for modularity
    AE_SpriteSetFrames(sprite, frameCount, reference_x, reference_y, frame_width, frame_height, dataToKeep);
}


/**
 Sets the number of frames in a sprite, and creates an array of frames for the sprite

 @param sprite The sprite whose frame count will be expanded
 @param frameCount The number of sprite frames
 @param reference_x The x of the sprite on its spritesheet
 @param reference_y The y of the sprite on its spritesheet
 @param frame_width The width of the new frames
 @param frame_height The height of the new frames
 @param dataToKeep Whether you want to keep the width, height, or neither
 */
void AE_SpriteSetFrames(AE_Sprite* sprite, int frameCount, int reference_x, int reference_y, int frame_width, int frame_height, AE_Flag dataToKeep)
{
    //Set the sprite framecount
    sprite->frameCount = frameCount;
    
    //Initialize temp_width and temp_height variables
    int temp_width = 0;
    int temp_height = 0;
    
    //If you're not saving the width, set it to the new frame_width
    if (dataToKeep != AE_SPRITE_KEEP_WIDTH)
    {
        temp_width = sprite->width = frame_width;
    }
    else
    {
        temp_width = sprite->width;
    }
    
    //If you're not saving the height, set it to the new frame_height
    if (dataToKeep != AE_SPRITE_KEEP_HEIGHT)
    {
        temp_height = sprite->height = frame_height;
    }
    else
    {
        temp_height = sprite->height;
    }
    
    //If there is one frame and it's the same size as the spritesheet and there is a spritesheet
    if (frameCount == 1 && reference_x == 0 && reference_y == 0 && sprite->spriteSheet != NULL && temp_width == AE_LinkedTexture_GetWidth(sprite->spriteSheet) && temp_height == AE_LinkedTexture_GetHeight(sprite->spriteSheet))
    {
        //Free any leftover frame data and make the sprite point at NULL frames
        if (sprite->frames != NULL)
        {
            free(sprite->frames);
        }
        sprite->frames = NULL;
    }
    else
    //If there's any frames and it's not the same size as the spritesheet
    {
        //If there's a prexisting framesheet and it's not the same framecount as the new framecount, destroy the old framesheet
        if (sprite->frames != NULL && sprite->frameCount != frameCount)
        {
            //Free any leftover frame data and make the sprite point at NULL frames
            SDL_Rect* temp_frames = sprite->frames;
            sprite->frames = NULL;
            free(&temp_frames);
        }
        if (sprite->frames == NULL)
        {
            sprite->frames = malloc(sizeof(SDL_Rect)*frameCount);
            for (int i = 0; i<frameCount; i++)
            {
                sprite->frames[i].x = 0;
                sprite->frames[i].y = 0;
                sprite->frames[i].w = 0;
                sprite->frames[i].h = 0;
            }
        }
        //For each frame in the array, set the frame attributes up
        for (int i = 0; i<frameCount; i++)
        {
            //If the currently set frame is not frame 0, make the frame the endpoint + 1
            sprite->frames[i].x = (i == 0) ? reference_x : reference_x + ((temp_width * i) + 1);
            
            //The frames run along the width of the sprite sheet, so the y remains static
            sprite->frames[i].y = reference_y;
            
            //Set the frame width and height to be the sprite width and height
            sprite->frames[i].w = temp_width;
            sprite->frames[i].h = temp_height;
        }
    }
}

/**
 Sets the current pivot

 @param sprite The sprite whose pivot will be set
 @param pivot_x The x of the new pivot
 @param pivot_y The y of the new pivot
 @param dataToKeep Which pivot point attributes will be overwritten: x, y, both, or sets the pivot to the default position (centerpoint)
 */
void AE_SpriteSetPivot(AE_Sprite* sprite, int pivot_x, int pivot_y, AE_Flag dataToKeep)
{
    //If you're keeping the original pivot x, set the pivot y to the given y
    if (dataToKeep != AE_SPRITE_KEEP_X && dataToKeep != AE_SPRITE_DEFAULT)
    {
        sprite->pivot.x = pivot_x;
    }
    //If you're keeping the original pivot y, set the pivot x to the given x
    if (dataToKeep == AE_SPRITE_KEEP_Y && dataToKeep != AE_SPRITE_DEFAULT)
    {
        sprite->pivot.y = pivot_y;
    }
    //If you're setting the pivot point to the centerpoint, do that
    if (dataToKeep == AE_SPRITE_DEFAULT)
    {
        sprite->pivot.x = sprite->width/2;
        sprite->pivot.y = sprite->height/2;
    }
}

/**
 Sets the sprite angle

 @param sprite The sprite whose angle will be set
 @param angle The angle of the sprite
 */
void AE_SpriteSetAngle(AE_Sprite* sprite, double angle)
{
    sprite->angle = angle;
}

/**
 Sets the rendererflip of the sprite

 @param sprite The sprite whose rendererflip will be set
 @param flip The desired rendererflip type
 */
void AE_SpriteSetFlip(AE_Sprite* sprite, SDL_RendererFlip flip)
{
    sprite->flip = flip;
}

/**
 Sets the sprite scale

 @param sprite The sprite whose scale will be set
 @param x_scale The new scale on the x axis
 @param y_scale The new scale on the y axis
 @param dataToKeep Which scale attributes will be overwritten: x scale, y scale, both, or sets the both scales to the default size (1)
 */
void AE_SpriteSetScale(AE_Sprite* sprite, float x_scale, float y_scale, AE_Flag dataToKeep)
{
    //Set a drawRect pointer to the sprite's drawRect for less dereferencing
    SDL_Rect* drawRect = &sprite->drawRect;
    //If you're setting the scales to default, set both scales to 1
    if (dataToKeep == AE_SPRITE_DEFAULT)
    {
        sprite->x_scale = sprite->y_scale = 1;
        
        //Set the drawRect width and height to 1
        drawRect->w = sprite->width;
        drawRect->h = sprite->height;
    }
    else
    {
        //If you're not keeping it, set the x_scale
        if (dataToKeep != AE_SPRITE_KEEP_X)
        {
            sprite->x_scale = x_scale;
            
            //If the sprite has a preexisting drawRect to render to, adjust it
            if (drawRect != NULL)
            {
                drawRect->w =(int)(((float)sprite->width) * sprite->x_scale);
            }
        }
        //If you're not keeping it, set the y_scale
        if (dataToKeep != AE_SPRITE_KEEP_Y)
        {
            sprite->y_scale = y_scale;
            
            //If the sprite has a preexisting drawRect to render to, adjust it
            if (drawRect != NULL)
            {
                drawRect->h =(int)(((float)sprite->height) * sprite->y_scale);
            }
        }
    }
}

/**
 Gets the current loop frame of the sprite

 @param sprite The sprite whose frame will be retrieved
 @return The current loop frame
 */
int AE_SpriteGetSavedFrame(AE_Sprite* sprite)
{
    return sprite->currentFrame;
}

//
//Sets the current loop frame of the sprite
//return: void
//

/**
 Sets the current loop frame of the sprite

 @param sprite The sprite whose current loop frame will be set
 @param desiredFrame The frame the loop will be set to
 */
void AE_SpriteSetSavedFrame(AE_Sprite* sprite, int desiredFrame)
{
    //If the desired frame is bigger than the amount of frames in the sprite, set the current frame to the final frame
    if (desiredFrame >= sprite->frameCount)
    {
        sprite->currentFrame = sprite->frameCount - 1;
    }
    //If the desired frame is less than 0, set the sprite frame to 0
    else if (desiredFrame < 0)
    {
        sprite->currentFrame = 0;
    }
    else
    //If the frame is within the frame count, set the frame to the desired frame
    {
        sprite->currentFrame = desiredFrame;
    }
}

//
//Sets the frame speed of the sprite
//return: void
//

/**
 Sets the frame speed of the sprite

 @param sprite The sprite whose frame speed will be set
 @param frameSpeed The speed the sprite will be set to
 */
void AE_SpriteSetFrameSpeed(AE_Sprite* sprite, float frameSpeed)
{
    sprite->frameSpeed = frameSpeed;
}

/**
 Sets the color of the sprite

 @param sprite The AE_Sprite whose color will be set
 @param color The color the AE_Sprite will be set to
 @param dataToKeep Which preexisting color values to keep (red, green, blue) or set the color values to default color white
 */
void AE_SpriteSetColor(AE_Sprite* sprite, AE_ColorBundle* color, AE_Flag dataToKeep)
{
    if (dataToKeep != AE_SPRITE_KEEP_R && dataToKeep != AE_SPRITE_DEFAULT && color != NULL)
    {
        sprite->color.r = color->r;
    }
    if (dataToKeep != AE_SPRITE_KEEP_G && dataToKeep != AE_SPRITE_DEFAULT && color != NULL)
    {
        sprite->color.g = color->g;
    }
    if (dataToKeep != AE_SPRITE_KEEP_B && dataToKeep != AE_SPRITE_DEFAULT && color != NULL)
    {
        sprite->color.b = color->b;
    }
    if (dataToKeep == AE_SPRITE_DEFAULT)
    {
        sprite->color.r = 255;
        sprite->color.g = 255;
        sprite->color.b = 255;
    }
}

/**
 Gets the color modulation of a sprite

 @param sprite The AE_Sprite whose color will be retrieved
 @return A pointer to an AE_ColorBundle containing the colors of the AE_Sprite
 */
AE_ColorBundle* AE_SpriteGetColor(AE_Sprite* sprite)
{
    //Create a new AE_ColorBundle and fill with the given sprite's colors
    AE_ColorBundle* output = malloc(sizeof(AE_ColorBundle));
    output->r = sprite->color.r;
    output->g = sprite->color.g;
    output->b = sprite->color.b;
    return output;
}

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
bool AE_SpriteRender(AE_Sprite* sprite, SDL_Renderer* renderer, int x, int y, int currentFrame, float step)
{
    bool success = false;
    if (sprite->spriteSheet != NULL)
    {
        //If currentFrame is less than 0, loop through the spritesheet (sets the sprite's currentFrame along with it)
        if (currentFrame < 0)
        {
            if (sprite->currentFrame >= (float)(sprite->frameCount))
            {
                sprite->currentFrame = fmodf(sprite->currentFrame,(float)(sprite->frameCount - 1));
            }
            currentFrame = fmodf(sprite->currentFrame,(float)(sprite->frameCount - 1));
        }
        //If the currentFrame is larger than the number of frames the sprite has, go to the last frame of the animation, and reset the sprite's currentFrame for the next time it loops
        else if (currentFrame >= sprite->frameCount)
        {
            currentFrame = sprite->frameCount-1;
            sprite->currentFrame = 0;
        }
        //Reset the sprite's currentFrame for the next time it loops
        else
        {
            sprite->currentFrame = 0;
        }
        sprite->drawRect.x = x;
        sprite->drawRect.y = y;
        
        SDL_SetTextureColorMod(sprite->spriteSheet->texture, sprite->color.r, sprite->color.g, sprite->color.b);
        SDL_SetTextureAlphaMod(sprite->spriteSheet->texture, sprite->alpha);
        
        if ( SDL_RenderCopyEx(renderer, sprite->spriteSheet->texture, &sprite->frames[currentFrame], &sprite->drawRect, sprite->angle, &sprite->pivot, sprite->flip) )
        {
            success = true;
        }
        sprite->currentFrame+=((sprite->frameSpeed/2)*step);
    }
    return success;
}

/**
 Frees and destroys an AE_Sprite

 @param sprite The sprite to be destroyed
 */
void AE_DestroySprite(AE_Sprite* sprite)
{
    AE_LinkedTexture_Leave(sprite->spriteSheet, sprite);
    free(sprite->frames);
    free(sprite);
    sprite = NULL;
}

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

/**
 Creates a new AE_Timer

 @return A pointer to the new AE_Timer
 */
AE_Timer* AE_Create_Timer()
{
    AE_Timer* output = malloc(sizeof(AE_Timer));
    output->isStarted = false;
    output->isPaused = false;
    output->startTime = 0;
    output->pauseTime = 0;
    return output;
}

/**
 Starts (and unpauses) an AE_Timer

 @param timer The timer to start
 */
void AE_Timer_Start(AE_Timer* timer)
{
    timer->isStarted = true;
    timer->isPaused = false;
    timer->startTime = SDL_GetTicks();
    timer->pauseTime = 0;
}

/**
 Pauses an AE_Timer

 @param timer The AE_Timer to pause
 */
void AE_Timer_Pause(AE_Timer* timer)
{
    if (timer->isStarted && !timer->isPaused)
    {
        timer->isPaused = true;
        timer->pauseTime = SDL_GetTicks() - timer->startTime;
    }
}

/**
 Resumes a paused AE_Timer

 @param timer The AE_Timer to resume
 */
void AE_Timer_Resume(AE_Timer* timer)
{
    //If a timer is started and paused
    if (timer->isStarted && timer->isPaused)
    {
        //Unpause the timer
        timer->isPaused = false;
        //Set the start time to the saved time
        timer->startTime = SDL_GetTicks() - timer->pauseTime;
        timer->pauseTime = 0;
    }
}

/**
 Stops a paused AE_Timer
 
 @param timer The AE_Timer to stop
 */
void AE_Timer_Stop(AE_Timer* timer)
{
    timer->isStarted = false;
    timer->isPaused = false;
    timer->startTime = 0;
    timer->pauseTime = 0;
}

/**
 Gets the amount of time in an AE_Timer

 @param timer The AE_Timer to get the time from
 @return The current time on the AE_Timer
 */
Uint32 AE_Timer_GetTime(AE_Timer* timer)
{
    Uint32 output = 0;
    
    //If the timer's started
    if (timer->isStarted)
    {
        //If the timer is paused, return the saved time
        if (timer->isPaused)
        {
            output = timer->pauseTime;
        }
        else
        //return the regular time
        {
            output = SDL_GetTicks() - timer->startTime;
        }
    }
    
    return output;
}

/**
 Returns whether an AE_Timer is paused or not

 @param timer The AE_Timer to be determined whether it is paused or not
 @return Whether the AE_Timer is paused or not
 */
bool AE_Timer_IsPaused(AE_Timer* timer)
{
    return timer->isStarted && timer->isPaused;
}

/**
 Returns whether an AE_Timer is started

 @param timer The AE_Timer to determine whether it is started or not
 @return Whether the AE_Timer is started or not
 */
bool AE_Timer_IsStarted(AE_Timer* timer)
{
    return timer->isStarted;
}

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
Uint8 AE_BlendColorChannel(Uint8 col1, Uint8 col2, Uint8 percentage)
{
    //Return color 1 mixed with a percentage of color 2
    return (Uint8)(col1 + ( (col2 - col1) * ((float)percentage) / 100 ) );
}

/**
 Gets a random number between two numbers

 @param min The minimun number the output can be
 @param max The maximum number the output can be
 @return The random number
 */
int AE_Random(int min, int max)
{
    //If the min and max are the same, return that: else, return the random number
    return (min != max) ? ((int )(floor(rand() % (max - min)) + min)) : max;
}

/**
 Gets a random seed number

 @return The random seed number
 */
Uint64 AE_RandomSeed()
{
    //Create a random Uint32 seed
    Uint64 seed = rand();
    //Bitshift the original by 32 and repeat
    seed = (seed<<32) | rand();
    
    return seed;
}

/**
 Gets a pseudorandom number between two numbers based on a given seed. Used for procedural generation: if given the same input numbers, the output will always be the same

 @param seed The seed number to be manipulated
 @param x The x upon which the seed number will be manipulated
 @param y The y upon which the seed number will be manipulated
 @param min The min number the output can be
 @param max The max number the output can be
 @return The pseudorandom number based on the seed, x, and y
 */
int AE_PseudoRandomFromSeed(Uint64 seed, int x, int y, int min, int max)
{
    //If the min and max are the same, return that: else, return the random number
    return (min != max) ? ((int )(floor((seed % x % y) % (max - min)) + min)) : max;
}

/**
 Returns the distance between two points as an int

 @param x1 The x of the first point
 @param y1 The y of the first point
 @param x2 The x of the second point
 @param y2 The y of the second point
 @return The distance between two points as an int
 */
int AE_PointDistance(int x1, int y1, int x2, int y2)
{
    return (int) sqrt((double)(pow((x1-x2), 2) + pow((y1 - y2), 2)));
}

/**
 Returns a float rounded to floor or ceil depending on if it's less than 0

 @param input The original float
 @return The float, rounded to the nearest floor or ceiling
 */
float AE_FloatBase(float input)
{
    float output = 0;
    
    if (input < 0)
    {
        output = floorf(input);
    }
    else
    {
        output = ceilf(input);
    }
    return output;
}

//
//
//End: General Functions