// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL2.h"

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

// Function to clamp position of animal.
int clamp(int position, int max_position) {
    if (position < frame_boundary)
        return frame_boundary;
    if (position > (max_position - frame_boundary))
        return max_position - frame_boundary;

    return position;
}

// Helper function to calculate distance between two objects.
double distance(SDL_Rect* first_object_pos, SDL_Rect* second_object_pos) {
    int x1 = first_object_pos->x;
    int x2 = second_object_pos->x;
    int y1 = first_object_pos->y;
    int y2 = second_object_pos->y;

    return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
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

// Constructor for rendered object.
rendered_object::rendered_object(const std::string &file_path, SDL_Surface* window_surface_ptr, int object_width, int object_height) {
    std::cout << file_path + "\n";
    image_ptr_ = load_surface_for(file_path, window_surface_ptr);
    window_surface_ptr_ = window_surface_ptr;

    // Set initial (random) position and start object movement.
    position_ptr_ = new SDL_Rect();
    position_ptr_->x = clamp(frame_boundary + (rand() % frame_width), frame_width);
    position_ptr_->y = clamp(frame_boundary + (rand() % frame_height), frame_height);

    // Set object width and height.
    position_ptr_->w = object_width;
    position_ptr_->h = object_height;

    draw();
}

// Draw function.
void rendered_object::draw() {
    // Blit onto the screen surface
    if (SDL_BlitScaled(image_ptr_, NULL, window_surface_ptr_, position_ptr_) < 0)
        throw std::runtime_error("BlitSurface error: " + std::string(SDL_GetError()) + "\n");
}

// Constructor for application.
application::application(unsigned n_sheep, unsigned n_wolf, unsigned n_shepherd_dog) {
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
        ground_ptr_->add_moving_object(new sheep(window_surface_ptr_, seed_itr));
        seed_itr += 50;
    }
    for (unsigned int i = 0; i < n_wolf; i++) {
        ground_ptr_->add_moving_object(new wolf(window_surface_ptr_, seed_itr));
        seed_itr += 50;
    }
    for (unsigned int i = 0; i < n_shepherd_dog; i++) {
        ground_ptr_->add_moving_object(new shepherd_dog(window_surface_ptr_, seed_itr));
        seed_itr += 50;
    }

    std::cout << "Added animals.\n";
}

// Destructor for application.
application::~application() {
    delete ground_ptr_;
    // Close and destroy the surface and window.
    // SDL_FreeSurface(window_surface_ptr_);
    SDL_DestroyWindow(window_ptr_);
}

void ground::update(SDL_Window* window_ptr) {
    /**
     * todo: if algo fails, start by just deleting all dead animals, then do interactions
     * no object can interact with a dead object anyways
     */
    for (unsigned int i = 0; i < moving_objects.size(); i++) {
        // todo: Find better way to add object in random position.
        moving_objects[i]->move(new sheep(window_surface_ptr_, seed_itr));
        for (unsigned int j = 0; i < moving_objects.size(); i++) {
            // A moving object should not interact with itself.
            if (i == j)
                continue;
            if (moving_objects[i]->interact(moving_objects[j], nullptr)) {
                add_moving_object()
            };
            // todo: Add offspring if any.

            // Remove potential dead object.
            if (!moving_objects[i]->is_alive()) {
                moving_objects.erase(moving_objects.begin() + i--);
                break;  // dead object at index i no longer needs to interact with others
            } else if (!moving_objects[j]) {
                moving_objects.erase(moving_objects.begin() + j--);
            }
        }
    }
    // Refresh screen.
    SDL_UpdateWindowSurface(window_ptr);
    SDL_Delay(1000 * frame_time);
    SDL_FillRect(window_surface_ptr_, NULL,
                 SDL_MapRGB(window_surface_ptr_->format, 255, 255, 255));
}

// todo: finish
int application::loop(unsigned period) {
    while (period) {
        ground_ptr_->update(window_ptr_);
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
    while (!moving_objects.empty())
    {
        moving_object* current_object = moving_objects.back();
        delete current_object;
        moving_objects.pop_back();
    }
}

void ground::add_moving_object(moving_object *moving_object) {
    moving_objects.push_back(moving_object);
}