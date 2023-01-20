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
            std::cout << "New BABYY\n";
            return true;
        }
    }

    return false;
}

bool wolf::interact(interact_object *other_object) {
    if (other_object->has_property("shepherd_dog")) {
        if (nearest_shepherd_dog_position == position_ptr_) {
            nearest_shepherd_dog_position = other_object->get_position();
        }
        else {
            double object_distance = distance(position_ptr_, other_object->get_position());
            if (nearest_shepherd_dog_position != other_object->get_position() && object_distance <
                    distance(position_ptr_, nearest_shepherd_dog_position)) {
                nearest_shepherd_dog_position = other_object->get_position();
            }
        }
    }
    else if (other_object->has_property("prey")) {
        // In all cases, nearest sheep should not be too close to nearest dog.
        if (nearest_prey_position == position_ptr_ && distance(nearest_prey_position, nearest_shepherd_dog_position) >
            wolf_danger_distance) {
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
            } else if (object_distance < distance(this->position_ptr_, nearest_prey_position) &&
                distance(other_object->get_position(), nearest_shepherd_dog_position) > wolf_danger_distance) {
                nearest_prey_position = other_object->get_position();
            } else if (distance(nearest_prey_position, nearest_shepherd_dog_position) <= wolf_danger_distance){
                nearest_prey_position = position_ptr_;
            }
        }
    }
    return false;
}

bool shepherd::interact(interact_object *other_object) {
    return false;
}

bool shepherd_dog::interact(interact_object *other_object) {
    // Find shepherd and stay next to them.
    if (shepherd_position_ == position_ptr_ && other_object->has_property("shepherd")) {
        shepherd_position_ = other_object->get_position();

        srand(time(0) + seed_);
        double angle = (double) rand() / RAND_MAX * 2 * M_PI;
        double min_radius = 50;
        double max_radius = 70;

        // Radius is reused to update dog position.
        radius_ = min_radius + (double) rand() / RAND_MAX * (max_radius);
        position_ptr_->x = shepherd_position_->x + radius_ * cos(angle);
        position_ptr_->y = shepherd_position_->y + radius_ * sin(angle);
    }

    return false;
}

void shepherd_dog::fetch(int x, int y) {
    has_order = true;
    ordered_position.x = x;
    ordered_position.y = y;
}

bool shepherd_dog::was_clicked(int x, int y) {
    if (x >= position_ptr_->x && x <= position_ptr_->x + position_ptr_->w &&
        y >= position_ptr_->y && y <= position_ptr_->y + position_ptr_->h) {
        return true;
    } else {
        return false;
    }
}

void shepherd_dog::move() {
    // Receive order if clicked.
    while(SDL_PollEvent(&this->window_event_)){
        switch(this->window_event_.type){
            case SDL_MOUSEBUTTONDOWN:
                if (first_click_ && was_clicked(window_event_.button.x, window_event_.button.y)) {
                    first_click_ = false;
                    std::cout << "Clicked\n";
                }
                else if (first_click_ == false) {
                    std::cout << "Clicked 2\n";
                    fetch(this->window_event_.button.x, this->window_event_.button.y);
                    first_click_ = true;
                }
                break;
        }
    }
    // Update velocity vector to follow shepherd.
    if (!has_order) {
        double new_angle = atan2(position_ptr_->y - shepherd_position_->y, position_ptr_->x - shepherd_position_->x);
        position_ptr_->x = shepherd_position_->x + radius_ * cos(new_angle);
        position_ptr_->y = shepherd_position_->y + radius_ * sin(new_angle);
        step(0, 0);
    } else {
        // Go to ordered position.
        int step_x = 0;
        int step_y = 0;
        double x_diff = ordered_position.x - this->position_ptr_->x;
        double y_diff = ordered_position.y - this->position_ptr_->y;

        // Case if position reached (approximately).
        if (x_diff >= 0 && x_diff <= 20 && y_diff >= 0 && y_diff <= 20) {
            has_order = false;
            std::cout << "position reached\n";
        }

        if (abs(x_diff) > abs(y_diff)) {
            if (x_diff > 0)
                position_ptr_->x += shepherd_dog_velocity;
            else
                position_ptr_->x -= shepherd_dog_velocity;
            velocity_y_ = 0;
        } else {
            if (y_diff > 0)
                position_ptr_->y += shepherd_dog_velocity;
            else
                position_ptr_->y -= shepherd_dog_velocity;
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
    }
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
    if (position_ptr_->x < nearest_prey_position->x)
        position_ptr_->x += wolf_velocity;
    else if (position_ptr_->x > nearest_prey_position->x)
        position_ptr_->x-= wolf_velocity;

    if (position_ptr_->y < nearest_prey_position->y)
        position_ptr_->y += wolf_velocity;
    else if (position_ptr_->y > nearest_prey_position->y)
        position_ptr_->y -= wolf_velocity;
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

void playable_character::move() {
    velocity_x_ = 0;
    velocity_y_ = 0;
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_GetScancodeFromKey(SDLK_UP)]) {
        velocity_y_ -= player_velocity;
    }
    if (key_state[SDL_GetScancodeFromKey(SDLK_DOWN)]) {
        velocity_y_ += player_velocity;
    }
    if (key_state[SDL_GetScancodeFromKey(SDLK_RIGHT)]) {
        velocity_x_ += player_velocity;
    }
    if (key_state[SDL_GetScancodeFromKey(SDLK_LEFT)]) {
        velocity_x_ -= player_velocity;
    }
    // Update player position.
    step(0, 0);
    draw();
}