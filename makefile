FILENAME = test
FILENAME = linker

exec: a.out
	@./a.out ./input_folder/lab1samples/input-17
	@rm a.out
a.out: $(FILENAME).cpp
	@g++ -std=c++0x $^
clear: a.out
	rm $^
loadModule:
	module load gcc-6.2.0
unloadModule:
	module unload gcc-6.2.0