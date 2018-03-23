# Binarization

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/dbfb9289ceeb4900af253ea6176e0a76)](https://www.codacy.com/app/Samir55/Binarization?utm_source=github.com&utm_medium=referral&utm_content=Samir55/Binarization&utm_campaign=badger)

This is a working in progress project. The Implementation is based on "Adaptive degraded document image binarization" paper by  B. Gatos, I. Pratikakis, and S.J. Perantonis found in [this link](http://users.iit.demokritos.gr/~bgat/PatRec2006.pdf).

The steps for Adaptive binarization are:
* Apply Wiener filter woth window size 3x3. (DONE)
* Apply Sauvola adaptive thresolding using k = 0.2.(DONE)
* Background surface estimation. (TODO)
* Final thresholding. (TODO)
* Upsampling. (TODO)

# Dependencies
* ImageMagick: 6.7.7 or newer **but less than 7.0**.
* OpenCV: 3.0 or newer.

# Build
```Console
git clone https://github.com/Samir55/Binarization.git
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make
```
# Usage
As it's a working in progress project, currently modify the image path in main.cpp then build and run
```Console
./pre_processing
```
the output will be in the 'repository_directory/output' under the name 'paper.jpg'. you can also see the output of the weiner filter under the name 'weiner.ppm' in the same folder.
