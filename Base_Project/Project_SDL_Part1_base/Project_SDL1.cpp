// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>

void init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("init():" + std::string(SDL_GetError()));

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error("init(): SDL_image could not initialize! "
                             "SDL_image Error: " +
                             std::string(IMG_GetError()));
}

namespace {
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.

SDL_Surface* load_surface_for(const std::string& path,
                              SDL_Surface* window_surface_ptr) {

  // Helper function to load a png for a specific surface
  // See SDL_ConvertSurface

    // Load image.
    SDL_Surface* image_surface = IMG_Load(path.c_str());
    if(image_surface == NULL)
        throw std::runtime_error("Failed to load image" + path + "with error: " + std::string(IMG_GetError()) + "\n");

    // Convert created surface to windows surface format.
    SDL_Surface* converted_surface = SDL_ConvertSurface(image_surface, window_surface_ptr->format, 0);
    if(converted_surface == NULL)
    {
        throw std::runtime_error("Unable to convert image" + path + "with Error: " + std::string(SDL_GetError()) + "\n");
    }

    // Free temporary image surface.
    SDL_FreeSurface(image_surface);

    return converted_surface;
}
} // namespace

// Constructor for application.
application::application(unsigned int n_sheep, unsigned int n_wolf) {
    // Create window surface.
    window_ptr_ = SDL_CreateWindow("SDL2 Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   frame_width, frame_height, 0);
    window_surface_ptr_ = SDL_GetWindowSurface(window_ptr_);

    // todo: Use correct color.
    SDL_FillRect(window_surface_ptr_, NULL, SDL_MapRGB(window_surface_ptr_->format, 255, 255, 255));

    // Create ground with animals.
    ground_ptr_ = new ground(window_surface_ptr_);
    std::cout << "Created ground for application.\n";

    for (unsigned int i = 0; i < n_sheep; i++) {
        ground_ptr_->add_animal(new sheep(window_surface_ptr_));
    }

    std::cout << "Added animals.\n";

    /*
    for (unsigned int i = 0; i < n_wolf; i++) {
        // Missing arguments for wolf
        ground_ptr_->add_animal(new wolf())
    }
    */
}

// Destructor for application.
application::~application() {
    // Close and destroy the surface and window.
    SDL_FreeSurface(window_surface_ptr_);
    SDL_DestroyWindow(window_ptr_);
}

// todo: finish
int application::loop(unsigned period) {
    while (period) {
        SDL_UpdateWindowSurface(window_ptr_);
        SDL_Delay(1000 * frame_time);
        period--;
    }

    return 0;
}

// Constructor for ground.
ground::ground(SDL_Surface* window_surface_ptr) {
    window_surface_ptr_ = window_surface_ptr;
}

// Destructor for ground.
ground::~ground() {
    // Free all animals in vector.
    while (!animals.empty()) {
        animal* current_animal = animals.back();
        delete current_animal;
        animals.pop_back();
    }
}

void ground::add_animal(animal *animal) {
    animals.push_back(animal);
}


// Constructor for animal.
animal::animal(const std::string &file_path, SDL_Surface* window_surface_ptr) {
    std::cout << file_path + "\n";
    image_ptr_ = load_surface_for(file_path, window_surface_ptr);
    window_surface_ptr_ = window_surface_ptr;

    // Set initial (random) position and start animal movement.
    position_ptr_ = new SDL_Rect();
    position_ptr_->x = frame_boundary + (rand() % frame_width);
    std::cout << position_ptr_->x << "\n";

    position_ptr_->y = frame_boundary + (rand() % frame_height);
    std::cout << position_ptr_->y << "\n";

    position_ptr_->w = animal_width;
    std::cout << position_ptr_->w << "\n";

    position_ptr_->h = animal_height;
    std::cout << position_ptr_->h << "\n";

    draw();
}

// Draw function.
void animal::draw() {
    // Blit onto the screen surface
    if (!image_ptr_)
        std::cout << "Null image surface passed\n";
    if (!window_surface_ptr_)
        std::cout << "Null window surface passed\n";
    if (SDL_BlitScaled(image_ptr_, NULL, window_surface_ptr_, position_ptr_) < 0)
        throw std::runtime_error("BlitSurface error: " + std::string(SDL_GetError()) + "\n");
}

void sheep::move()
{
  // This is for the first movement
  int random = rand() % 3;
  switch(random)
  {
    case 0:
      position_ptr_->x += 1;
    case 1:
      position_ptr_->y += 1;
    case 2:
      position_ptr_->x -= 1;
    case 3:
      position_ptr_->y -= 1;
  }

  draw();
}