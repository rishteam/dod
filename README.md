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

![My image](https://hackmd.io/_uploads/rJb_7onFWa.png)

![](https://hackmd.io/_uploads/rye_QihKb6.png)

![](https://hackmd.io/_uploads/Syg_XohFb6.png)

![](https://hackmd.io/_uploads/SydQj3FZT.png)

![](https://hackmd.io/_uploads/rkxOmi2tWa.png)

![](https://hackmd.io/_uploads/S1bumohtW6.png)

![](https://hackmd.io/_uploads/SyuXs3tW6.png)

![](https://hackmd.io/_uploads/B1d7j2tb6.png)

![](https://hackmd.io/_uploads/B1_mjnY-p.png)

![](https://hackmd.io/_uploads/HJ-u7shYZT.png)

![](https://hackmd.io/_uploads/rJedQohY-6.png)


## Reference

* [Game Engine series - Ther Cherno](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
