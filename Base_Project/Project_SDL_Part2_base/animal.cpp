// todo: animal, sheep, etc.

#include "Project_SDL2.h"

void sheep::move() {
    // Manage step.
    // todo: figure out whuy there's two conditions
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

bool sheep::interact(interact_object *other_object, SDL_Rect* other_position) {
    // Boost speed if close to wolf.
    if (other_object->has_property("wolf") &&
        distance(position_ptr_, other_position) == sheep_danger_distance ) {
        // Set timer for temporary speed boost and movement change.

        // Boost sheep speed.
        velocity_x_ *= -1;
        velocity_x_ += velocity_x_ > 0 ? velocity_boost : -velocity_boost;
        velocity_y_ *= -1;
        velocity_y_ += velocity_y_ > 0 ? velocity_boost : -velocity_boost;
        this->step(true, true);
    }
    else if (is_reproduction_time() && other_object->has_property("sheep") && has_different_sex(other_object)) {
        // todo: reset time_before_reproduction timer.
        time_before_reproduction = ...;
        return true;
    }

    return false;
}

bool wolf::interact(interact_object *other_object, SDL_Rect *other_object_position) {
    if (other_object->has_property("shepherd_dog")) {
        if (nearest_shepherd_dog_position == position_ptr_)
            nearest_shepherd_dog_position = other_object_position;
        else {
            double object_distance = distance(this->position_ptr_, other_object_position);
            if (object_distance < distance(this->position_ptr_, nearest_shepherd_dog_position))
                nearest_shepherd_dog_position = other_object_position;
        }
    }
    else if (other_object->has_property("sheep")) {
        if (nearest_sheep_position == position_ptr_)
            nearest_sheep_position = other_object_position;
        else {
            double object_distance = distance(this->position_ptr_, other_object_position);
            // Kill sheep if too close else update nearest sheep position..
            if (object_distance == 0.0) {
                // todo: maybe increase kill distance
                // Reset last meal time.
                last_meal_time = SDL_GetTicks();
                other_object->mark_dead();
            }
            else if (object_distance < distance(this->position_ptr_, nearest_sheep_position)) {
                // Nearest sheep should not be too close to nearest dog.
                if (distance(nearest_sheep_position, nearest_shepherd_dog_position) > wolf_danger_distance)
                    nearest_sheep_position = other_object_position;
            }
        }
    }
    return false;
}

void wolf::move() {
    // Starve wolf if has not caught sheep for a long time.
    if (time_to_die()) {
        mark_dead();
        return;
    }

    // Update velocity vector to follow nearest sheep.
    velocity_x_ = nearest_sheep_position->x - this->position_ptr_->x;
    velocity_y_ = nearest_sheep_position->y - this->position_ptr_->y;

    // Reverse direction of wolf if needed.
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
