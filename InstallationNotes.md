## Mac OS X _(Tested on Mac OS X 10.6.3)_ ##

  * Download yimp-mac.zip.
  * Unzip.
  * Move all folders in "frameworks" folder to /Library/Frameworks. i.e. you should have /Library/Frameworks/QtCore.framework etc.
  * Run the yimp.app.


## Windows _(Tested on Windows XP SP3, Vista, and 7)_ ##

  * Download yimp-setup.exe.
  * Install and run it from Start menu.


## Linux _(Tested on Ubuntu 10.04 x86)_ ##
(it doesn't support x64 yet)

  * Give various distributions, no binary is provided; you need to compile it yourself.
  * Download yimp-src.tar.gz.
  * Install g++, make, and Qt development tools (at least 4.5) on your system. For example, in Ubuntu, run the command:
> > `sudo apt-get install g++ qt4-dev-tools libqt4-dev make`
  * Unpack the src tarball file.
  * Run the command inside the source directory:
> > `qmake && make`
  * The executable is in the `build` subdirectory.