# Path tracer

```
git clone --recursive https://github.com/ljghy/path-tracing.git
```

## Build and run

```
mkdir build
cd build
```

#### Windows MSVC

```
cmake ..
cmake --build . --config [Debug|Release]
..\bin\[Debug|Release]\path_tracer.exe ..\res\Scene.json ..\res\RenderConfig.json
```

#### Windows MinGW-w64

```
cmake -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=[Debug|Release] ..
mingw32-make
..\bin\path_tracer.exe ..\res\Scene.json ..\res\RenderConfig.json
```

#### Linux GCC

```
cmake -DCMAKE_BUILD_TYPE=[Debug|Release] ..
make 
../bin/path_tracer ../res/Scene.json ../res/RenderConfig.json
```

![640x640, 128spp](https://github.com/ljghy/path-tracing/blob/master/results/cornell_box_640x640_128spp.png)
