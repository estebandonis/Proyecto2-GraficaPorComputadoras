#pragma once

#include <print.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <string>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <unordered_map>
#include "color.h"
#include "imageloader.h"


const Color B = {0, 0, 0};
const Color W = {255, 255, 255};

const int WIDTH = 16;
const int HEIGHT = 11;
const int BLOCK = 50;
const int SCREEN_WIDTH = WIDTH * BLOCK;
const int SCREEN_HEIGHT = HEIGHT * BLOCK;


struct Player {
  int x;
  int y;
  float a;
  float fov;
}; 

struct Impact {
  float d;
  std::string mapHit;  // + | -
  int tx;
};

class Raycaster {
public:
  Raycaster(SDL_Renderer* renderer)
    : renderer(renderer) {

    player.x = BLOCK + BLOCK / 2;
    player.y = BLOCK + BLOCK / 2;

    player.a = M_PI / 4.0f;
    player.fov = M_PI / 3.0f;

    scale = 50;
    tsize = 128;
  }

  void load_map(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (getline(file, line)) {
      map.push_back(line);
    }
    file.close();
  }

  void point(int x, int y, Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(renderer, x, y);
  }

  void rect(int x, int y, int blockSize, const std::string& mapHit) {
    for(int cx = x; cx < x + blockSize; cx++) {
      for(int cy = y; cy < y + blockSize; cy++) {
        int tx = ((cx - x) * tsize) / blockSize;
        int ty = ((cy - y) * tsize) / blockSize;

        Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b , 255);
        SDL_RenderDrawPoint(renderer, cx, cy);
      }
    }
  }

  Impact cast_ray(float a) {
    float d = 0;
    std::string mapHit;
    int tx;

    while(true) {
      int x = static_cast<int>(player.x + d * cos(a)); 
      int y = static_cast<int>(player.y + d * sin(a)); 
      
      int i = static_cast<int>(x / BLOCK);
      int j = static_cast<int>(y / BLOCK);


      if (map[j][i] != ' ') {
        mapHit = map[j][i];

        int hitx = x - i * BLOCK;
        int hity = y - j * BLOCK;
        int maxhit;

        if (hitx == 0 || hitx == BLOCK - 1) {
          maxhit = hity;
        } else {
          maxhit = hitx;
        }

        tx = maxhit * tsize / BLOCK;

        break;
      }
     
//      point(x, y, W);
      
      d += 1;
    }
    return Impact{d, mapHit, tx};
  }

  void draw_stake(int x, float h, Impact i) {
    float start = SCREEN_HEIGHT/2.0f - h/2.0f;
    float end = start + h;

    for (int y = start; y < end; y++) {
      int ty = (y - start) * tsize / h;
      Color c = ImageLoader::getPixelColor(i.mapHit, i.tx, ty);
      SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

      SDL_RenderDrawPoint(renderer, x, y);
    }
  }

  int render() {

    bool flag = false;

    // Lanzar rayos y dibujar estacas en el lado izquierdo de la pantalla
    for (int i = 1; i < SCREEN_WIDTH; i++) {
        double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
        Impact impact = cast_ray(a);
        float d = impact.d;
        Color c = Color(255, 0, 0);

        if (d == 0) {
          return 1;
        }

        int x = i;
        float h = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(d) * static_cast<float>(scale);
        draw_stake(x, h, impact);

        if (impact.mapHit == "g") {
          flag = true;
        }
    }

    // Dibujar el minimapa en la esquina superior derecha de la pantalla
    int minimapSize = SCREEN_WIDTH / 7;  // Make the minimap smaller
    int startX = SCREEN_WIDTH - minimapSize - 50;  // Coordenada x de inicio del minimapa, moved 10 pixels to the left
    int startY = 0;  // Coordenada y de inicio del minimapa
    int endX = SCREEN_WIDTH - 10;  // Coordenada x de fin del minimapa, moved 10 pixels to the left
    int endY = minimapSize - 10;  // Coordenada y de fin del minimapa
    // Adjust the size of the minimap and each block
    int smallBlockSize = BLOCK / 5;  // Make each block smaller

    // Set the draw color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Create an SDL_Rect for the minimap
    SDL_Rect minimapRect;
    minimapRect.x = startX;
    minimapRect.y = startY;
    minimapRect.w = minimapSize + 40;
    minimapRect.h = minimapSize - 10;

    // Fill the minimap area with the background color
    SDL_RenderFillRect(renderer, &minimapRect);

    for (int y = startY; y < endY; y += smallBlockSize) {  // Use smallBlockSize as the loop increment
      for (int x = startX; x < endX; x += smallBlockSize) {  // Use smallBlockSize as the loop increment
        int i = std::min(static_cast<int>((x - startX) / smallBlockSize), static_cast<int>(map[0].size() - 1));
        int j = std::min(static_cast<int>((y - startY) / smallBlockSize), static_cast<int>(map.size() - 1));

        int min = std::min(1, 10);

        if (map[j][i] != ' ') {
            std::string mapHit;
            mapHit = map[j][i];
            Color c = Color(255, 0, 0);
            rect(x, y, smallBlockSize, mapHit);  // Pass smallBlockSize to the rect function
        }
      }
    
      if (flag) return 0;
    }

    // Dibujar el jugador en el minimapa
    // Scale the player's coordinates to the minimap's size
    int minimapPlayerX = startX + 10 + ((player.x * 1.8) / smallBlockSize);
    int minimapPlayerY = startY + 10 + ((player.y * 1.8) / smallBlockSize);

    // Ensure the point is within the bounds of the minimap
    minimapPlayerX = std::max(startX, std::min(minimapPlayerX, endX - 1));
    minimapPlayerY = std::max(startY, std::min(minimapPlayerY, endY - 1));

    // Set the draw color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw a point at the player's position on the minimap
    SDL_RenderDrawPoint(renderer, minimapPlayerX, minimapPlayerY);

    return 2;
  }

  Player player;
private:
  int scale;
  SDL_Renderer* renderer;
  std::vector<std::string> map;
  int tsize;
};
