// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include <set>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 900; // Height of window in pixel
constexpr unsigned sheep_width = 50; // Width of sheep in pixel
constexpr unsigned sheep_height = 50; // Height of sheep in pixel
constexpr unsigned wolf_width = 50; // Width of wolf in pixel
constexpr unsigned wolf_height = 25; // Height of wolf in pixel

// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

// Path to sheep and wolf texture
const std::string sheep_texture_path = "../../media/sheep.png";
const std::string wolf_texture_path = "../../media/wolf.png";
const std::string shepherd_dog_texture_path = "../../media/sheperd_dog.png";
const std::string shepherd_texture_path = "../../media/sheperd.png";

        // Helper function to initialize SDL
void init();

class interact_object {
protected:
    std::set<std::string> properties;
public:
    interact_object();  // constructor (todo: should initiate properties with "alive" property)
    virtual ~interact_object(){}; // destructor
    virtual void interact(interact_object *other_object); // todo: handle interaction between characters; could maybe take another interact object

    // "get" and "add" functions for interact object properties
    void add_property(std::string);
    void mark_dead();
    bool is_alive();
};

class rendered_object : public interact_object {
protected:
    // Attribute(s) to define its position
    SDL_Rect* position_ptr_; // Rectangle containing position of animal
private: // todo: elements below should maybe not be private
    SDL_Surface* window_surface_ptr_; // ptr to the surface on which we want the
    // animal to be drawn, also non-owning
    SDL_Surface* image_ptr_; // The texture of the sheep (the loaded image), use
    // load_surface_for
public:
    // todo: create a constructor
    void draw(); // todo: Draw the animal on the scree<n <-> window_surface_ptr.
    // Note that this function is not virtual, it does not depend
    // on the static type of the instance
    virtual ~rendered_object(){
        SDL_FreeSurface(image_ptr_);
        delete position_ptr_;
    }
};

class moving_object : public rendered_object {
protected:
    int velocity_x = 0; // maybe the same as step_x and step_y that were used before
    int velocity_y = 0;
public:
    virtual ~moving_object(){};
    virtual void move() = 0; // todo: Animals move around, but in a different
    // fashion depending on which type of animal
    void step();
};

class playable_character {

};

class shepherd {

};

class animal {
public:
  animal(const std::string& file_path, SDL_Surface* window_surface_ptr);
  // todo: The constructor has to load the sdl_surface that corresponds to the
  // texture
  virtual ~animal(){
  }; // todo: Use the destructor to release memory and "clean up
               // behind you"
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {
  public:
    sheep(SDL_Surface* window_surface_ptr, unsigned seed):animal(sheep_texture_path, window_surface_ptr){
        position_ptr_->w = sheep_width;
        position_ptr_->h = sheep_height;

        // Randomly select movement direction.
        srand(time(0) + seed);

        while (step_x == 0 && step_y == 0) {
            step_x = -1 + rand() % 3;
            step_y = -1 + rand() % 3;
        }

    };

    virtual ~sheep(){};// destructor for the sheep

    virtual void move() override;
  // implement functions that are purely virtual in base class
};

class wolf : public animal {
private:
    unsigned seed_;
    // Dtor
  public:
    wolf(SDL_Surface* window_surface_ptr, unsigned seed):animal(wolf_texture_path, window_surface_ptr){
        position_ptr_->w = wolf_width;
        position_ptr_->h = wolf_height;
        seed_ = seed;
    };

    virtual ~wolf(){}; // destructor for the wolf

    virtual void move() override;
};

// Insert here:
// class wolf, derived from animal
// Use only sheep at first. Once the application works
// for sheep you can add the wolves


// The "ground" on which all the animals live (like the std::vector
// in the zoo example).
class ground {
private:
  // Attention, NON-OWNING ptr, again to the screen
  SDL_Surface* window_surface_ptr_;

  // Some attribute to store all the wolves and sheep
  // here
public:
    // todo: move to private and create getter
    std::vector<moving_object*> moving_objects;
    ground(SDL_Surface* window_surface_ptr); // todo: Ctor
  ~ground(); // todo: Dtor, again for clean up (if necessary)
  void add_animal(animal *animal); // todo: Add an animal
  void update(); // todo: "refresh the screen": Move animals and draw them
  // Possibly other methods, depends on your implementation
};

// The application class, which is in charge of generating the window
class application {
private:
  // The following are OWNING ptrs
  SDL_Window* window_ptr_;
  SDL_Surface* window_surface_ptr_;
  SDL_Event window_event_;

  // Other attributes here, for example an instance of ground
  ground* ground_ptr_;
  unsigned seed_itr = 0;

public:
  application(unsigned n_sheep, unsigned n_wolf); // Ctor
    ~application();                                 // dtor

  int loop(unsigned period); // main loop of the application.
                             // this ensures that the screen is updated
                             // at the correct rate.
                             // See SDL_GetTicks() and SDL_Delay() to enforce a
                             // duration the application should terminate after
                             // 'period' seconds
};
