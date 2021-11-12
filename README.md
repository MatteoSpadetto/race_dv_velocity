# Development of an on-board system for measuring the velocity vector of a driverless Formula SAE race car

## Key words

* FFT of an image and its info;
* FFT filters;
* Velocity vector direction;
* Veclocity vector modulus;
* Real scenarios and problems;
* Sample frame and camera parameters;
* Auto adaptive camera settings.

## Summary graph

<br/>

![flow](flow.png?raw=true "Flow")

This summary is just a base to give a structure to the project but it will be changed several times accordingly with the improvements.

## Milestones

1. Understand 3 + 1 methods: 
    * Long exposition for blurred frame analysis by FFT;
    * Analysis of differences between two consecutive frames;
    * EKF on direct measurements;
    * [+1] Different kinds of direct measurements (GPS, Encoder, IMU);
2. Introduction on direct measurements (more qualitative just to understand the state of art) and explenation of why there are not sufficient anymore;
3. Explaining the EKF in order to use it for comparison with the other 2 main methods;
3. QFD and comparison between the 3 main competitors and the different kinds of direct measurements:
    * Preavious E-agleTRT car (Chimera Evoluzione);
    * Formula1 car;
    * AMZ racing Formula Student driverless car;
4. Analyse the best 2 technologies also comparing them with EKF:
    * Long exposition for blurred frame analysis by FFT;
    * Analysis of differences between two consecutive frames;

## QFD

The following QFD is analysing the most important features to implement in the method to calculate the velocity vector (this is just a first try to improve during the thesis improvement).

<br/>

![qfd](qfd.png?raw=true "QFD")

## Reference

### Fast Fourier Transform (FFT)

[What is an FFT](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=1447887)

[FFT of an image (general)](https://www.cs.unm.edu/~brayer/vision/fourier.html)

[Fourier transform of images (math)](http://mstrzel.eletel.p.lodz.pl/mstrzel/pattern_rec/fft_ang.pdf)

[Images and videos (part 1)](https://didatticaonline.unitn.it/dol/pluginfile.php/739709/mod_resource/content/0/01-1%20Images%20and%20Videos.pdf)

[Images and videos (part 2)](https://didatticaonline.unitn.it/dol/pluginfile.php/739853/mod_resource/content/0/01-2%20Images%20and%20Videos.pdf)

[Features](https://didatticaonline.unitn.it/dol/pluginfile.php/800036/mod_resource/content/0/06%20Features.pdf)

[Filtering image in frequency (general)](http://paulbourke.net/miscellaneous/imagefilter/)

[Image enhancement in the frequency domain](https://www.di.univr.it/documenti/OccorrenzaIns/matdid/matdid997179.pdf)

[Frequency Domain Filtering](http://www.cs.cmu.edu/~16385/s15/lectures/Lecture3.pdf)

[An Introduction to the Kalman Filter](https://didatticaonline.unitn.it/dol/pluginfile.php/1043712/mod_resource/content/0/kalman_intro.pdf)

### Velocity vector measurament systems comparison

[End-to-End Velocity Estimation For Autonomous Racing](https://arxiv.org/pdf/2003.06917.pdf)

[Accurate Speed Measurement Methodologies for Formula One Cars](https://ieeexplore.ieee.org/abstract/document/4258488)

[Velocity measuerment based on image blur January 2008 (not official)](https://www.researchgate.net/publication/228993494_Velocity_measuerment_based_on_image_blur)

[Measuring Vehicle Velocity in Real Time Using Modulated Motion Blur of Camera Image Data](https://ieeexplore.ieee.org/document/7543532)

[Velocity detection from a motion blur image using radon transformation (not official)](https://www.researchgate.net/publication/334263992_Velocity_Detection_from_a_Motion_Blur_Image_Using_Radon_Transformation)

### Camera parameters and auto adaptability

[Camera models](https://didatticaonline.unitn.it/dol/pluginfile.php/778032/mod_resource/content/0/02%20Models.pdf)

[Camera geometry](https://didatticaonline.unitn.it/dol/pluginfile.php/796020/mod_resource/content/0/05%20Geometry.pdf)

[Camera exposure (not official)](https://www.exposureguide.com/exposure/)

[Camera parameters (not official)](https://www.borrowlenses.com/blog/shutter-speed-chart/)

[Neural auto exposure](https://light.cs.princeton.edu/wp-content/uploads/2021/04/Neural_Auto_Exposure.pdf)
