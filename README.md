# AR
Acquisition Reconstruction

Project in C++ allowing the user to visualize images contained in a DICOM file. This project allows us to visualize CT-Scan files in 2D and 3D using the DCMTK API and Qt as the user interface.

Build Project : 

```
mkdir build && cd build && qmake --qt=qt5 .. && make
```

Interface :

![](https://raw.githubusercontent.com/carl-221b/AR/main/screens/empty_window.png)


Opening CT-Scan File : 

![](https://raw.githubusercontent.com/carl-221b/AR/main/screens/default.png)

Selecting Layer feature :

![](https://raw.githubusercontent.com/carl-221b/AR/main/screens/hide_layer.png)

Frustum view 3D :
![](https://raw.githubusercontent.com/carl-221b/AR/main/screens/frustum_projection.png)


Authors : 
- Arthur Mondon (arthurjm)
- Antonin Ayot (Antonneau)
- Tsiory Rakotoarisoa (Carl_221b)
