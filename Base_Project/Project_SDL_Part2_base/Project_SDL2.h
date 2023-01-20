// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
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
constexpr unsigned shepherd_dog_width = 50; // Width of shepherd dog in pixel
constexpr unsigned shepherd_dog_height = 25; // Height of shepherd dog in pixel
constexpr unsigned shepherd_width = 50; // Width of shepherd in pixel
constexpr unsigned shepherd_height = 25; // Height of shepherd in pixel

// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

// Distance for which sheep should run away from wolf.
constexpr unsigned sheep_danger_distance = 40;
constexpr unsigned kill_distance = 30;

// Sheep temporary speed boost value (in absolute value).
constexpr unsigned velocity_boost = 3;
constexpr Uint32 speed_boost_time = 10000;

// Character's default velocity.
constexpr unsigned sheep_velocity = 1;
constexpr unsigned wolf_velocity = 1;
constexpr unsigned player_velocity = 2;
constexpr unsigned shepherd_dog_velocity = 1;

// Minimum distance between shepherd and dog.
constexpr unsigned shepherd_min_distance = 5;

// Sheep default time before reproduction.
constexpr unsigned sheep_reproduction_delay = 2500;

// Wolf time before death and delay before killing (to prevent sheep from dying at start of game).
constexpr unsigned death_delay = 1500000;
constexpr unsigned delay_before_kill = 3000;

// Wolf accepted distance between closest sheep and closest dog.
constexpr unsigned wolf_danger_distance = 200;

// Path to sheep and wolf texture
const std::string sheep_texture_path = "../../media/sheep.png";
const std::string wolf_texture_path = "../../media/wolf.png";
const std::string shepherd_dog_texture_path = "../../media/shepherd_dog.png";
const std::string shepherd_texture_path = "../../media/shepherd.png";

// Helper function to initialize SDL
void init();

// Helper function to calculate distance between two objects.
double distance(SDL_Rect* first_object_pos, SDL_Rect* second_object_pos);

// Helper function to clamp position of object.
int clamp(int position, int max_position);

class interact_object {
protected: // todo: maybe make properties private
    std::set<std::string> properties;
    // Attribute(s) to define its position
    SDL_Rect* position_ptr_; // Rectangle containing position of object
public:
    // All interact objects are supposed as alive
    interact_object() {
        properties.insert("alive");
    };
    virtual ~interact_object(){
        delete position_ptr_;
    }; // destructor
    /**
     * Interact function makes two objects interact with each other depending on their
     * properties as well as their poisition.
     * Interact returns true if a new offspring is created after interaction.
     */
    virtual bool interact(interact_object *other_object) = 0;

    // "get" and "add" functions for interact object properties
    void add_property(std::string property) {
        properties.insert(property);
    };
    void mark_dead() {
        properties.erase("alive");
        properties.insert("dead");
    };
    bool is_alive() {
        return std::find(properties.begin(), properties.end(), "alive") != properties.end();
    };
    bool has_property(std::string property) {
        return std::find(properties.begin(), properties.end(), property) != properties.end();
    }
    SDL_Rect* get_position() {
        return position_ptr_;
    };
};

class rendered_object : public interact_object {
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
    }
};

class moving_object : public rendered_object {
protected:
    int velocity_x_ = 0; // maybe the same as step_x and step_y that were used before
    int velocity_y_ = 0;
public:
    moving_object(const std::string &file_path,
                  SDL_Surface* window_surface_ptr, int object_width, int object_height): rendered_object(
                          file_path, window_surface_ptr, object_width, object_height){}; // todo: maybe add an initial velocity ? (not that necessary for now maybe)
    virtual ~moving_object(){};
    virtual void move() = 0;
    // fashion depending on which type of animal. Move calls step function to modify object position
    // Step modifies velocity of object through step_x and step_y and updates object position.
    void step(int increment_x, int increment_y) {
        velocity_x_ += increment_x;
        velocity_y_ += increment_y;
        position_ptr_->x = clamp(position_ptr_->x + velocity_x_, frame_width);
        position_ptr_->y = clamp(position_ptr_->y + velocity_y_, frame_height);
    };
};

class playable_character : public moving_object {
private:
    SDL_Event window_event_;
public:
    playable_character(const std::string &file_path, SDL_Surface* window_surface_ptr, int object_width,
                       int object_height, SDL_Event window_event) :
                       moving_object(file_path, window_surface_ptr, object_width, object_height) {
        window_event_ = window_event;
    }
    virtual void move() override;  // todo: implement
};

class shepherd : public playable_character {
public:
    shepherd(SDL_Surface* window_surface_ptr,
             SDL_Event window_event): playable_character(shepherd_texture_path, window_surface_ptr, shepherd_width,
                                                          shepherd_height, window_event) {
        // Add properties.
        add_property("shepherd");
        add_property("alive");

        position_ptr_->x = frame_width / 2;
        position_ptr_->y = frame_height / 2;
    };
    virtual bool interact(interact_object *other_object) override; // todo: implement
};

