# OpenXM, OpenXM contrib2
OpenXM package including Risa/Asir computer algebra system.

* [OpenXM project](http://www.openxm.org). Binaries are distributed from this project page.

* Prerequisite for building from the source codes on Debian:
nkf, wget, texinfo, java-common, openjdk-11-jdk, gnupg, latex2html, evince, dpkg-dev, debhelper, fakeroot, texi2html, dvipdfmx, gcc, freeglut3, freeglut3-dev, make, libxaw7, libxaw7-dev, bison, libtinfo-dev, libtinfo5, sharutils, build-essential

* Building the package from the source codes.
```
# Clone the source codes
git clone https://github.com/openxm-org/OpenXM
git clone https://github.com/openxm-org/OpenXM_contrib2

# Building the binary (you do not need to be the root).
# Binaries are installed under OpenXM/bin
cd OpenXM/src
make configure
make install

# Generating openxm script which contains the path information for binaries, libraries.
cd ../rc
make
mkdir ~/bin ; cp openxm ~/bin

# Test if you can start the Risa/Asir
openxm asir
```
