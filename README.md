# ambf_refactor
## JUANSETUP
### START ASSETS FOLDER DOWNLOAD 
[Assets Google Drive](https://drive.google.com/drive/folders/1E6EIFbBrb6o7wJoxWUANeckHJCKVS_An?usp=sharing)

### GET VULKAN STUFF 
```
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
sudo apt update
sudo apt install vulkan-sdk
sudo apt install vulkan-validationlayers-dev spirv-tools
```

### DOUBLE CHECK VULKAN WORKS 
```
vkcube
vulkaninfo --summary
```

### GET SDL2 
```
sudo apt-get install libsdl2-dev
```

### COMPILE SHADERS
- download glslc at https://github.com/google/shaderc/blob/main/downloads.md
- copy to /usr/local/bin

(from build directory)
```
glslc ../shaders/gradient_color.comp --target-env=vulkan1.3 -O -o ../shaders/gradient_color.comp.spv
glslc ../shaders/sky.comp --target-env=vulkan1.3 -O -o ../shaders/sky.comp.spv
glslc ../shaders/pbr.frag --target-env=vulkan1.3 -O -o ../shaders/pbr.frag.spv 
glslc ../shaders/pbr.vert --target-env=vulkan1.3 -O -o ../shaders/pbr.vert.spv 
glslc ../shaders/post_process.vert --target-env=vulkan1.3 -O -o ../shaders/post_process.vert.spv
glslc ../shaders/post_process.frag --target-env=vulkan1.3 -O -o ../shaders/post_process.frag.spv
```

### GET ASSETS FOLDER
unzip and move assets folder into ambf_refactor directory

### RUN ENGINE
(from ambf_refactor directory)
```
mkdir build
cd build
cmake ..
make
./ambf_refactor
```
*from seperate terminal in build folder*
```
./ipc_test_client/ipc_test_client
```

