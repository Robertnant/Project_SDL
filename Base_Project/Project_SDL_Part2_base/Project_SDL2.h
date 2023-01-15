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
const std::string shepherd_dog_texture_path = "../../media/shepherd_dog.png";
const std::string shepherd_texture_path = "../../media/shepherd.png";

        // Helper function to initialize SDL
void init();

class interact_object {
protected: // todo: maybe make properties private
    std::set<std::string> properties;
public:
    // All interact objects are supposed as alive
    interact_object() {
        properties.insert("alive");
    };
    virtual ~interact_object(){}; // destructor
    virtual void interact(interact_object *other_object); // todo: handle interaction between characters; could maybe take another interact object

    // "get" and "add" functions for interact object properties
    void add_property(std::string property) {
        properties.insert(property);
    };
    void mark_dead() {
        properties.erase("alive");
        properties.insert("dead");
    };
    bool is_alive() {
        return properties.find("alive") != properties.end();
    };
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
    rendered_object(const std::string &file_path, SDL_Surface* window_surface_ptr, int object_width, int object_height);
    void draw();
    // Note that this function is not virtual, it does not depend
    // on the static type of the instance
    virtual ~rendered_object(){
        SDL_FreeSurface(image_ptr_);
        delete position_ptr_;
    }
};

class moving_object : public rendered_object {
private:
    int velocity_x_ = 0; // maybe the same as step_x and step_y that were used before
    int velocity_y_ = 0;
public:
    moving_object(const std::string &file_path,
                  SDL_Surface* window_surface_ptr, int object_width, int object_height): rendered_object(
                          sheep_texture_path, window_surface_ptr, object_width, object_height){}; // todo: maybe add an initial velocity ? (not that necessary for now maybe)
    virtual ~moving_object(){};
    virtual void move() = 0;
    // fashion depending on which type of animal. Move calls step function to modify velocity
    void step(int step_x, int step_y) {
        velocity_x_ += step_x;
        velocity_y_ += step_y;
    };
};

class playable_character {

};

class shepherd {

};

class animal : public moving_object {
public:
  animal(const std::string &file_path, SDL_Surface* window_surface_ptr, int object_width,
         int object_height): moving_object(sheep_texture_path, window_surface_ptr,
                                           object_width, object_height){};
  virtual ~animal(){};
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {
  public:
    sheep(SDL_Surface* window_surface_ptr, unsigned seed):animal(
            sheep_texture_path, window_surface_ptr, sheep_width, sheep_height){

        // Randomly select movement direction.
        srand(time(0) + seed);

        int step_x = 0;
        int step_y = 0;
        while (step_x == 0 && step_y == 0) {
            step_x = -1 + rand() % 3;
            step_y = -1 + rand() % 3;
        }

        step(step_x, step_y);
    };

    virtual ~sheep(){};// destructor for the sheep

    virtual void move() override;  // todo: finish implementation
    virtual void interact(interact_object *other_object); // todo: implement
  // implement functions that are purely virtual in base class
};

class wolf : public animal {
private:
    unsigned seed_;
  public:
    wolf(SDL_Surface* window_surface_ptr, unsigned seed):animal(
            wolf_texture_path, window_surface_ptr, wolf_width, wolf_height){
        seed_ = seed;
    };

    virtual ~wolf(){}; // destructor for the wolf

    virtual void move() override;  // todo: finish implementation
    virtual void interact(interact_object *other_object) override; // todo: implement
};

class shepherd_dog : public animal {
private:
    unsigned seed_;
public:
    shepherd_dog(SDL_Surface* window_surface_ptr, unsigned seed):animal(
            wolf_texture_path, window_surface_ptr, wolf_width, wolf_height){
        seed_ = seed;
    };

    virtual ~shepherd_dog(){}; // destructor for the wolf

    virtual void move() override;  // todo: implement
    virtual void interact(interact_object *other_object) override; // todo: implement
};

// The "ground" on which all the objects live (like the std::vector
// in the zoo example).
class ground {
private:
  // Attention, NON-OWNING ptr, again to the screen
  SDL_Surface* window_surface_ptr_;

  // Some attribute to store all the moving objects
  std::vector<moving_object*> moving_objects;
public:
    ground(SDL_Surface* window_surface_ptr);
    ~ground();
    void add_moving_object(moving_object *moving_object);
    void update(SDL_Window* window_ptr);
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
  application(unsigned n_sheep, unsigned n_wolf, unsigned n_shepherd_dog); // Ctor
    ~application();                                 // dtor

  int loop(unsigned period); // main loop of the application.
                             // this ensures that the screen is updated
                             // at the correct rate.
                             // See SDL_GetTicks() and SDL_Delay() to enforce a
                             // duration the application should terminate after
                             // 'period' seconds
};
