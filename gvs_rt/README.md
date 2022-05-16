# Ground Velocity Sensor (GVS)
Software for GVS application (direction and modulus):
* Consecutive frame differences 
* Long time adaptable exposure 

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

## Prerequisites

This software was implemented with `OpenCV` in `C++` so you basically need the source code, `cmake` and `make`. The `Makefile` is provided in the folder.

## Compile and run on VScode or linux-base terminal
To compile and run this software just type the following commands:

```
cd /path/to/folder/gvs
```
```
cmake .
```
```
make
```
```
./gvs
```

In the source code the path to the frame files are written for linux based systems. If you have another operative system you might have to change the path accordingly with your system rules.
```
    String folderpath = "./path/to/frames/";
```

## Built With

- [VScode](https://code.visualstudio.com/) - VScode
- [OpenCV](https://opencv.org/) - OpenCV C++

## Versioning

This is the first version of `GVS`. Probably a new version will be deployed later with new improvments.

## Matteo Spadetto contacts

For issues and questions please contact me at matteo.spadetto-1@studenti.unitn.it

- **Matteo Spadetto GitHub** - [eagletrt](https://github.com/MatteoSpadetto)
