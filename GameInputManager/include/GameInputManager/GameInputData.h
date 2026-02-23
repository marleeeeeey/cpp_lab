#pragma once

// High-level representation of user input
struct GameInputData {
  // ----------
  // Keyboard
  // ----------

  struct KeyboardState {
    struct Buttons {
      bool up = false;
      bool down = false;
      bool left = false;
      bool right = false;
    };

    Buttons held;
    Buttons pressed;
    Buttons released;
  };

  KeyboardState keyboard;

  // -------
  // Mouse
  // -------

  struct MouseButtons {
    bool left = false;
    bool right = false;
    bool middle = false;
    bool x1 = false;
    bool x2 = false;
  };

  struct MouseState {
    // Position in window coordinates (pixels)
    int winX = 0;
    int winY = 0;

    // Position in screen coordinates (pixels)
    int screenX = 0;
    int screenY = 0;

    // Delta since last frame (pixels)
    int dx = 0;
    int dy = 0;

    // Wheel impulses for this frame (commonly -1/0/1, but can be larger)
    float wheelX = 0.0f;
    float wheelY = 0.0f;

    // Buttons: held + impulses
    MouseButtons held;
    MouseButtons pressed;
    MouseButtons released;
  };

  MouseState mouse;
};
