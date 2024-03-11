/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>


//Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

enum fonts{SMALL, MEDIUM, LARGE};

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );


		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor, int typeFont);


		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move(LTimer movement_timer);

		//Shows the dot on the screen
		void render(LTexture &texture);
		int mPosX, mPosY;
		int mVelX, mVelY;

    private:
		//The X and Y offsets of the dot


		//The velocity of the dot

};
Dot dot;
Dot dot2;
Dot bullet;
Dot enemybullet;
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

Mix_Chunk *gLaser = NULL;
Mix_Chunk *gExplosion = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gBackground;
LTexture gBulletTexture;
LTexture gWinnerBackground;
LTexture gDot2Texture;
LTexture gDotDotTexture;

LTexture gTextTemp;
LTexture gTextWinnerTexture;
LTexture gStartText;

const int WALKING_ANIMATION_FRAMES = 37;
SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];
LTexture gSpriteSheetTexture;

TTF_Font *gFontSmall = NULL;
TTF_Font *gFontMedium = NULL;
TTF_Font *gFontLarge = NULL;



LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor, int typeFont)
{
	//Get rid of preexisting texture
	free();
    SDL_Surface* textSurface;
    if(typeFont==SMALL)
    {
        textSurface = TTF_RenderText_Solid( gFontSmall, textureText.c_str(), textColor );
    }
    else if(typeFont== MEDIUM)
    {
        textSurface = TTF_RenderText_Solid( gFontMedium, textureText.c_str(), textColor );
    }
    else if(typeFont==LARGE)
    {
        textSurface = TTF_RenderText_Solid( gFontLarge, textureText.c_str(), textColor );
    }
	//Render text surface
	//SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}


void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
 mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

 //Clear tick variables
 mStartTicks = 0;
 mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
  mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
 //The actual timer time
 Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
 //Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
 //Timer is running and paused
    return mPaused && mStarted;
}


Dot::Dot()
{
    //Initialize the offsets
    mPosX = 0;
    mPosY = 0;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            /*case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;*/
         case SDLK_SPACE:
            Mix_PlayChannel( -1, gLaser, 0);
            bullet.mPosX=dot.mPosX;
            bullet.mPosY=dot.mPosY;
            bullet.mVelY=-6;

            break;

            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            /*case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;*/
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move(LTimer movement_timer)
{
    //Move the dot left or right
    mPosX += int((float)mVelX * (float)movement_timer.getTicks() *0.15);

    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + 70 > SCREEN_WIDTH ) )
    {
        //Move back
        mPosX -= int((float)mVelX * (float)movement_timer.getTicks() *0.15);
    }

    //Move the dot up or down
    mPosY += int((float)mVelY * (float)movement_timer.getTicks() *0.15);

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back
        mPosY -= int((float)mVelY * (float)movement_timer.getTicks()*0.15 );
    }

}

