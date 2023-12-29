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
A C++11 compiler is also required. The project was developed and tested with both clang++ and g++.
3. Clone the repository and run `make`, the executable will be generated under the project root and named `FinalProject`.
4. Run the executable and follow the on-screen menu for instructions.

## Interesting Discovery
The implementation of `std::get_time` has [documented bugs](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71367) in older versions of g++/libstdc++.
I was trying to use `std::get_time` & `std::put_time` in `csvParser` for date format conversion from, for example, `15-aug-03`
to `2003-08-15`. The results were clearly wrong, so I did some research and stumbled upon these old bug reports that
confirmed the problem. A mimimal example to reproduce the bug:

```c++
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main() {
  tm time = {};
  string x = "15-aug-03";
  istringstream ss(x);
  ss >> get_time(&time, "%d-%b-%y");
  ostringstream oss;
  oss << put_time(&time, "%Y-%m-%d");
  cout << oss.str() << endl;
  return 0;
}
```

This code snippet gives the correct *2003-08-15* on g++12&13, and *1900-01-15* on every previous C++11-enabled (these two functions were introduced
in C++11) version I tested on.
