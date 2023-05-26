REM installs the box2d release dependencies
REM as a statically linked library plus headers

cd .

git clone https://github.com/erincatto/box2d.git
cd box2d
mkdir build
cd build
cmake -DCMAKE_CONFIGURATION_TYPES=Release -DBOX2D_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX="c:\lc\bloodot\packages\box2d_release" ..
cmake --build . --config Release
cmake --build . --target INSTALL --config Release
