#pragma once

// High-level representation of user input. Should be used in business logic
// in class like GameWorld.
struct UserInputData {
  struct Buttons {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
  };

  // State: is the key currently held down?
  Buttons held;

  // Impulses: was the key pressed this frame?
  Buttons pressed;
};
