#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL2/SDL_ttf.h>  // Include the SDL_ttf header
#include <SDL2/SDL_mixer.h>

#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;  // Declare the font variable

void clear() {
  SDL_SetRenderDrawColor(renderer, 56, 56, 56, 255);
  SDL_RenderClear(renderer);
}

void draw_floor() {
    // floor color
    SDL_SetRenderDrawColor(renderer, 112, 122, 122, 255);
    SDL_Rect rect = {
            0,  // Coordenada x modificada para comenzar desde el lado izquierdo
            SCREEN_HEIGHT / 2,
            SCREEN_WIDTH,
            SCREEN_HEIGHT / 2
    };
    SDL_RenderFillRect(renderer, &rect);
}

// Variables globales para almacenar la posición anterior del mouse
int previousMouseX = 0;

// Función para manejar los eventos del mouse
Raycaster handleMouseEvents(SDL_Event event, Raycaster r) {
    if (event.type == SDL_MOUSEMOTION) {
        if (event.motion.state & SDL_BUTTON_LMASK) {
            // Calcular el cambio en la posición del mouse
            int deltaX = event.motion.x - previousMouseX;

            // Ajustar la rotación del jugador según el movimiento del mouse
            float rotationAmount = static_cast<float>(deltaX) * (3.14 / 24);
            r.player.a -= rotationAmount;

            // Actualizar la posición anterior del mouse
            previousMouseX = event.motion.x;
        }
    }
    return r;
}

// Function to display the welcome message
void displayWelcomeMessage() {
    clear();

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Welcome to Minecraft DOOM style!", textColor);
    SDL_Texture* welcomeMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = (SCREEN_WIDTH - surfaceMessage->w) / 2;
    messageRect.y = (SCREEN_HEIGHT - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, welcomeMessage, nullptr, &messageRect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(welcomeMessage);
}

void displayWinningMessage() {
    clear();

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "You reached the goal!", textColor);
    SDL_Texture* welcomeMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = (SCREEN_WIDTH - surfaceMessage->w) / 2;
    messageRect.y = (SCREEN_HEIGHT - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, welcomeMessage, nullptr, &messageRect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(welcomeMessage);
}

void displayLosingMessage() {
    clear();

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "You lost, but try again!", textColor);
    SDL_Texture* welcomeMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = (SCREEN_WIDTH - surfaceMessage->w) / 2;
    messageRect.y = (SCREEN_HEIGHT - surfaceMessage->h) / 2;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, welcomeMessage, nullptr, &messageRect);
    SDL_RenderPresent(renderer);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(welcomeMessage);
}

void drawUI(int value) {
    int size = 720;
    if (value == 0){
        ImageLoader::render(renderer, "p", SCREEN_WIDTH/2.0f - size/2.0f, SCREEN_HEIGHT - size, size);
    } else {
        ImageLoader::render(renderer, "pg", SCREEN_WIDTH/2.0f - size/2.0f, SCREEN_HEIGHT - size, size);
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();  // Initialize the SDL_ttf library
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);  // Initialize SDL_mixer

    window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load the font
    font = TTF_OpenFont("../assets/Arial Narrow.ttf", 24);  // Replace with the actual path to your font file and desired font size
    if (!font) {
        std::cout << "TTF_OpenFont error: " << TTF_GetError() << std::endl;
    }

    // Load the music file
    Mix_Music* music = Mix_LoadMUS("../assets/mine.mp3");
    if (!music) {
        std::cout << "Mix_LoadMUS error: " << Mix_GetError() << std::endl;
    }

    // Load the sound effect file
    Mix_Chunk* soundEffect = Mix_LoadWAV("../assets/minecraft-footsteps.wav");
    if (!soundEffect) {
        // Handle sound effect loading error
        // Print an error message or throw an exception
    }

    // Play the music in an infinite loop
    if (Mix_PlayMusic(music, -1) == -1) {
        // Handle music playing error
        // Print an error message or throw an exception
    }

    // Load the images
    ImageLoader::init();
    ImageLoader::loadImage("+", "assets/wall3.png");
    ImageLoader::loadImage("-", "assets/wall1.png");
    ImageLoader::loadImage("|", "assets/wall2.png");
    ImageLoader::loadImage("*", "assets/wall4.png");
    ImageLoader::loadImage("g", "assets/wall5.png");
    ImageLoader::loadImage("p", "assets/ui.png");
    ImageLoader::loadImage("pg", "assets/ui2.png");

    Raycaster r = { renderer };
    r.load_map("assets/map.txt");

    bool running = true;
    int speed = 10;

    int frameCount = 0;

    displayWelcomeMessage();

    // Mini event loop
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;
    SDL_Event event;
    while (SDL_GetTicks() - startTime < 2000) { // Run the loop for 3 seconds
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }
        }
    }

    int value = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        r.player.a += 3.14 / 24;
                        break;
                    case SDLK_RIGHT:
                        r.player.a -= 3.14 / 24;
                        break;
                    case SDLK_UP:
                        if (value == 0){
                            value = 1;
                        } else {
                            value = 0;
                        }
                        r.player.x += speed * cos(r.player.a);
                        r.player.y += speed * sin(r.player.a);
                        Mix_PlayChannel(-1, soundEffect, 0);
                        break;
                    case SDLK_DOWN:
                        if (value == 0){
                            value = 1;
                        } else {
                            value = 0;
                        }
                        r.player.x -= speed * cos(r.player.a);
                        r.player.y -= speed * sin(r.player.a);
                        Mix_PlayChannel(-1, soundEffect, 0);
                        break;
                    default:
                        break;
                }
            } else if (event.type == SDL_MOUSEMOTION) {
                r = handleMouseEvents(event, r);
            }
        }

        clear();
        draw_floor();

        int res = r.render();

        drawUI(value);

        if (res == 0) {
            displayWinningMessage();
            SDL_Delay(2000);
            running = false;
        } else if (res == 1) {
            displayLosingMessage();
            SDL_Delay(2000);
            running = false;
        }

        // render
        SDL_RenderPresent(renderer);

        frameCount++;
        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "Doom - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
