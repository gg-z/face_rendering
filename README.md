This project is the face render of Photo-Realistic Facial Details Synthesis from Single Image(ICCV 2019 Oral)
### Dependency library

## Run executable
```cpp
./exe/hmrenderer.exe  /root/to/obj /root/to/norm /root/to/shader

example: ./
```

## build 
If you want to build the source code, you can follow the following introduce. All dependents and libs are included.

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
cmake -A X64 -D CMAKE_PREFIX_PATH=../thirds ../src
```



### Some screenshots 

<div align="left">
<img src="https://github.com/gg-z/face_rendering/blob/master/imgs/left.png" width = "430" height = "430" alt="demo_0" align=center />
<img src="https://github.com/gg-z/face_rendering/blob/master/imgs/right.png" width = "430" height = "430" alt="demo_1" align=center /> 
</div>