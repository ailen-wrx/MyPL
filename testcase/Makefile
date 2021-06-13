TESTTARGET = qs

.PHONY: test run

test:
	g++ output.o -o $(TESTTARGET) -no-pie
	./$(TESTTARGET)test ./$(TESTTARGET)

run:
	../compiler <$(TESTTARGET).in