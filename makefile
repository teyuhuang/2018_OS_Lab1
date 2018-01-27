FILENAME = test
FILENAME = linker

# exec: a.out
# 	@./a.out ./input_folder/lab1samples/input-18
# 	@rm a.out
linker: $(FILENAME).cpp
	@g++ -std=c++0x $^ -o linker
clear: linker
	rm $^
loadModule:
	module load gcc-6.2.0
unloadModule:
	module unload gcc-6.2.0