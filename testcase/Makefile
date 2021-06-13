TESTTARGET = mm

.PHONY: test run selftest qstest mmtest

$(TESTTARGET): output.o
	g++ output.o -o $(TESTTARGET) -no-pie

selftest: $(TESTTARGET)
	./$(TESTTARGET)

test: $(TESTTARGET)
	./$(TESTTARGET)test ./$(TESTTARGET)

qstest: qs
	./qstest ./qs

mmtest: mm
	./mmtest ./mm

run:
	../compiler <$(TESTTARGET).in