// todo: animal, sheep, etc.

#include "Project_SDL2.h"

void sheep::move()
{
    // Manage step.
    if (position_ptr_->x == frame_boundary || position_ptr_->x == frame_width - frame_boundary) {
        step_x *= -1;
    }
    if (position_ptr_->y == frame_boundary || position_ptr_->y == frame_height - frame_boundary) {
        step_y *= -1;
    }

    // Update position.
    position_ptr_->x = clamp(position_ptr_->x + step_x, frame_width);
    position_ptr_->y = clamp(position_ptr_->y + step_y, frame_height);

    draw();
}

void wolf::move()
{
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
