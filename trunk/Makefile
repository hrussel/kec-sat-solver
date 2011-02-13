GCC=gcc
GCC_FLAGS=-Ilib -Wall -O3 

#Name of the files that are part of the project
MAIN_FILE=kec_o_sat_s

FILES=\
    sat \
    sat_io \
    pure_literals \
    list 

#The next variables hold the dependencies of each file
#DEP_example=dep1 dep2 dep3
DEP_sat=list
DEP_sat_io=list sat 
DEP_kec_o_sat_s=sat sat_io
DEP_pure_literals=sat
DEP_list=

#Rule for compiling the main file
$(MAIN_FILE): src/$(MAIN_FILE).c $(FILES:%=bin/%.o)
	$(GCC) $(GCC_FLAGS) -o $(MAIN_FILE) src/$(MAIN_FILE).c $(FILES:%=bin/%.o)
	
#General rule for compiling
bin/%.o: src/%.c lib/%.h
	$(GCC) $(GCC_FLAGS) -c $< -o $@

#Dependencies:
#bin/example.o: $(DEP_example:%=src/%.c) $(DEP_example:%=lib/%.h)
bin/sat.o: $(DEP_sat:%=src/%.c) $(DEP_sat:%=lib/%.h)
bin/sat_io.o: $(DEP_sat_io:%=src/%.c) $(DEP_sat_io:%=lib/%.h)
bin/pure_literals.o: $(DEP_pure_literals:%=src/%.c) $(DEP_pure_literals:%=lib/%.h)
bin/list.o: $(DEP_list:%=src/%.c) $(DEP_list:%=lib/%.h)

.PHONY: doc

doc:
	doxygen doc/Doxyfile
clean:
	rm -f bin/*.o kec_o_sat_s
