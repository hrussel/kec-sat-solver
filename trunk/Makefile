GCC=gcc
GCC_FLAGS=-Ilib -Wall -O3

#Name of the files that are part of the project
MAIN_FILE=kec_o_sat_s

FILES=\
    kecosats_algorithm \
    sat_io \
    pure_literals \
    kecosats_structures \
    list \
    conflict_analysis\
    clause_learning\
    heuristics\

#The next variables hold the dependencies of each file
#DEP_example=dep1 dep2 dep3
DEP_kecosats_algorithm=list kecosats_structures
DEP_sat_io=list kecosats_structures
DEP_kec_o_sat_s=kecosats_algorithm sat_io kecosats_structures
DEP_pure_literals=kecosats_structures
DEP_kecosats_structures=list
DEP_list=
DEP_clause_learning=kecosats_structures pure_literals kecosats_algorithm
DEP_conflict_analysis=
DEP_heuristics=kecosats_structures

#Rule for compiling the main file
$(MAIN_FILE): src/$(MAIN_FILE).c $(FILES:%=bin/%.o)
	$(GCC) $(GCC_FLAGS) -o $(MAIN_FILE) src/$(MAIN_FILE).c $(FILES:%=bin/%.o)
	
#General rule for compiling
bin/%.o: src/%.c lib/%.h
	$(GCC) $(GCC_FLAGS) -c $< -o $@

#Dependencies:
#bin/example.o: $(DEP_example:%=src/%.c) $(DEP_example:%=lib/%.h)
bin/kecosats_algorithm.o: $(DEP_kecosats_algorithm:%=src/%.c) $(DEP_kecosats_algorithm:%=lib/%.h)
bin/sat_io.o: $(DEP_sat_io:%=src/%.c) $(DEP_sat_io:%=lib/%.h)
bin/pure_literals.o: $(DEP_pure_literals:%=src/%.c) $(DEP_pure_literals:%=lib/%.h)
bin/kecosats_structures.o: $(DEP_kecosats_structures:%=src/%.c) $(DEP_kecosats_structures:%=lib/%.h)
bin/list.o: $(DEP_list:%=src/%.c) $(DEP_list:%=lib/%.h)
bin/conflict_analysis.o: $(DEP_conflict_analysis:%=src/%.c) $(DEP_conflict_analysis:%=lib/%.h)
bin/clause_learning.o: $(DEP_clause_learning:%=src/%.c) $(DEP_clause_learning:%=lib/%.h)

.PHONY: doc

sudoku:
	make; $(GCC) sudoku2cnf/sudoku.c -o sudoku;
	$(GCC) sudoku2cnf/parse_zchaff_output.c -o sudoku2cnf/parse_zchaff_output

doc:
	doxygen doc/Doxyfile
clean:
	rm -f bin/*.o kec_o_sat_s sudoku sudoku2cnf/parse_zchaff_output;

cleandoc:
	rm -rf doc/html
