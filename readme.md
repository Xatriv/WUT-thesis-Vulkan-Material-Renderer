# Real-time Skin Rendering Demo
The following renderer demonstrates several skin rendering techniques: texturing, PBR, normal mapping and approximated subsurface scattering. Application setting are listed in `config.json` file, where one may choose different models, textures, shaders and more.

## Building the application
The project requires C++ standard of at least 17. The project relies on make for building the project. There are also six external dependencies, three of which are already present in the `include` folder. Other dependencies should be provided in a different way:

### GLSLC
Can be downloaded from [Google's unofficial binaries](https://github.com/google/shaderc/blob/main/downloads.md). The project's makefile expects them to be located at `/usr/local/bin`. They may be placed elsewhere, but it is necessary to modify the makefile first.

### GLFW
Can be installed through apt (`sudo apt install libglfw3-dev`), dnf (`sudo dnf install glfw-devel`) or pacman (`sudo pacman -S glfw-wayland` or `glfw-x11` for X11 users)

### GLM
Can be installed through apt (`sudo apt install libglm-dev`), dnf (`sudo dnf install glm-devel`) or pacman (`sudo pacman -S glm`) 

## User manual
First, all the assets will be loaded. Information about the models will be printed to the console. Right after, a window with the renderer will pop up, and it will immediately consume the cursor. User can move using standard `WSADQE` keyboard movement and the mouse for free-look. User can also move the light around, by pressing `2` and then `WSADQE`. To return to camera movement mode, user can simply press `1` key. If the user wishes to close the application, they can just press `ESC` key. Afterward, the average number of frames per second will be printed to the console.