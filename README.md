# dod

## Clone

```bash
git clone --recursive https://github.com/rishteam/dod.git
```

## Build

* Dependencies
  * SFML 2.5.1
  * ImGui-SFML
    * Built with imgui docking branch
  * fmt
  * nlohmann-json
  * spdlog
  * glm

```bash
pacman -S - < package.txt
```

* Build [imgui-SFML](https://github.com/eliasdaler/imgui-sfml) with [imgui](https://github.com/ocornut/imgui) docking branch
    * `git clone -b docking https://github.com/ocornut/imgui.git imgui`
    * `git clone https://github.com/eliasdaler/imgui-sfml.git imgui-sfml && mkdir -p imgui-sfml/build && cd imgui-sfml/build`
    * `cmake -G "MSYS Makefiles" .. -DCMAKE_INSTALL_PREFIX=C:/msys64/mingw32 -DIMGUI_DIR=$(pwd)/../../imgui`
    * `make -j8 && make install`

* Environment
  * msys2
  * cmake

```bash
./scripts/build.sh full run
```

* Generate documentation
  * `make doxygen-docs`


## Demo
![B03 - RishEngine01](https://github.com/rishteam/dod/assets/37583122/c1facd08-5026-4dfd-9617-ab52372c47d8)

![B03 - RishEngine02](https://github.com/rishteam/dod/assets/37583122/317e669e-4c52-40b6-9050-d01d59393c13)

![B03 - RishEngine03](https://github.com/rishteam/dod/assets/37583122/a61e54ba-c009-4071-ae04-0cb1f41a9059)

![B03 - RishEngine04](https://github.com/rishteam/dod/assets/37583122/e9916065-7a4b-4076-8909-30ea31db2913)

![B03 - RishEngine05](https://github.com/rishteam/dod/assets/37583122/b6671e4b-ae68-477b-af50-fa621c936a09)

![B03 - RishEngine06](https://github.com/rishteam/dod/assets/37583122/9f300ea9-213c-4046-9258-b80601fccbc9)

![B03 - RishEngine0](https://github.com/rishteam/dod/assets/37583122/1a94c1ff-0260-4510-b35b-7acbb34fc71c)

![B03 - RishEngine08](https://github.com/rishteam/dod/assets/37583122/6b325b6d-202d-48ae-a411-b1fe91af7bd9)

![09](https://github.com/rishteam/dod/assets/37583122/c28d0c89-2d8c-4e51-a779-63d64a6a4068)

![B03 - RishEngine10](https://github.com/rishteam/dod/assets/37583122/0bff8233-c80d-4f23-9af1-0653853c5cdd)

![B03 - RishEngine11](https://github.com/rishteam/dod/assets/37583122/8a6bad12-e8b6-4280-ab18-678dffe8225c)

## Reference

* [Game Engine series - Ther Cherno](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
