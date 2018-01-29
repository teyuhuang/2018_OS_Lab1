FILENAME = test
FILENAME = linker

# exec: linker
# 	@./$^ ./input_folder/lab1samples/input-20
# 	@rm $^
linker: $(FILENAME).cpp
	@echo "=======IMPORTANT======"
	@echo "Pleas use gcc 6.2.0 or above"
	@echo "module load gcc-6.2.0"
	@echo "======================"
	@g++ -std=c++0x $^ -o linker
clear: linker
	rm $^
loadModule:
	module load gcc-6.2.0
unloadModule:
	module unload gcc-6.2.0