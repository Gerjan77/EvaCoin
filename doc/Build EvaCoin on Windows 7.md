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
        
Compilers Supported
-------------------
TDM-GCC with gcc 4.5.1 was used to build this release.
MSYS 1.0.11 was also used (sh needed to compile some dependencies)

    MinGW GCC  https://sourceforge.net/projects/tdm-gcc/files/TDM-GCC%20Installer/Previous/1.1006.0/tdm64-gcc-4.5.1.exe/download
uncheck updates when installing, and use the mingw installer to get 
   
    MSYS-> MinGW Developer Toolkit -> msys-perl
    MSYS-> MSYS System Builder -> msys-w32api

Candidate releases were built with MSVC 10.0 (2010), but
compiling with Visual C++ caused rendering artifacts when
bitcoin was run.


Dependencies
------------
Libraries you need to download separately and build:

    dependency      default path               download
    wxWidgets       \wxwidgets-2.9.1-mgw       https://sourceforge.net/projects/wxwindows/files/2.9.1/wxWidgets-2.9.1.zip/download
    OpenSSL         \openssl-1.0.0c-mgw        https://www.openssl.org/source/old/1.0.0/openssl-1.0.0c.tar.gz
    Berkeley DB     \db-4.7.25.NC-mgw          http://download.oracle.com/berkeley-db/db-4.7.25.NC.tar.gz
    Boost           \boost-1.43.0-mgw          https://sourceforge.net/projects/boost/files/boost/1.43.0/boost_1_43_0.zip/download
    Boost jam       \boost-jam-3.1.18          https://sourceforge.net/projects/boost/files/boost-jam/3.1.18/boost-jam-3.1.18.zip/download
    miniupnpc       \upnpc-exe-win32-20110215  http://miniupnp.tuxfamily.org/files/

Their licenses:

    wxWidgets      LGPL 2.1 with very liberal exceptions
    OpenSSL        Old BSD license with the problematic advertising requirement
    Berkeley DB    New BSD license with additional requirement that linked software must be free open source
    Boost          MIT-like license
    miniupnpc      New (3-clause) BSD license

Versions used in this release:

    wxWidgets    2.9.1
    OpenSSL      1.0.0c
    Berkeley DB  4.7.25.NC
    Boost        1.43.0
    Boost jam    3.1.18
    miniupnpc    1.5-20110215
    
The output of
    
    mingw32-make --version
    
should be

    GNU Make 3.82
    Built for i386-pc-mingw32
    Copyright (C) 2010  Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    
in the MinGW shell and the windows terminal / dos prompt.

Notes
-----
The UI layout is edited with wxFormBuilder.  The project file is
uiproject.fbp.  It generates uibase.cpp and uibase.h, which define base
classes that do the rote work of constructing all the UI elements.

wxWidgets
---------
DOS shell:
    
    cd \wxWidgets-2.9.1-mgw\build\msw
    mingw32-make -f makefile.gcc

OpenSSL
-------
MSYS shell:
un-tar sources with MSYS 'tar xfz' to avoid issue with symlinks (OpenSSL ticket 2377)
change 'MAKE' env. variable from 'D:\MinGW32\bin\mingw32-make.exe' to '/d/MinGW32/bin/mingw32-make.exe'

     MAKE=/d/MinGW32/bin/mingw32-make.exe
     echo $MAKE

     cd /d/openssl-1.0.0c-mgw
     ./config
     make

     perl util/mkdef.pl crypto 32 libeay enable-static-engine > libeay32.def
     dllwrap --dllname libeay32.dll --output-lib libeay32.a --def libeay32.def libcrypto.a -lws2_32 -lgdi32

after that openssl libeay is in main source dir (openssl-1.0.0c-mgw)

Berkeley DB
-----------
MSYS shell:

     cd /c/db-4.7.25.NC-mgw/build_unix
     sh ../dist/configure --enable-mingw --enable-cxx
     make

Boost
-----
MSYS shell with mingw 6.2, newest version of the compiler:

     cd d/boost-1.66.0-mgw
     ./bootstrap.sh gcc
     ./b2 toolset=gcc --build-type=complete stage


