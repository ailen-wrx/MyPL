TESTTARGET = aa

.PHONY: test run selftest qstest mmtest

$(TESTTARGET): output.o
	g++ output.o -o $(TESTTARGET) -no-pie

selftest: $(TESTTARGET)
	./$(TESTTARGET)

test: $(TESTTARGET)
	./$(TESTTARGET)test ./$(TESTTARGET)

qstest:
	./qstest ./ourqs

mmtest:
	./mmtest ./ourmm

run:
	../compiler <$(TESTTARGET).in