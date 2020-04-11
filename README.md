# xglobe

__XGlobe__ is the further development of Thorsten Scheuermann' xglobe 0.5
release. It started as a simple port from Qt3 to Qt5. For more informations
about the old xglobe, please read the original readme [here](README.xglobe-0.5)

## CMake build option:

- Set default map file.
  - `-DSET_DEFAULT_MAP="/usr/local/share/xglobe/map.png"`
- Set default night map file.
  - `-DSET_DEFAULT_MAP_NIGHT="/usr/local/share/xglobe/night.png"`
- Set default back map file.
  - `-DSET_DEFAULT_MAP_BACK="/usr/local/share/xglobe/back.png"`
- Set default marker file.
  - `-DSET_DEFAULT_MARKER_FILE="/usr/local/share/xglobe/marker.txt"`

## Dependencies
 + C++ 17 Compiler
 + CMake 3.7
 + Qt5
 + [xwallpaper](https://github.com/stoeckmann/xwallpaper)
