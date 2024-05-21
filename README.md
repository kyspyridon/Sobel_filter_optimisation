# Sobel Filter Optimisation

The project is focused on optimising a greyscale Sobel filter implemented in C. The Sobel filter is used for edge detection in image processing. ​The project applies the filter to a 4096x4096 grayscale image.

It was developed as a Lab assignment for the "ECE415 High Performance Computing" course, in the University Of Thessaly, in collaboration with Charalampos Karaiskos, over the course of a week. Our development was based on the requirements set by [Lab1.pdf](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/Lab1.pdf).

# Naming Convention
All source file directories follow a specific naming convention. A number indicating the order in which the optimizations were performed during the projects development, followed by the name of the optimization itself meaning that each test builds on top of each predecessor. All optimisations were performed in an additive manner, with each test building on the previous ones.​ The results showed performance improvements for each optimisation. ​

Our full work can be seen in the report file [report.pdf](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/report.pdf). Our numerical results can be viewed in detail in [results.xlsx](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/results.xlsx).

As a bonus, additional experiments were conducted with different compilers, including gcc, icc, and Intel oneAPI DPC++/C++ Compiler to test how they perform. The Intel compiler performed best, and gcc showed consistent executable file sizes. These can be viewed in [extras.xlsx](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/extras.xlsx).

# Target Hardware
All experiments were performed in the same machine, a Dell laptop with the following specs:
- CPU: Intel(R) Core(TM) i7-4800MQ CPU @ 2.70GHz, 2701 Mhz, 4 Core(s), 8 Logical Processor(s)
- Installed Physical Memory (RAM): 8.00 GB DDR3
- GPU: NVIDIA Quadro K1100
- Disk: Crucial MX500 500GB SSD
- Operating System: Ubuntu 20.04 LTS

# Tool API
A scripting suite was developed to help streamline the experimental process. This consisted of several Bash and Python scripts. These are available as makefile commands for ease of use. Detailed descriptions for all the commands below can be found in [report.pdf](https://github.com/kyspyridon/Sobel_filter_optimisation/blob/main/report.pdf).

Here is the list of available commands:
```sh
make clean
make all
make run
make image
make excel
```

Use ```make all``` to build the executables under the build/ directory and ```make run``` to perform the tests. Each test is performed 22 times. ```make excel``` discards the best and worst performances as outliers and creates an Excel spreadsheet by parsing the results under the output/ directory. ```make image``` converts the generated .grey files to .jpg using ImageMagick.


# Notes
The scripting suite was developed using Python 3.9 and Bash. Certain make commands require the installations of a few packages.

For the full suite to run:
```sh
apt-get install ImageMagick
apt-get install python3.9
pip install openpyxl
pip install pandas
```