class animal : public moving_object {
public:
  animal(const std::string &file_path, SDL_Surface* window_surface_ptr, int object_width,
         int object_height): moving_object(file_path, window_surface_ptr,
                                           object_width, object_height){};
  virtual ~animal(){};
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {
private:
    Uint32 time_before_reproduction;
    Uint32 speed_timeout;
public:
    sheep(SDL_Surface* window_surface_ptr, unsigned seed):animal(
            sheep_texture_path, window_surface_ptr, sheep_width, sheep_height){
        speed_timeout = 0;
        time_before_reproduction = SDL_GetTicks() + sheep_reproduction_delay;

        // Add properties.
        add_property("sheep");
        add_property("alive");
        add_property("prey");
        // Randomly select sheep sex and movement direction.
        srand(time(0) + seed);
        if ((rand() % 2) == 0)
            add_property("male");
        else
            add_property("female");

        // In this case the possible velocities for x/y are: -sheep_velocity, 0, +sheep_velocity.
        int step_x = 0;
        int step_y = 0;
        while (step_x == 0 && step_y == 0) {
            step_x = (-1 + rand() % 3) * sheep_velocity;
            step_y = (-1 + rand() % 3) * sheep_velocity;
        }

        // Set initial position (random).
        position_ptr_->x = clamp(frame_boundary + (rand() % frame_width), frame_width);
        position_ptr_->y = clamp(frame_boundary + (rand() % frame_height), frame_height);

        // Set movement.
        step(step_x, step_y);
    };

    virtual ~sheep(){};// destructor for the sheep

    virtual void move() override;  // todo: finish implementation
    virtual bool interact(interact_object *other_object) override; // todo: implement
    bool has_different_sex(interact_object *other_object) {
        if (other_object->has_property("female"))
            return this->has_property("male");
        return this->has_property("female");
    };
    bool is_reproduction_time() {
        return SDL_TICKS_PASSED(SDL_GetTicks(), time_before_reproduction);
    }
  // implement functions that are purely virtual in base class
};

class wolf : public animal {
private:
    SDL_Rect *nearest_prey_position;
    SDL_Rect *nearest_shepherd_dog_position;
    Uint32 death_time;
    Uint32 wolf_delay_before_kill = 1000;
public:
    wolf(SDL_Surface* window_surface_ptr, unsigned seed):animal(
            wolf_texture_path, window_surface_ptr, wolf_width, wolf_height){
        // Add properties.
        add_property("wolf");
        add_property("hunter");
        add_property("alive");
        velocity_x_ = wolf_velocity;
        velocity_y_ = wolf_velocity;
        // Nearest sheep position and dog have not been found yet so wolf should stay still.
        nearest_prey_position = position_ptr_;
        nearest_shepherd_dog_position = position_ptr_;
        // Set timer for when wolf should die if starving.
        death_time = SDL_GetTicks() + death_delay;

        wolf_delay_before_kill = SDL_GetTicks() + delay_before_kill;

        // Set initial position (random).
        position_ptr_->x = clamp(frame_boundary + (rand() % frame_width), frame_width);
        position_ptr_->y = clamp(frame_boundary + (rand() % frame_height), frame_height);
    };

    virtual ~wolf(){}; // destructor for the wolf

    virtual void move() override;  // todo: finish implementation
    virtual bool interact(interact_object *other_object) override; // todo: implement
    bool time_to_die() {
        return SDL_TICKS_PASSED(SDL_GetTicks(), death_time);
    };
    bool can_kill() {
        return SDL_TICKS_PASSED(SDL_GetTicks(), wolf_delay_before_kill);;
    }
};

class shepherd_dog : public animal {
private:
    SDL_Rect *shepherd_position_;
public:
    shepherd_dog(SDL_Surface* window_surface_ptr):animal(
            shepherd_dog_texture_path, window_surface_ptr,
            shepherd_dog_width, shepherd_dog_height){
        // Set shepherd position this shepherd dog's position at first.
        shepherd_position_ = position_ptr_;

        // Add properties.
        add_property("shepherd_dog");
        add_property("alive");
    };

    virtual ~shepherd_dog(){}; // destructor for the wolf

    virtual void move() override;  // todo: implement
    virtual bool interact(interact_object *other_object) override; // todo: implement
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
    void add_moving_object(moving_object *moving_object) {
        moving_objects.push_back(moving_object);
    };
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
  application(unsigned n_sheep, unsigned n_wolf); // Ctor
    ~application();                                 // dtor

  int loop(unsigned period); // main loop of the application.
                             // this ensures that the screen is updated
                             // at the correct rate.
                             // See SDL_GetTicks() and SDL_Delay() to enforce a
                             // duration the application should terminate after
                             // 'period' seconds
};
