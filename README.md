# Earnings Data Analysis Program
## Introduction
This is the final project for *FRE-GY 6883 Financial Computing* for the Fall 2023 semester at NYU Tandon.
It analyzes impacts of earnings announcements on the returns of stocks from the Russell 3000 index.
A menu is provided with multiple options to choose from, including statistical analysis and data visualization
powered by [*gnuplot*](http://www.gnuplot.info/).

This is a group project and originally hosted within the GitHub organization for the course.
I had to remove git histories as they contained personal information and proprietary materials from the course.
I was the main contributor and responsible for the most fundamental 
data downloading & analysis logic (`csvParser`, `utils` and `downloader`).

There is still much room for improvements to the current code base, but I do not plan to work on it in the near future.

## How to run
1. Get your own API key at [*EODHD*](https://eodhd.com/) and swap it with the defunct keys in `main`.
Switching to other data sources is possible, but will likely require significant changes to the code base.
2. Make sure you have [*gnuplot*](http://www.gnuplot.info/) and [*libcurl*](https://curl.se/libcurl/) headers installed.
A C++11 compiler is also required. The project was developed and tested with both *clang++* and *g++*.
3. Clone the repository and run `make`, the executable will be generated under the project root and named `FinalProject`.
4. Run the executable and follow the on-screen menu for instructions.