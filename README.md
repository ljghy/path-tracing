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

![960x720, 800spp](https://github.com/ljghy/path-tracing/blob/master/results/result_960x720_800spp.png)
