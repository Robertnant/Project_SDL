// todo: animal, sheep, etc.

#include "Project_SDL2.h"

void sheep::move() {
    // Recover sheep original speed if speed boost timeout has passed.
    if (speed_timeout != 0 && SDL_TICKS_PASSED(SDL_GetTicks(), speed_timeout)) {
        if (velocity_x_ > 0)
            velocity_x_ -= velocity_boost;
        else if (velocity_x_ < 0)
            velocity_x_ += velocity_boost;
        if (velocity_y_ > 0)
            velocity_y_ -= velocity_boost;
        else if (velocity_y_ < 0)
            velocity_y_ += velocity_boost;
        // Reset speed timeout.
        speed_timeout = 0;
    }
    // Manage step.
    // Update position.
    step(0, 0);

    // Check collision.
    if (position_ptr_->x <= frame_boundary ||
        position_ptr_->x + position_ptr_->w >= frame_width - frame_boundary) {
        velocity_x_ *= -1;
        step(0, 0);
    }
    if (position_ptr_->y <= frame_boundary ||
        position_ptr_->y + position_ptr_->h >= frame_height - frame_boundary) {
        velocity_y_ *= -1;
        step(0, 0);
    }
    draw();
}

bool sheep::interact(interact_object *other_object) {
    // Boost speed if close to wolf.
    int dist = distance(position_ptr_, other_object->get_position());
    if (speed_timeout == 0 && other_object->has_property("hunter") &&
        dist <= sheep_danger_distance ) {
        // Set timer for temporary speed boost and movement change.

        // Boost sheep speed.
        if (velocity_x_ != 0) {
            velocity_x_ *= -1;
            velocity_x_ += velocity_x_ > 0 ? velocity_boost : -velocity_boost;
        }
        if (velocity_y_ != 0) {
            velocity_y_ *= -1;
            velocity_y_ += velocity_y_ > 0 ? velocity_boost : -velocity_boost;
        }
        this->step(0, 0);

        speed_timeout = SDL_GetTicks() + speed_boost_time;
    }
    else if (distance(position_ptr_, other_object->get_position()) == 0) {
        if (is_reproduction_time() && other_object->has_property("sheep") && has_different_sex(other_object)) {
            time_before_reproduction = SDL_GetTicks() + sheep_reproduction_delay;
            return true;
        }
    }

    return false;
}

bool wolf::interact(interact_object *other_object) {
    if (other_object->has_property("shepherd_dog")) {
        if (nearest_shepherd_dog_position == position_ptr_) {
            nearest_shepherd_dog_position = other_object->get_position();
            std::cout << "Nearest shepherd dog found Case 1\n";
        }
        else {
            double object_distance = distance(position_ptr_, other_object->get_position());
            if (nearest_shepherd_dog_position != other_object->get_position() && object_distance < distance(position_ptr_, nearest_shepherd_dog_position)) {
                nearest_shepherd_dog_position = other_object->get_position();
                std::cout << "Nearest shepherd dog found\n";
            }
        }
    }
    else if (other_object->has_property("prey")) {
        // In all cases, nearest sheep should not be too close to nearest dog.
        if (nearest_prey_position == position_ptr_ && distance(nearest_prey_position, nearest_shepherd_dog_position) > wolf_danger_distance) {
            std::cout << "Prey found\n";
            nearest_prey_position = other_object->get_position();
        }
        else {
            double object_distance = distance(this->position_ptr_, other_object->get_position());
            // Kill sheep if too close else update nearest sheep position..
            if (can_kill() && object_distance < kill_distance) {
                // Reset wolf death time.
                death_time = SDL_GetTicks() + death_delay;
                other_object->mark_dead();
                // Reset nearest prey position..
                nearest_prey_position = position_ptr_;
            } else if (object_distance < distance(this->position_ptr_, nearest_prey_position) && distance(other_object->get_position(), nearest_shepherd_dog_position) > wolf_danger_distance) {
                nearest_prey_position = other_object->get_position();
            } else if (distance(nearest_prey_position, nearest_shepherd_dog_position) <= wolf_danger_distance){
                std::cout << "Current prey is too close to shepherd\n";
                nearest_prey_position = position_ptr_;
            }
        }
    }
    return false;
}

bool shepherd::interact(interact_object *other_object) {
    // Nothing particular to be done for now.
    return false;
}

bool shepherd_dog::interact(interact_object *other_object) {
    // Find shepherd.
    if (shepherd_position_ == position_ptr_ && other_object->has_property("shepherd"))
        shepherd_position_ = other_object->get_position();
    return false;
}

void shepherd_dog::move() {
    // Update velocity vector to follow shepherd.
    velocity_x_ = shepherd_position_->x - this->position_ptr_->x + 30;
    velocity_y_ = shepherd_position_->y - this->position_ptr_->y + 30;

    // todo: Add reverse direction if needed. If not needed, then remove direction reverse for wolf
    step(0, 0);
    draw();
}

void wolf::move() {
    // Starve wolf if has not caught sheep for a long time.
    if (time_to_die()) {
        std::cout << "Wolf died :/\n";
        mark_dead();
        return;
    }

    // Follow nearest sheep.
    int step_x = 0;
    int step_y = 0;
    double x_diff = nearest_prey_position->x - this->position_ptr_->x;
    double y_diff = nearest_prey_position->y - this->position_ptr_->y;

    // Case if sheep reached.
    if (x_diff == 0 && y_diff == 0) {
        // Do not move.
    }

    if (abs(x_diff) > abs(y_diff)) {
        if (x_diff > 0)
            position_ptr_->x += wolf_velocity;
        else
            position_ptr_->x -= wolf_velocity;
        velocity_y_ = 0;
    } else {
        if (y_diff > 0)
            position_ptr_->y += wolf_velocity;
        else
            position_ptr_->y -= wolf_velocity;
        velocity_x_ = 0;
    }


    step(step_x, step_y);

    // Check collision.
    if (position_ptr_->x <= frame_boundary ||
        position_ptr_->x + position_ptr_->w >= frame_width - frame_boundary) {
        velocity_x_ *= -1;
        step(0, 0);
    }
    if (position_ptr_->y <= frame_boundary ||
        position_ptr_->y + position_ptr_->h >= frame_height - frame_boundary) {
        velocity_y_ *= -1;
        step(0, 0);
    }
    draw();
}

void playable_character::move() {
    while(SDL_PollEvent(&window_event_)) {
        switch(window_event_.type) {
            case SDL_KEYDOWN:
                switch(window_event_.key.keysym.sym){
                    case SDLK_LEFT:
                        velocity_x_ = -player_velocity;
                        break;
                    case SDLK_RIGHT:
                        velocity_x_ = player_velocity;
                        break;
                    case SDLK_UP:
                        velocity_y_ = -player_velocity;
                        break;
                    case SDLK_DOWN:
                        velocity_y_ = player_velocity;
                        break;
                    default:
                        break;
                }
                break;
                // Zero velocities if necessary.
            case SDL_KEYUP:
                switch(window_event_.key.keysym.sym){
                    case SDLK_LEFT:
                        if(velocity_x_ < 0 )
                            velocity_x_ = 0;
                        break;
                    case SDLK_RIGHT:
                        if(velocity_x_ > 0 )
                            velocity_x_ = 0;
                        break;
                    case SDLK_UP:
                        if(velocity_y_ < 0 )
                            velocity_y_ = 0;
                        break;
                    case SDLK_DOWN:
                        if(velocity_y_ > 0 )
                            velocity_y_ = 0;
                        break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
    // Update player position.
    step(0, 0);
    draw();
}