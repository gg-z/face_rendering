
### Dependency library
all dependent libs are included in folder **thirds**

### Project structure
```
├──  assets                         - assets data
│
├──  thirds                         - dependent libs
│
├──  build                      
│
├──  src                            - header files
│    └── includes                   - main
│    └── renderer_hm                - cpp/cc files
│    └── shaders                    - shders folder
│    └── CMakeLists.txt             - cmake list
│    └── depvars.h                  -  
│ 
├──  exe                            - windows executable file
│ 
├──  imgs                           - screenshots of this software 
│ 
├──  README.md  
│ 
├──  .gitignore                     
```

### Cmake build Visual Studio 2017 Project
```cpp
cd to the root of this project
mkdir build
cd build
cmake -A X64 -D CMAKE_PREFIX_PATH=/root/to/thirds ../src
```

### Run executable
```cpp
./hmrenderer.exe  /root/to/obj /root/to/norm /root/to/shader

eg: (unzip ./exe/face_rendering.rar file to get the executable file.)
./hmrenderer.exe ../assets/face.obj ../assets/nrm.png ../src/shaders/
```
