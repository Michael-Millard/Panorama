### Panorama Generator

Needed a LinkedIn background and a cure for my boredom. Went exploring Dublin and captured a bunch of scenes. 

Requirements: C++17?, OpenCV 4.X, anything else? Should be OS independent. Images need to be sorted left-most to right-most and placed in images/. They need sufficient overlap between neighbouring images.

Foreseeable challenges: moving people and cars/buses. Lack of texture. Insufficient overlap. Perhaps feature extraction and matching in top half only? Bit presumptuous. Will see. HEIC image handling. Perhaps just .png and .jpg allowed.

Structure:
root/
|-- build/
|-- images/
|-- include/
|-- media/
|-- src/
|-- main.cpp
|-- .gitignore
|-- Makefile
\-- CMakeLists.txt

TODO: whole project.
