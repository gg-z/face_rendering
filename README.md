This project is the result visualizer(facial render) of the paper: Photo-Realistic Facial Details Synthesis from Single Image(ICCV 2019 Oral). Paper could be downloaded [here](https://arxiv.org/abs/1903.10873).

## Run executable
```
./exe/hmrenderer.exe  /root/to/obj /root/to/norm /root/to/shader
```

For example:
```
./exe/hmrenderer.exe ./assets/face.obj ./assets/nrm.png ./src/shaders
```

## build 
If you want to build the source code, you can follow the following introduction. All dependents and libs are included.

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

### Cmake build this project
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
