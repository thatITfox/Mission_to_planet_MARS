#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <math.h>
#include <random>
#include <vector>
#include "player.hpp"
#include "sprite.hpp"

const int WIDTH = 800, HEIGHT = 600;
int background1 = 0, background2 = -HEIGHT;
int points = 0;
bool game_over = false;
bool paused = false;
bool start_menu = true;

bool checkCollision(const Sprite &sprite1, const Sprite &sprite2){
    return (sprite1.x < sprite2.x + sprite2.w && sprite1.x + sprite1.w > sprite2.x &&
            sprite1.y < sprite2.y + sprite2.h && sprite1.y + sprite1.h > sprite2.y);
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Mission to planet MARS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;

    SDL_Texture* player_texture = IMG_LoadTexture(renderer, "./images/rocket2.png");
    SDL_Texture* background = IMG_LoadTexture(renderer, "./images/background.png");
    SDL_Texture* laser = IMG_LoadTexture(renderer, "./images/laser.png");
    SDL_Texture* asteroid_texture = IMG_LoadTexture(renderer, "./images/asteroid2.png");
    SDL_Texture* pause_page = IMG_LoadTexture(renderer, "./images/paused.png");
    SDL_Texture* battery_texture = IMG_LoadTexture(renderer, "./images/battery.png");
    SDL_Texture* start_page = IMG_LoadTexture(renderer, "./images/elon_musk4.png");
    SDL_Texture* crashed_screen = IMG_LoadTexture(renderer, "./images/crashed.png");
    TTF_Init();
    TTF_Font* VCR = TTF_OpenFont("./VCR_OSD_MONO_1.001.ttf", 24);
    SDL_Color White = {255, 255, 255};

    if(player_texture == NULL){
        std::cout << "Error loading player texture" << std::endl;
        return 1;
    }

    Player player(0, 0, 64, 68);
    player.x = WIDTH/2 - player.w/2;
    player.y = HEIGHT - (player.h+140);

    // creates a vector for the bullets that are gonna be on screen
    std::vector<Sprite> Bullets;
    std::vector<Sprite> Asteroids;
    std::vector<Sprite> Batteries;

    for(int i=0;i<10;i++){
        Asteroids.push_back(Sprite((rand()%727), -(rand()%1201+100), 74, 52, 3));
    }
    for(int i=0;i<2;i++){
        Batteries.push_back(Sprite((rand()%790), -(rand()%1201+100), 11, 20, 4));
    }

    bool isShooting = false;
    bool running = true;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) running = false;

            if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_LEFT: player.speed = -3;
                        break;
                    case SDLK_RIGHT: player.speed = 3;
                        break;
                    case SDLK_SPACE:
                        if(!isShooting && !game_over && !paused && !start_menu){
                            // make it so you can't shoot again without letting the button off
                            isShooting = true;
                            // shoot bullet by putting it to the vector of running bullets
                            if(player.bullets > 0){
                                Bullets.push_back(Sprite(player.x+(player.w/2), player.y, 5, 20, -4));
                                player.bullets--;
                            }
                        }
                        break;
                    case SDLK_RETURN:
                        background1 = 0, background2 = -HEIGHT;
                        points = 0;
                        start_menu = false;
                    case SDLK_r: 
                        // restart game code here please
                        if(game_over){ // make sure to only restart if the player actually lost
                            for(auto &asteroid : Asteroids){
                                asteroid.x = rand()%727; asteroid.y = -(rand()%1201+600);
                            }
                            for(auto &battery : Batteries){
                                battery.x = rand()%890; battery.y = -(rand()%1201+600);
                            }
                            background1 = 0, background2 = -HEIGHT;
                            Bullets.clear();
                            player.bullets = 5;
                            player.x = WIDTH/2-player.w;
                            points = 0;
                            game_over = false;
                        }
                        break;
                    case SDLK_p:
                        if(!game_over){
                            paused = !paused;
                        }
                }
            }
            if(event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_LEFT: if(player.speed < 0) player.speed = 0;
                        break;
                    case SDLK_RIGHT: if(player.speed > 0) player.speed = 0;
                        break;
                    case SDLK_SPACE: isShooting = false;
                }
            }
        }

        SDL_RenderClear(renderer);

        // render the background and make sure they are at the right place
        if(background1 > HEIGHT) background1 = -HEIGHT;
        if(background2 > HEIGHT) {
            background2 = -HEIGHT;
            points++;
            if(points%2==0){
                Asteroids.push_back(Sprite((rand()%727), -(rand()%1201), 74, 52, 3));
            }
            // std::cout << points << std::endl;
        }
        // render backhround 1
        SDL_Rect background_rect = {0, background1, WIDTH, HEIGHT};
        SDL_RenderCopy(renderer, background, NULL, &background_rect);
        // render background 2
        SDL_Rect background_rect2 = {0, background2, WIDTH, HEIGHT};
        SDL_RenderCopy(renderer, background, NULL, &background_rect2);
        // draw the player on the screen
        SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
        SDL_RenderCopy(renderer, player_texture, NULL, &playerRect);

        for(auto &bullet : Bullets){
            SDL_Rect bullet_rect = {bullet.x, bullet.y, bullet.w, bullet.h};
            SDL_RenderCopy(renderer, laser, NULL, &bullet_rect);
        }
        for(auto &asteroid : Asteroids){
            SDL_Rect asteroid_rect = {asteroid.x, asteroid.y, asteroid.w, asteroid.h};
            SDL_RenderCopy(renderer, asteroid_texture, NULL, &asteroid_rect);
        }
        for(auto &battery : Batteries){
            SDL_Rect battery_rect = {battery.x, battery.y, battery.w, battery.h};
            SDL_RenderCopy(renderer, battery_texture, NULL, &battery_rect);
        }
        if(game_over){
            SDL_Rect game_over_rect = {0, 0, 800, 100};
            SDL_RenderCopy(renderer, crashed_screen, NULL, &game_over_rect);
        }
        if(paused){
            SDL_Rect pause_rect = {0, 0, 800, 100};
            SDL_RenderCopy(renderer, pause_page, NULL, &pause_rect);
        }
        if(start_menu){
            background1+=2; background2+=2;
            SDL_Rect start_rect = {0, 0, 800, 500};
            SDL_RenderCopy(renderer, start_page, NULL, &start_rect);
        }
        if(!start_menu){
            // this is very bad, so bad yet i feel like it should be destroyed
            // i tried multiple times to find a better way but every time i change it
            // stuff will break again, if you're smarter than me, plese, i beg to god help me
            SDL_Surface* textSurface = TTF_RenderText_Solid(VCR, ("level: " + std::to_string(points)).c_str(), White);
            SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect renderQuad = {0, 0, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, text, NULL, &renderQuad);
            SDL_Surface* BullettextSurface = TTF_RenderText_Solid(VCR, ("bullets: " + std::to_string(player.bullets)).c_str(), White);
            SDL_Texture* Bullettext = SDL_CreateTextureFromSurface(renderer, BullettextSurface);
            SDL_Rect BulletrenderQuad = {0, textSurface->h, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, Bullettext, NULL, &BulletrenderQuad);
            // this code in the bottom slows down the game but is necesary for garbage collection
            // TODO: get better way to update and render the score texture, since this is not the way bro
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(text);
            SDL_FreeSurface(BullettextSurface);
            SDL_DestroyTexture(Bullettext);
            
        }
        
        if(!game_over && !paused && !start_menu){
            // update the background
            background1+=2; background2+=2;
            // update the player location
            player.update();
            // for bullet in bullets, update bullet location
            for(auto &bullet : Bullets){
                bullet.update();
                if(bullet.y < -bullet.h){
                    Bullets.erase(Bullets.begin());
                    // this will remove the first element
                    // since only the first element will be out first
                }
            }
            for(auto &battery : Batteries){
                battery.update();
                if(battery.y > HEIGHT){
                    battery.x = rand()%727; battery.y = -(rand()%1201);
                }
                if(battery.x < player.x + player.w && battery.x + battery.w > player.x
                && battery.y < player.y + player.h && battery.y + battery.h > player.y){
                    player.bullets++;
                    battery.x = rand()%727; battery.y = -(rand()%1201);
                }

            }

            for(auto &asteroid : Asteroids){
                asteroid.update();
                if(asteroid.y > HEIGHT){
                    asteroid.x = rand()%727; asteroid.y = -(rand()%1201);
                }
                for(int i=0;i<Bullets.size();i++){
                    if(checkCollision(asteroid, Bullets[i])){
                        asteroid.x = rand()%727; asteroid.y = -(rand()%1201);
                        Bullets.erase(Bullets.begin()+i);
                    }
                }
                if(asteroid.x < player.x + player.w-20 && asteroid.x + asteroid.w > player.x+20
                && asteroid.y < player.y + player.h-20 && asteroid.y + asteroid.h > player.y+20){
                    game_over = true;
                }
            }
        }

        // make sure the player doesn't go off screen
        if(player.x > WIDTH - player.w) player.x = WIDTH - player.w;
        if(player.x < 0) player.x = 0;

        SDL_RenderPresent(renderer);
        SDL_Delay(5); // haha, don't remove this code please-

    }
    // clean up the textures
    SDL_DestroyTexture(player_texture);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(laser);
    SDL_DestroyTexture(asteroid_texture);
    SDL_DestroyTexture(pause_page);
    SDL_DestroyTexture(start_page);
    SDL_DestroyTexture(crashed_screen);
    // destroy the window and renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(VCR);
    TTF_Quit();
    SDL_Quit();
    return 0;
}