@echo off
"C:\\Program Files\\Java\\jdk-21\\bin\\java" ^
  --class-path ^
  "C:\\Users\\simoneru\\.gradle\\caches\\modules-2\\files-2.1\\com.google.prefab\\cli\\2.1.0\\aa32fec809c44fa531f01dcfb739b5b3304d3050\\cli-2.1.0-all.jar" ^
  com.google.prefab.cli.AppKt ^
  --build-system ^
  cmake ^
  --platform ^
  android ^
  --abi ^
  arm64-v8a ^
  --os-version ^
  26 ^
  --stl ^
  c++_static ^
  --ndk-version ^
  26 ^
  --output ^
  "C:\\Users\\simoneru\\AppData\\Local\\Temp\\agp-prefab-staging10342858685796956860\\staged-cli-output" ^
  "C:\\Users\\simoneru\\.gradle\\caches\\8.12\\transforms\\001c4dbd8e820adf7d94e5a0828691ee\\transformed\\openxr_loader_for_android-1.1.46\\prefab"
