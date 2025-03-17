# Template for build MRE .vxp apps with CMake

## Folder structure

|Folder|Description|
|-|-|
|.vscode|VS Code cmake Tools config files|
|cmake|Build config files|
|platform|Some source files to bridge the gap between the MRE SDK and the Toolchain|
|resources|Place resources you want to bundle with your app here|
|src|Main code for app|Static liblary|

## Requirements to build

- [cmake](https://cmake.org/download/) (>=3.25)
- [Ninja](https://github.com/ninja-build/ninja)
- [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [MRE SDK](https://archive.org/details/mre-sdk_developer) (for includes, libs and MoDis)
- [TinyMRESDK](https://github.com/XimikBoda/TinyMRESDK) (For resourse pack and app pack)

## How to build?
### For phone

```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm-none-eabi.cmake "-DTOOLCHAIN_PREFIX=C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.2 rel1" "-DMRE_SDK=C:/Program Files (x86)/MRE SDK V3.0.00" -DTinyMRESDK=path/to/tiny/mre/sdk -DCMAKE_BUILD_TYPE=Release -DVXP_DEV_SIGN=C:\path\to\vxp-dev-sign\vxp-dev-sign.exe -G "Ninja" ..
ninja
```

### For MoDis simulator (only on Windows)

```
mkdir vs
cd vs
cmake -DMRE_SDK=path/to/mre/sdk -DTinyMRESDK=path/to/tiny/mre/sdk -G "Visual Studio 17 2022" -A Win32 ..
Demo.sln
```

Also you can use cmake gui for config (Don`t forget to set MRE_SDK and TinyMRESDK variables)

## How to run

**Note** You can't run `.vxp` on MoDis and `.vc.vxp` on phone. 
### On phone

**Note**: If you use Nokia phone, you need to add your IMSI (```-DIMSI="91234567890"```) with 9 at begin (because of mre api bug)

Just copy `build/vxp/vxp.vxp` to sd-card

### On MoDis simulator (only on Windows)

You can use `run_in_MoDis` target (right click and `Set as Startup Project`), but if you want to debug it, manualy change *Command* (in project properties) to MoDis.exe.

Or you can manualy copy `vs/vxp/vxp.vc.vxp` to MoDis.

## How to create own project

- Copy this to new folder
- Change app/developer name and other settings in base `CMakeList.txt`
- Update paths in CMakePresets.json, .vscode/settings.json, .vscode/c_cpp_properties.json and .vscode/cmake-kits.json to match your installation locations
- If you want to add CMake liblary, add it as static lib (you can use `set(BUILD_SHARED_LIBS FALSE)`)

## Credits

Based on [XimikBoda/CmakeMreTemplate](https://github.com/XimikBoda/CmakeMreTemplate)
