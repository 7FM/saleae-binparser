all:
	g++ -Wall -O3 -o example example_main.cpp saleae_v2_digital.cpp mmap.cpp
	