void Dot::render(LTexture& texture)
{
    //Show the dot
	texture.render( mPosX, mPosY );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf("Sdl mixer: %s\n", Mix_GetError() );
                    success = false;
                }
                if ( TTF_Init() == -1 )
                {
                    printf("Sdl ttf: %s\n", TTF_GetError() );
                    success = false;
                }
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	int brojac=0;

	//Load dot texture
	if( !gSpriteSheetTexture.loadFromFile( "explosion.png" ) )
    {
        printf( "Failed to load walking animation texture!\n" );
        success = false;
    }
    else
    {
        int brojac =0;
        for(int i = 0;i<1024;i+=128)
        {
            for(int j=0;j<1024;j+=128)
            {
                gSpriteClips[brojac].x=j;
                gSpriteClips[brojac].y=i;
                gSpriteClips[brojac].h=128;
                gSpriteClips[brojac].w=128;
                if(brojac==WALKING_ANIMATION_FRAMES-1)
                {
                    break;
                }
                brojac++;
            }
            if(brojac==WALKING_ANIMATION_FRAMES-1)
            {
                break;
            }
        }

    }
	if( !gDotDotTexture.loadFromFile( "dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	if( !gDotTexture.loadFromFile( "final2Ship.png" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
    if( !gDot2Texture.loadFromFile( "spiked_ship_3_small_blue.png" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	if( !gBulletTexture.loadFromFile("laser.png") )
    {
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	if( !gBackground.loadFromFile( "space.jpg" ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }
    if( !gWinnerBackground.loadFromFile( "winner.jpg" ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }
    gLaser = Mix_LoadWAV( "laser_sound.wav" );
    if (gLaser == NULL)
    {
        printf("gLaser: %s\n", Mix_GetError() );
    }
    gExplosion = Mix_LoadWAV( "medium-explosion.wav" );
    if (gExplosion == NULL)
    {
        printf("gExplosion: %s\n", Mix_GetError() );
    }
    gFontSmall = TTF_OpenFont( "UQ_0.ttf", 30);
    gFontMedium = TTF_OpenFont( "UQ_0.ttf", 90);
    gFontLarge = TTF_OpenFont( "UQ_0.ttf", 150);
    if( gFontSmall == NULL || gFontMedium == NULL || gFontLarge == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 0, 0, 120 };
    }
    return success;
}

void close()
{
	//Free loaded images
	gDotDotTexture.free();
	gDotTexture.free();
    gBackground.free();
    gBulletTexture.free();
    gDot2Texture.free();
    gWinnerBackground.free();

    Mix_FreeChunk( gLaser );
    gLaser = NULL;
    Mix_FreeChunk( gExplosion );
    gExplosion = NULL;

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	TTF_CloseFont( gFontSmall );
    TTF_CloseFont( gFontMedium );
    TTF_CloseFont( gFontLarge );
    gFontSmall = NULL;
    gFontMedium = NULL;
    gFontLarge = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
	Mix_Quit();
}
bool checkCollision( Dot dot, Dot dot2 )
{

    //Calculate the sides of rect A
    int top1 = dot.mPosY;
    int bottom1 = dot.mPosY + 75;
    int left1 = dot.mPosX;
    int right1 = dot.mPosX + 120;


    int top2 = dot2.mPosY ;
    int bottom2 = dot2.mPosY;
    int left2 = dot2.mPosX ;
    int right2 = dot2.mPosX;

    //If any of the sides from A are outside of B
    if( bottom1 <= top2 )
    {
        return false;
    }

    if( top1 >= bottom2 )
    {
        return false;
    }

    if( right1 <= left2 )
    {
        return false;
    }

    if( left1 >= right2 )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

int main( int argc, char* args[] )
{
    int flagleft=false;
    int flagexplosion=false;
    int flagright=true;
    int flagmove=false;
    int flagstartgame=false;
    int flagendgame= false;
    int flagenemybullet=false;
    int flagsound=true;
    int dot2_temp;
    int dot2_temp2;
    bullet.mPosX=1600;
    enemybullet.mPosX=2400;
    //enemybullet.mPosY=200;
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			SDL_Event e_keydown;

            //Current animation frame
            int frame = 0;
			//The dot that will be moving around on the screen
			/*Dot dot;
			Dot dot2;
			Dot bullet;*/

            LTimer start_timer1;
            LTimer start_timer2;
            LTimer timer_velocity;
            LTimer timer_enemybullet;
            LTimer frametimer;
            LTimer explosion_timer;

            SDL_Color textColor = {255, 255, 255};

			//While application is running
			while( !quit )
			{
			    e_keydown=e;
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent( e );
				}
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
				gBackground.render( 0, 0 );

                enemybullet.render(gBulletTexture);
				bullet.render(gBulletTexture);
                dot.render(gDotTexture);
                dot2.render(gDot2Texture);




				//Move the dot
                if(flagright == true)
                {
                    dot2.mVelX=4;
                    if (dot2.mPosX >= SCREEN_WIDTH - 130)
                    {
                        flagleft = true;
                        flagright = false;
                    }

                }
                if (flagleft == true)
                {
                    dot2.mVelX=-4;
                    if (dot2.mPosX -10 <= 0)
                    {
                        flagright = true;
                        flagleft = false;
                    }
                }
                if(bullet.mPosY-bullet.DOT_HEIGHT<=0)
                {
                    bullet.mPosX = 1500;
                }
                if(flagmove==true)
                {
                    dot.move(timer_velocity);
                    dot2.move(timer_velocity);
                    bullet.move(timer_velocity);
                    enemybullet.move(timer_velocity);
                }

                if(flagstartgame==false)
                {
                    dot2.mPosX=390;
                    dot2.mPosY=40;
                    dot.mPosX=SCREEN_WIDTH/2 -75;
                    dot.mPosY=SCREEN_HEIGHT-120;
                    dot2.mPosX=390;


                    enemybullet.render(gBulletTexture);
                    bullet.render(gBulletTexture);
                    dot.render(gDotTexture);
                    dot2.render(gDot2Texture);

                    flagmove=false;
                    gStartText.loadFromRenderedText("Press any key to start", textColor, MEDIUM);
                    gStartText.render(SCREEN_WIDTH-900, 300);
                    if(e_keydown.type == SDL_KEYDOWN)
                    {
                        flagstartgame=true;
                    }
                }
                if (timer_enemybullet.getTicks() >= 2000)
                {
                    flagenemybullet = true;
                    timer_enemybullet.start();
                }
                if (flagenemybullet == true)
                {
                    if(flagsound==true)
                    {
                        Mix_PlayChannel(-1, gLaser, 0);
                    }
                    enemybullet.mPosX = dot2.mPosX;
                    enemybullet.mPosY = dot2.mPosY;
                    enemybullet.mVelX = (dot.mPosX - dot2.mPosX) * 0.01;
                    enemybullet.mVelY = (dot.mPosY - dot2.mPosY) * 0.01;
                    flagenemybullet = false;

                }
                if(enemybullet.mPosY >= SCREEN_HEIGHT - enemybullet.DOT_HEIGHT-20)
                {
                    enemybullet.mPosY=2500;
                }
                if(flagstartgame==true)
                {


                    if (start_timer1.isStarted() == false)
                    {
                         start_timer1.start();
                    }
                    if(start_timer1.getTicks()>=1000 && start_timer1.getTicks() < 2000)
                    {
                        gStartText.loadFromRenderedText("3", textColor, LARGE);
                        gStartText.render(SCREEN_WIDTH/2-20, 300);

                    }
                    if(start_timer1.getTicks()>=2000 && start_timer1.getTicks() < 3000)
                    {
                        gStartText.loadFromRenderedText("2", textColor, LARGE);
                        gStartText.render(SCREEN_WIDTH/2-20, 300);
                    }
                    if(start_timer1.getTicks()>=3000 && start_timer1.getTicks() < 4000)
                    {
                        gStartText.loadFromRenderedText("1", textColor, LARGE);
                        gStartText.render(SCREEN_WIDTH/2-20, 300);
                    }
                    if(start_timer1.getTicks()>=4000)
                    {
                        if (timer_enemybullet.isStarted() == false)
                        {
                            timer_enemybullet.start();
                        }
                        flagmove=true;
                    }

                }

                if(checkCollision(dot2, bullet)==true)
                {
                    dot2_temp = dot2.mPosX;
                    dot2_temp2 = dot2.mPosY;
                    dot2.mPosX = 4500;
                    dot2.mPosY = 4500;
                    bullet.mPosX=5000;
                    bullet.mPosY=3000;

                    flagmove=false;
                    flagexplosion = true;
                    dot2.move(timer_velocity);
                    dot2.render(gDot2Texture);
                    SDL_Rect* currentClip = &gSpriteClips[ frame ];
                    gSpriteSheetTexture.render( dot2.mPosX, dot2.mPosY, currentClip );
                    if(frametimer.isStarted()==false)
                    {
                        frametimer.start();
                    }
                    if(frametimer.getTicks() >=50)
                    {
                        ++frame;
                        frametimer.start();
                    }
                    if(frame==WALKING_ANIMATION_FRAMES-1)
                    {
                        flagendgame=true;
                        frame=0;
                    }
                }
                if(checkCollision(dot, enemybullet)==true)
                {
                    dot2_temp = dot.mPosX;
                    dot2_temp2 = dot.mPosY;
                    dot.mPosX = 4700;
                    dot.mPosY = 4500;
                    enemybullet.mPosX=5080;
                    enemybullet.mPosY=3000;
                    flagmove=false;
                    flagexplosion = true;
                    SDL_Rect* currentClip = &gSpriteClips[ frame ];
                    gSpriteSheetTexture.render( dot.mPosX , dot.mPosY, currentClip );
                    if(frametimer.isStarted()==false)
                    {
                        frametimer.start();
                    }
                    if(frametimer.getTicks() >=50)
                    {
                        ++frame;
                        frametimer.start();
                    }
                    if(frame==WALKING_ANIMATION_FRAMES-1)
                    {
                        flagendgame=true;
                        frame=0;
                    }

                }
                if(flagexplosion == true)
                {
                    if(flagsound==true)
                    {
                        Mix_PlayChannel( -1, gExplosion, 0 );
                    }

                    if (explosion_timer.isStarted() == false)
                    {
                        explosion_timer.start();
                    }
                    if (explosion_timer.getTicks() < 3000)
                    {
                        explosion_timer.stop();

                        SDL_Rect* currentClip = &gSpriteClips[ frame ];
                        gSpriteSheetTexture.render( dot2_temp, dot2_temp2, currentClip );
                        if (frametimer.isStarted() == false)
                        {
                            frametimer.start();
                        }
                        if (frametimer.getTicks() % 30 < 10)
                        {
                            ++frame;
                        }
                        if(frame==WALKING_ANIMATION_FRAMES-1)
                        {
                            flagendgame=true;
                            flagexplosion = false;
                            frame=0;
                        }
                    }
                }
                if(flagendgame==true)
                {
                        flagsound=false;
                        gWinnerBackground.render(0,0);
                        gTextTemp.loadFromRenderedText("Play again: y/ n", textColor, MEDIUM);
                        gTextTemp.render(SCREEN_WIDTH-600, 400);
                        /*dot.mPosX=3000;
                        dot.mPosY=3000;*/
                        if( e.key.keysym.sym == SDLK_y )
                        {
                            flagmove = true;
                            flagendgame = false;
                            flagexplosion = false;
                            flagstartgame = true;
                            flagsound=true;
                            dot2.mPosX=390;
                            dot2.mPosY=40;
                            dot.mPosX=SCREEN_WIDTH/2 -75;
                            dot.mPosY=SCREEN_HEIGHT-120;
                            dot.move(timer_velocity);
                            dot2.move(timer_velocity);
                            bullet.move(timer_velocity);
                            enemybullet.move(timer_velocity);
                            enemybullet.render(gBulletTexture);
                            bullet.render(gBulletTexture);
                            dot.render(gDotTexture);
                            dot2.render(gDot2Texture);
                        }
                        if(e.key.keysym.sym == SDLK_n)
                        {
                            quit=true;
                        }
                }

				//Go to next frame
                timer_velocity.start();




				SDL_RenderPresent( gRenderer );
			}
		}
    }
	//Free resources and close SDL
	close();
	return 0;
}
