compiler=gcc
compile_flags=-Wall
link_flags=-lpvm3 -lgpvm3 -lm
sources=vector.h vector.c

all: master slave
	mv master slave ${PVM_HOME}

run: all
	pvm -nvxlen

clean:
	rm -rf master slave

master: master.c $(sources)
	$(compiler) $(compile_flags) $(link_flags) -o $@ $^

slave: slave.c $(sources)
	$(compiler) $(compile_flags) $(link_flags) -o $@ $^
