mkdir build
cd build
export CC=/usr/bin/clang
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR=AppDir

cp ../AppRun AppDir/AppRun
chmod +x AppDir/AppRun
cp ../DisasterLauncher.desktop AppDir/DisasterLauncher.desktop
cp ../DisasterLauncher.png AppDir/DisasterLauncher.png

linuxdeploy --appdir=AppDir/
appimagetool AppDir/ DisasterLauncher-x86_64.AppImage
zip -r DisasterLauncher-x86_64.zip DisasterLauncher-x86_64.AppImage assets/
