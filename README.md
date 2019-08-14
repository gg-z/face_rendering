### Project description
This project face render of **ICCV 2019 Oral paper:** __Photo-Realistic Facial Details Synthesis from Single Image__ [[1]](https://arxiv.org/abs/1903.10873)

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
├──  imgs                            - screens about this software 
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
```

### Some screenshots 

<div align="left">
<img src="https://github.com/gg-z/face_rendering/blob/master/imgs/screen_shot1.png" width = "400" height = "400" alt="demo_0" align=center />
<img src="https://github.com/gg-z/face_rendering/blob/master/imgs/screen_shot1.png" width = "400" height = "400" alt="demo_1" align=center /> 
</div>