WINDOWS BUILD NOTES
===================

Used versions
----------------
    Processor
        3,4 GHz Intel Core i7
    OS
        Windows 7 Professional Service Pack 1
    OS type
        64-bit
    Software
        EvaCoin         v1.0
        Qt version      5.9.3
        OpenSSL         1.0.2m  2 Nov 2017
        berkeley-db@4   4.8.30 NC
        boost           1.65.1
        miniupnpc       2.0.20171102
        Qt Creator      4.5.0
        Qt              5.9.3 + MinGW 53_32
        
Compilers Supported
-------------------

Use the mingw installer to install MinGW and get MSYS 
   
    MSYS-> MinGW Developer Toolkit -> msys-perl
    MSYS-> MSYS System Builder -> msys-w32api

Dependencies
------------
Libraries you need to download separately and build:

    dependency      default path               download
    OpenSSL         \openssl-1.0.2n-mgw        https://www.openssl.org/source/openssl-1.0.2n.tar.gz
    OpenSSL         \openssl-1.0.2m-mgw        https://www.openssl.org/source/old/1.0.2/openssl-1.0.2m.tar.gz
    Berkeley DB     \db-4.8.30.NC-mgw          http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz
    Boost           \boost-1.65.1-mgw          https://sourceforge.net/projects/boost/files/boost/1.65.1/boost_1_65_1.tar.gz/download
    miniupnpc       \upnpc-exe-win32-20171102  http://miniupnp.tuxfamily.org/files/download.php?file=miniupnpc-2.0.20171102.tar.gz

Their licenses:

    wxWidgets      LGPL 2.1 with very liberal exceptions
    OpenSSL        Old BSD license with the problematic advertising requirement
    Berkeley DB    New BSD license with additional requirement that linked software must be free open source
    Boost          MIT-like license
    miniupnpc      New (3-clause) BSD license
   
Navigate to All Controll Panel Items -> System -> Advanced System Settings -> Environment Variables
Set the PATH variable to D:\MinGW\bin;C:\system32\cmd.exe
The output of
    
    mingw32-make --version
    
should be

    C:\Users\Admin>mingw32-make --version
    GNU Make 3.82.90
    Built for i686-pc-mingw32
    Copyright (C) 1988-2012 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    
in the MinGW shell and the windows terminal / dos prompt.

Untar the sources in an MSYS shell. Start D:\MinGW\msys\1.0\msys.bat

    tar xfz openssl-1.0.2m.tar.gz
    tar xfz openssl-1.0.2n.tar.gz
    tar xfz boost_1_65_1.tar.gz
    tar xfz db-4.8.30.NC.tar.gz
    tar xfz miniupnpc-1.9.20150917.tar.gz
    tar xfz miniupnpc-2.0.20171102.tar.gz
    
Notes
-----
The UI layout is edited with wxFormBuilder.  The project file is
uiproject.fbp.  It generates uibase.cpp and uibase.h, which define base
classes that do the rote work of constructing all the UI elements.

OpenSSL
-------
MSYS shell:
un-tar sources with MSYS 'tar xfz' to avoid issue with symlinks (OpenSSL ticket 2377)
change 'MAKE' env. variable from 'D:\MinGW32\bin\mingw32-make.exe' to '/d/MinGW32/bin/mingw32-make.exe'

    MAKE=/d/MinGW32/bin/mingw32-make.exe
    echo $MAKE
    cd /d/openssl-1.0.2m-mgw
    ./config
    make
    perl util/mkdef.pl crypto 32 libeay enable-static-engine > libeay32.def
    dllwrap --dllname libeay32.dll --output-lib libeay32.a --def libeay32.def libcrypto.a -lws2_32 -lgdi32

after that openssl libeay is in main source dir (openssl-1.0.0c-mgw)

Berkeley DB
-----------
MSYS shell:

    cd /c/db-4.8.30.NC-mgw/build_unix
    sh ../dist/configure --enable-mingw --enable-cxx
    make

Boost
-----
MSYS shell

    cd d/boost-1.65.1-mgw
    ./bootstrap.sh gcc
    ./b2 toolset=gcc --build-type=complete stage

This will build boost 1.65.1 with gcc-mingw-6.3.0, we would expect the boost library suffix to be -mgw63-mt-s-1_65_1
The meaning is: toolset -mgw63 enable multithreading -mt static linked library -s version -1_65_1
See http://www.boost.org/doc/libs/1_66_0/more/getting_started/windows.html#library-naming


MiniUPnPc
---------

MSYS shell

    cd /d/miniupnpc-2.0.20171102
    make -f Makefile.mingw
    strip upnpc-static.exe
    upx --best upnpc-static.exe
    strip upnpc-shared.exe
    upx --best upnpc-shared.exe


If building miniupnpc v2.0 fails, it is expected that a binary copy of v1.9 will be used.
See http://miniupnp.tuxfamily.org/forum/viewtopic.php?t=642
UPnP support is optional, make with USE_UPNP= to disable it.

Get upnpc-exe-win32-20150918.zip http://miniupnp.free.fr/files/download.php?file=upnpc-exe-win32-20150918.zip
and unzip it to \upnpc-exe-win32-20150918

Get miniupnpc-1.9.20150917.tar.gz http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.9.20150917.tar.gz
and copy *.h to \upnpc-exe-win32-20150918\miniupnpc

Building LevelDB On Windows
---------------------------

MSYS shell 

    cd /d/github/EvaCoin/src/leveldb
    TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a
    
EvaCoin
------- 

Open Qt project 'EvaCoin-qt-win.pro' in Qt Creator
    
    copy D:\GitHub\EvaCoin\build\*.moc D:\GitHub\EvaCoin\src\qt\*.moc 
    
and build for release.


Deploy
------

Copy EvaCoin.exe to a new 'deploy' folder
DOS shell

    set PATH=%PATH%;D:\Qt\5.9.3\mingw53_32\bin
    windeployqt D:\GitHub\EvaCoin\deploy\EvaCoin.exe

Compress the contents of the 'deploy' folder to EvaCoin.zip



https://wiki.qt.io/Deploy_an_Application_on_Windows

http://doc.qt.io/qt-5/windows-deployment.html