MiniUPnPc
---------
Building miniupnpc failed on Windows Server 2003, thus it is expected that a binary copy will be used.
See http://miniupnp.tuxfamily.org/forum/viewtopic.php?t=642
UPnP support is optional, make with USE_UPNP= to disable it.

Get upnpc-exe-win32-20110215.zip 

http://miniupnp.free.fr/files/download.php?file=upnpc-exe-win32-20110215.zip

and unzip it to \upnpc-exe-win32-20110215

Get miniupnpc-1.5.20110215.tar.gz 

http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.5.20110215.tar.gz

and copy *.h to \upnpc-exe-win32-20110215\miniupnpc

Building LevelDB On Windows
---------------------------

Method 1: MSYS shell (recommended)

    cd /C/bitcoin-0.8.6/src/leveldb
    TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a
    
Two other methods will be listed in the footnotes.
     
Bitcoin
-------

DOS prompt:
   
    cd \bitcoin\src
    mingw32-make -f makefile.mingw
    strip EvaCoind.exe



FootNotes, Alternative methods for building leveldb.

Method 2: Microsoft Visual C++ 2010 Express

Download the installer vc_web.exe 3,17 MB sha1: c2d61a5424aa474a169e59199aa2158a858833f6 
from https://filehippo.com/download_visualc_2010_express_edition/ or soft32
Install and include SQL Server Express 2008 Service Pack 1 

On the first run, the create project wizard fails with error 
 
    Error: Visual studio 2010 Could not find a part of the path 
    D:\Program Files\Microsoft Visual Studio 10.0\VC\VCWizards\default.vcxproj
    
This is solved by creating the path yourself and put an empty project file in it.

INSTRUCTIONS FOR BUILDING LEVELDB ON WINDOWS / MSVC++
(Tested with Microsoft Visual C++ 2010 Express)

1. Install Boost
  You'll need the Boost libraries to compile LevelDB on Windows:
  http://www.boost.org/users/download/
  You'll need at least the following Boost packages:
  - date_time
  - filesystem
  - thread
  - interprocess

2. To create your LevelDB project, choose:
   File -> New -> "Project From Existing Code"
   and point Visual Studio to the leveldb root directory.

3. To build the tools, choose:
   Project Type: "Console application project"

4. Click Next, Next, In the configuration settings make sure you include
   Preprocessor Definitions (/D) : LEVELDB_PLATFORM_WINDOWS;OS_WIN
   
   You can also add these later in:
   Project -> Properties ->
      Configuration Properties -> C/C++ -> Preprocessor Definitions

   Include the root directory of your LevelDB sources and current directory in header search paths.
   Include Search Paths (/I) : D:\bitcoin\src\leveldb\include;.
   
   You can also add this later in:
   Project -> Properties -> 
      Configuration Properties -> C/C++ -> Additional Include Directories

5. Add boost/lib directory to Linker paths:
   Project -> Properties -> Linker -> General -> Additional Library Directories
   D:\boost-1.43.0-mgw\libs
   
   Project Properties -> C++ -> Precompiled Headers -> set Precompiled Header to "Not Using Precompiled Headers".

6. Manually exclude the following files from the build
   (Solution Explorer -> right-click on file -> Exclude from Project)
   - port/port_android.cc
   - port/port_posix.cc
   - util/env_chromium.cc
   - util/env_posix.cc

7. Manually exclude all the *_test.cc and *_bench.cc files you don't want
to build. There should only be one .cc file with a main() in your project.

8. Compile and build

Method 3: snappy

    regedit 4.7.02053 -> 4.0.30319
    HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\NET Framework Setup\NDP\v4\Client\Version
    HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\NET Framework Setup\NDP\v4\Full\Version
    uninstall
    "Microsoft Visual C++ 2010 x64 Redistributable - 10.0.40219"
    "Microsoft Visual C++ 2010 x86 Redistributable - 10.0.40219"

Update your Windows, download the Windows SDK for windows 7 and .NET framework 4 (version 7.1) 

