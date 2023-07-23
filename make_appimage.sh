mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR=AppDir

cp ../AppRun AppDir/AppRun
chmod +x AppDir/AppRun
cp ../DisasterLauncher.desktop AppDir/DisasterLauncher.desktop
cp ../DisasterLauncher.png AppDir/DisasterLauncher.png

if ! test -f "linuxdeploy-x86_64.AppImage"; then
    wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

if ! test -f "appimagetool-x86_64.AppImage"; then
    wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
    chmod +x appimagetool-x86_64.AppImage
fi

./linuxdeploy-x86_64.AppImage --appdir=AppDir/
./appimagetool-x86_64.AppImage AppDir/ DisasterLauncher-x86_64.AppImage
zip -r DisasterLauncher-x86_64.zip DisasterLauncher-x86_64.AppImage assets/