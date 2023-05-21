REM installs the box2d dev dependencies
REM as a statically linked library plus headers

cd .

git clone https://github.com/erincatto/box2d.git
cd box2d
mkdir build
cd build
cmake -DBOX2D_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX="c:\lc\bloodot\packages\box2d" ..
cmake --build .
cmake --build . --target INSTALL