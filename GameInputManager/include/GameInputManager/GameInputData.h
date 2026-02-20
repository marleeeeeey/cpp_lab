#pragma once

// High-level representation of user input
struct GameInputData {
  struct Buttons {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
  };

  // State: is the key currently held down?
  Buttons held;

  // Impulses: was the key pressed in this frame?
  Buttons pressed;
};
