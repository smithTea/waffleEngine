# Building & Running

## Build

```bash
cmake -B cmake-build-debug
cmake --build cmake-build-debug --target OpenGLSkeleton
./cmake-build-debug/OpenGLSkeleton
```

## Notes

- Run from a directory where `../shaders/` resolves correctly.
- Shaders are loaded from disk at runtime.