# Sobel Filter Optimisation

The project is focused on optimising a greyscale Sobel filter implemented in C. The Sobel filter is used for edge detection in image processing. ​The project applies the filter to a 4096x4096 grayscale image.

It was developed as a Lab assignment for the "ECE415 High Performance Computing" course, in collaboration with Charalampos Karaiskos, over the course of a week. Our development was based on the requirements set by [Lab1.pdf](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/Lab1.pdf).
Our full work can be seen in the report file [report.pdf](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/report.pdf). Our numerical results can be viewed in detail in [results.xlsx](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/results.xlsx).

As a bonus, additional experiments were conducted with different compilers, including gcc, icc, and Intel oneAPI DPC++/C++ Compiler to test how they perform. The Intel compiler performed best, and gcc showed consistent executable file sizes. These can be viewed in [extras.xlsx](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/extras.xlsx).

Scripts and a master Makefile were used to streamline the experimental process. All optimisations were performed in an additive manner, with each test building on the previous ones.​ The results showed performance improvements for each optimisation. ​

Use:
```sh
make clean
make all
make run
make image
make excel
```

# Notes
The scripting suite was developed using Python 3.9 and Bash. Certain make commands require the installations of a few packages.

For the full suite to run:
```sh
apt-get install ImageMagick
apt-get install python3.9
pip install openpyxl
pip install pandas
```
