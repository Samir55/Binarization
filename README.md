# Binarization

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/dbfb9289ceeb4900af253ea6176e0a76)](https://www.codacy.com/app/Samir55/Binarization?utm_source=github.com&utm_medium=referral&utm_content=Samir55/Binarization&utm_campaign=badger)

This is a working in progress project. The Implementation is based on "Adaptive degraded document image binarization" paper by  B. Gatos, I. Pratikakis, and S.J. Perantonis found in [this link](http://users.iit.demokritos.gr/~bgat/PatRec2006.pdf).

The steps for Adaptive binarization are:
* Apply Wiener filter woth window size 3x3. (DONE)
* Apply Sauvola adaptive thresolding using k = 0.2.(DONE)
* Background surface estimation. (TODO)
* Final thresholding. (TODO)
* Upsampling. (TODO)
