#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "color.h"
#include "imageloader.h"
#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;

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


int main() {

  SDL_Init(SDL_INIT_VIDEO);
  ImageLoader::init();

  window = SDL_CreateWindow("DOOM", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("+", "assets/wall3.png");
  ImageLoader::loadImage("-", "assets/wall1.png");
  ImageLoader::loadImage("|", "assets/wall2.png");
  ImageLoader::loadImage("*", "assets/wall4.png");
  ImageLoader::loadImage("g", "assets/wall5.png");

  Raycaster r = { renderer };
  r.load_map("assets/map.txt");

  bool running = true;
  int speed = 10;

  int frameCount = 0;
  Uint32 startTime = SDL_GetTicks();
  Uint32 currentTime = startTime;

  while(running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym ){
          case SDLK_LEFT:
            r.player.a += 3.14/24;
            break;
          case SDLK_RIGHT:
            r.player.a -= 3.14/24;
            break;
          case SDLK_UP:
            r.player.x += speed * cos(r.player.a);
            r.player.y += speed * sin(r.player.a);
            break;
          case SDLK_DOWN:
            r.player.x -= speed * cos(r.player.a);
            r.player.y -= speed * sin(r.player.a);
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

    r.render();

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
