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
    }
    // Manage step.
    // todo: figure out why there's two conditions
    // Reverse direction of sheep if needed.
    int step_x = 0;
    int step_y = 0;
    if (position_ptr_->x == frame_boundary || position_ptr_->x == frame_width - frame_boundary) {
        step_x = -velocity_x_ * 2;
    }
    if (position_ptr_->y == frame_boundary || position_ptr_->y == frame_height - frame_boundary) {
        step_y = -velocity_y_ * 2;
    }

    // Update position.
    step(step_x, step_y);
    draw();
}

bool sheep::interact(interact_object *other_object) {
    // Boost speed if close to wolf.
    if (other_object->has_property("hunter") &&
        distance(position_ptr_, other_object->get_position()) == sheep_danger_distance ) {
        // Set timer for temporary speed boost and movement change.

        // Boost sheep speed.
        if (velocity_x_) {
            velocity_x_ *= -1;
            velocity_x_ += velocity_x_ > 0 ? velocity_boost : -velocity_boost;
        }
        if (velocity_y_) {
            velocity_y_ *= -1;
            velocity_y_ += velocity_y_ > 0 ? velocity_boost : -velocity_boost;
        }
        this->step(0, 0);

        speed_timeout = SDL_GetTicks() + speed_boost_time;
    }
    else if (is_reproduction_time() && other_object->has_property("sheep") && has_different_sex(other_object)) {
        // todo: reset time_before_reproduction timer.
        time_before_reproduction = SDL_GetTicks() + sheep_reproduction_delay;
        return true;
    }

    return false;
}

bool wolf::interact(interact_object *other_object) {
    if (other_object->has_property("shepherd_dog")) {
        if (nearest_shepherd_dog_position == position_ptr_)
            nearest_shepherd_dog_position = other_object->get_position();
        else {
            double object_distance = distance(this->position_ptr_, other_object->get_position());
            if (object_distance < distance(this->position_ptr_, nearest_shepherd_dog_position))
                nearest_shepherd_dog_position = other_object->get_position();
        }
    }
    else if (other_object->has_property("prey")) {
        if (nearest_prey_position == position_ptr_)
            nearest_prey_position = other_object->get_position();
        else {
            double object_distance = distance(this->position_ptr_, other_object->get_position());
            // Kill sheep if too close else update nearest sheep position..
            if (object_distance == 0.0) {
                // todo: maybe increase kill distance
                // Reset wolf death time.
                death_time = SDL_GetTicks() + death_delay;
                other_object->mark_dead();
            }
            else if (object_distance < distance(this->position_ptr_, nearest_prey_position)) {
                // Nearest sheep should not be too close to nearest dog.
                if (distance(nearest_prey_position, nearest_shepherd_dog_position) > wolf_danger_distance)
                    nearest_prey_position = other_object->get_position();
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
        mark_dead();
        return;
    }

    // Update velocity vector to follow nearest sheep.
    velocity_x_ = nearest_prey_position->x - this->position_ptr_->x;
    velocity_y_ = nearest_prey_position->y - this->position_ptr_->y;

    // Reverse direction of wolf if needed.
    // todo: maybe remove since clamp is used
    int step_x = 0;
    int step_y = 0;
    if (position_ptr_->x == frame_boundary || position_ptr_->x == frame_width - frame_boundary) {
        step_x = -velocity_x_ * 2;
    }
    if (position_ptr_->y == frame_boundary || position_ptr_->y == frame_height - frame_boundary) {
        step_y = -velocity_y_ * 2;
    }

    step(step_x, step_y);
    draw();
}

void playable_character::move() {
    while(SDL_PollEvent(&window_event_)) {
        switch(window_event_.type) {
            case SDL_KEYDOWN:
                std::cout << "key pressed\n";
                std::cout << "Player position (x" << position_ptr_->x << ", y" << position_ptr_->y << ")\n";
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