https://www.microsoft.com/en-us/download/details.aspx?id=8442

    Platform      File Name               Size        SHA1
    x86           GRMSDK_EN_DVD.iso       567.3 MB    0xCDE254E83677C34C8FD509D6B733C32002FE3572
    x64           GRMSDKIAI_EN_DVD.iso    569.4 MB    0x9203529F5F70D556A60C37F118A95214E6D10B5A
    Itanium       GRMSDKX_EN_DVD.iso      570.9 MB    0x2093EE439193EF79A1629FD9A826CE8D4DE9A93D

Or Windows Software Development Kit for Windows 7 and Windows Server 2008 R2 (x64) - DVD (English) from
https://my.visualstudio.com/downloads

Download and extract the [Snappy source distribution]
http://pkgs.fedoraproject.org/repo/pkgs/snappy/snappy-1.0.5.tar.gz/4c0af044e654f5983f4acbf00d1ac236/snappy-1.0.5.tar.gz

1. Open the "Windows SDK 7.1 Command Prompt" :
   Start Menu -> "Microsoft Windows SDK v7.1" > "Windows SDK 7.1 Command Prompt"
2. Change the directory to the leveldb project

Building the Static lib

* 32 bit Version
        setenv /x86
        msbuild.exe /p:Configuration=Release /p:Platform=Win32 /p:Snappy=..\snappy-1.0.5    
* 64 bit Version
        setenv /x64
        msbuild.exe /p:Configuration=Release /p:Platform=x64 /p:Snappy=..\snappy-1.0.5
        
Building and Running the Benchmark app

* 32 bit Version
            setenv /x86
            msbuild.exe /p:Configuration=Benchmark /p:Platform=Win32 /p:Snappy=..\snappy-1.0.5
            Benchmark\leveldb.exe
* 64 bit Version
            setenv /x64
            msbuild.exe /p:Configuration=Benchmark /p:Platform=x64 /p:Snappy=..\snappy-1.0.5
            x64\Benchmark\leveldb.exe
	    
	    
	    
	    
	    
	    
	    
	    




WINDOWS BUILD NOTES
===================

See [readme-qt.md](readme-qt.md) for instructions on building EvaCoin, the
graphical user interface.

Compilers Supported
-------------------
    GCC              4.9 +


Dependencies
------------
Libraries you need to download separately and build:

	name            default path               download
	--------------------------------------------------------------------------------------------------------------------
	OpenSSL         \openssl-1.0.1c-mgw        http://www.openssl.org/source/
	Berkeley DB     \db-4.8.30.NC-mgw          http://www.oracle.com/technology/software/products/berkeley-db/index.html
	Boost           \boost-1.50.0-mgw          http://www.boost.org/users/download/
	MiniUPnP Client \miniupnpc-1.6-mgw         http://miniupnp.tuxfamily.org/files/

Their licenses:

	OpenSSL        Old BSD license with the problematic advertising requirement
	Berkeley DB    New BSD license with additional requirement that linked software must be free open source
	Boost          MIT-like license
	miniupnpc      New (3-clause) BSD license

Versions used in this release:

	OpenSSL      1.0.1c
	Berkeley DB  4.8.30.NC
	Boost        1.50.0
	miniupnpc    1.6


OpenSSL
-------
MSYS shell:

un-tar sources with MSYS 'tar xfz' to avoid issue with symlinks (OpenSSL ticket 2377)
change 'MAKE' env. variable from 'C:\MinGW32\bin\mingw32-make.exe' to '/c/MinGW32/bin/mingw32-make.exe'

	cd /c/openssl-1.0.1c-mgw
	./config
	make

Berkeley DB
-----------
MSYS shell:

	cd /c/db-4.8.30.NC-mgw/build_unix
	sh ../dist/configure --enable-mingw --enable-cxx
	make

Boost
-----
DOS prompt:

	downloaded boost jam 3.1.18
	cd \boost-1.50.0-mgw
	bjam toolset=gcc --build-type=complete stage

MiniUPnPc
---------
UPnP support is optional, make with `USE_UPNP=` to disable it.

MSYS shell:

	cd /c/miniupnpc-1.6-mgw
	make -f Makefile.mingw
	mkdir miniupnpc
	cp *.h miniupnpc/

EvaCoin
-------
DOS prompt:

	cd \EvaCoin\src
	mingw32-make -f makefile.mingw
	strip EvaCoind.exe
