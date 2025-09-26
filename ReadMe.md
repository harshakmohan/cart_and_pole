# Cart-Pole MuJoCo Simulation

Minimal C++ implementation of cart-pole using MuJoCo physics engine.

## Setup

1. Install dependencies:
```bash
brew install cmake glfw
```

2. Install MuJoCo (macOS):
   - Download and install MuJoCo.app from: https://github.com/google-deepmind/mujoco/releases
   - The build system will automatically find it at `/Applications/MuJoCo.app`

3. Build:
```bash
mkdir build && cd build
cmake ..
make
```

## Run

- `./cartpole` - Interactive demo (use arrow keys)
- `./test_env` - Test RL environment wrapper

## Files

- `src/main.cpp` - Interactive simulation
- `src/cartpole_env.cpp` - RL environment interface
- `mujoco/cartpole.xml` - MuJoCo model definition

