CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -lcurl -pthread

FinalProject: main.o csvParser.o utils.h downloader.o Calculation.h Sampled_Calculation.o gnuplot.o operator_overload.o
	$(CC) $(CFLAGS) -o FinalProject main.o csvParser.o downloader.o Sampled_Calculation.o gnuplot.o operator_overload.o

main.o: main.cpp csvParser.h utils.h Calculation.h gnuplot.h
	$(CC) $(CFLAGS) -c main.cpp

downloader.o: downloader.cpp downloader.h utils.h
	$(CC) $(CFLAGS) -c downloader.cpp

csvParser.o: csvParser.cpp csvParser.h
	$(CC) $(CFLAGS) -c csvParser.cpp

Sampled_Calculation.o: Sampled_Calculation.cpp Sampled_Calculation.h utils.h Calculation.h downloader.h
	$(CC) $(CFLAGS) -c Sampled_Calculation.cpp
	
gnuplot.o: gnuplot.cpp gnuplot.h
	$(CC) $(CFLAGS) -c gnuplot.cpp

operator_overload.o: operator_overload.cpp operator_overload.h
	$(CC) $(CFLAGS) -c operator_overload.cpp

# Matrix.o: Matrix.cpp Matrix.h
# 	$(CC) $(CFLAGS) -c Matrix.cpp
	
clean:
	rm -f *.o FinalProject