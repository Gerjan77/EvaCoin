# Build EvaCoin on MacOS High Sierra 10.13

Used versions
----------------
    Processor
        3,4 GHz Intel Core i7
    OS
        MacOS High Sierra 10.13
    OS type
        64-bit
    Software
        EvaCoin v1.0.0.0-g88ff655-beta
        Qt version 5.9.3
        OpenSSL OpenSSL 1.0.2m  2 Nov 2017
        
Set the build directory

    GIT=~/Documents/GitHub

Git
---
download Git Desktop https://desktop.github.com
clone the repository https://github.com/Gerjan77/EvaCoin

Brew
------
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

Boost library
--------------
brew install boost

edit the BOOST_INCLUDE_PATH and BOOST_LIB_PATH in EvaCoin.pro

    BOOST_LIB_PATH = /usr/local/opt/boost/lib
    BOOST_INCLUDE_PATH = /usr/local/opt/boost/include

edit the location of Boost libraries in src/makefile.osx

    line 12 BOOSTDIR=/usr/local/opt/boost
    

Openssl
---------
brew install openssl
edit the OPENSSL_INCLUDE_PATH and OPENSSL_LIB_PATH in EvaCoin.pro

    OPENSSL_LIB_PATH = /usr/local/opt/openssl/lib
    OPENSSL_INCLUDE_PATH = /usr/local/opt/openssl/include
edit the location of openssl in src/makefile.osx

    line 11 SSLDIR=/usr/local/opt/openssl

BerkeleyDb
-------------
brew search berkeley-db
brew install berkeley-db@4
edit the library and include paths in EvaCoin.pro

    BDB_LIB_PATH = /usr/local/opt/berkeley-db@4/lib
    BDB_INCLUDE_PATH = /usr/local/opt/berkeley-db@4/include
edit the location of BerkeleyDb in src/makefile.osx

    line 13 BERKELEYDIR=/usr/local/opt/berkeley-db@4

Check the versions:

    brew ls --versions

Libminiupnpc:
-------------
    cd ~/Documents/GitHub/EvaCoin/dependencies/miniupnpc-1.6
    sudo su
    INSTALLPREFIX=/opt/local make install
    exit
    
edit the library and include paths in EvaCoin.pro

    MINIUPNPC_INCLUDE_PATH = /opt/local/include
    MINIUPNPC_LIB_PATH = /opt/local/lib

Qt 5:
------
Download Qt 5.9.3 from https://download.qt.io/archive/qt/5.9/5.9.3/ and Qt Creator 2.4.0 from https://download.qt.io/archive/qtcreator/2.4/ Open EvaCoin.pro Go to Projects -> Build Settings -> Debug select Qt5.9.3 , Clang (86 64 bit)



EvaCoin.app
-----------------
Compile EvaCoin-mac.pro in Qt5.9.3

    
Deploy
-----------------------
edit the Info.plist not with Xcode, but with a plist editor or iHex

    ~/Documents/GitHub/EvaCoin/Share/qt/Info.plist

Read the deploy tool help

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt -h

Deploy the bundle

    ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app
    
with the deploy deploy tool

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app

Deploy MiniUPnPC

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app -libpath=/opt/local/lib -verbose=3
    
Deploy Openssl

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app -libpath=/usr/local/opt/openssl/lib -verbose=3
    
Deploy BerkeleyDB

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app -libpath=/usr/local/opt/berkeley-db@4/lib -verbose=3
    
Deploy the Boost library

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app -libpath=/usr/local/opt/boost/lib -verbose=3
    
 Codesign the Qt frameworks
 
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app/Contents/Frameworks/QtCore.framework
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app/Contents/Frameworks/QtGui.framework
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app/Contents/Frameworks/QtNetwork.framework
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app/Contents/Frameworks/QtPrintSupport.framework
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app/Contents/Frameworks/QtSvg.framework
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app/Contents/Frameworks/QtWidgets.framework
 
 Remove all symbolic links from the app bundle's root and codesign the app bundle
 
     codesign -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app

Make sure ˜/Documents/GitHub/EvaCoin/EvaCoin.app/Contents/Resources/qt.conf contains the following lines:

     [Paths]
       Plugins = PlugIns
    
Create a .dmg disk image

    ~/Qt5.9.3/5.9.3/clang_64/bin/macdeployqt ~/Documents/GitHub/EvaCoin/EvaCoin/EvaCoin.app -dmg -verbose=3

Or by disk utility:
Use Disk Utility to create a new empty sparse bundle disk image
Double-click the image to open it
Copy your app into the image
Make a link to /Applications in the image
Hide the toolbar/sidebar/etc. as desired
Using View Options set to always open in icon view
Also add a background image if you like using View Options. Background images can contain text such as "Drag App to Application", or arrows. Layout the app and /Applications icons to match your background image.
In another Finder window eject the sparse bundle.
In Disk Utility use Images -> Convert... to convert the sparse bundle to a read-only DMG
    
Sign with the apple developer identity in your keychain

    codesign -f -s "3rd Party Mac Developer Application: G.J.A. Uijtdewilligen (CK92ZX6P5T)" -i "com.goodjobunit.EvaCoin" -v EvaCoin.dmg
    
Check the signature

    codesign -v --verbose=4 --display EvaCoin.dmg


Location of Blockchain and wallet
--------------------------------------
    ls -l -a ~/Library/"Application Support"/EvaCoin



Testing to do
---------------

run the .dmg on a fresh install of MacOS 10.10 Yosemite on the apple account of my beta tester


