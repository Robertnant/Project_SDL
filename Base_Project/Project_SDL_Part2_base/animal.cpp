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
        distance(position_ptr_, other_position) ==sheep_danger_distance ) {
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

void wolf::move() {
    // Manage step.
    if (position_ptr_->x == frame_boundary || position_ptr_->x == frame_width - frame_boundary) {
        step_x *= -1;
    }
    if (position_ptr_->y == frame_boundary || position_ptr_->y == frame_height - frame_boundary) {
        step_y *= -1;
    }

    srand(time(0) + seed_++);
    int random = rand() % 4;

    switch(random)
    {
        case 0:
            step_x = 1;
            break;
        case 1:
            step_y = 1;
            break;
        case 2:
            step_x = -1;
            break;
        case 3:
            step_y = -1;
    }

    // Update position.
    position_ptr_->x = clamp(position_ptr_->x + step_x, frame_width);
    position_ptr_->y = clamp(position_ptr_->y + step_y, frame_height);

    draw();
}
