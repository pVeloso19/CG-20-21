all: main.cpp
	make clear
	make generator

generator: main.cpp
	$(CXX) main.cpp -o generator

clearfiguras:
	rm -f *.3d
	rm -f *.xml
	
clear: 
	rm -f generator