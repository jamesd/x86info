CFLAGS = -Wall -W -Wshadow -g -O2
CC = gcc

# Call the source code checker "sparse" as part of the C compilation.
# Use 'make C=1' to enable checking of only re-compiled files.
ifdef C
  ifeq ("$(origin C)", "command line")
    ifneq ($(C),0)
      CC = cgcc
    endif
  endif
endif

SHELL = /bin/sh


.c.o:
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<
	@cp $*.d $*.P; \
	 sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	     -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

.S.o:
	$(CC) $(CFLAGS) -o $@ -c $<


all: x86info test lsmsr


LSMSR_TMP_HEADERS=AMD/k8.h AMD/fam10h.h generic_msr.h

%.h: %.regs scripts/createheader.py
	python scripts/createheader.py $< `basename $< .regs` >$@

LSMSR_SRC =\
	lsmsr.c\
	cpuid.c\
	havecpuid.c

LSMSR_OBJS = $(LSMSR_SRC:%.c=%.o)

lsmsr: $(LSMSR_TMP_HEADERS) $(LSMSR_OBJS)
	$(CC) $(CFLAGS) -o lsmsr $(LSMSR_OBJS)

-include $(LSMSR_SRC:%.c=%.P)

X86INFO_SRC =\
	AMD/identify.c\
	AMD/bluesmoke.c\
	AMD/MSR-Athlon.c\
	AMD/MSR-K6.c\
	AMD/powernow.c\
	AMD/dumppsb.c\
	AMD/bugs.c\
\
	Cyrix/identify.c\
\
	Intel/identify.c\
	Intel/identify-family6.c\
	Intel/identify-family6-extended.c\
	Intel/identify-family15.c\
	Intel/info.c\
	Intel/bluesmoke.c\
	Intel/cachesize.c\
	Intel/eblcr.c\
	Intel/MSR-P4.c\
	Intel/MSR-P6.c\
	Intel/MSR-PM.c\
	Intel/microcode.c\
	Intel/topology.c\
\
	Centaur/identify.c\
	Centaur/MSR-C3.c\
	Centaur/longhaul.c\
	Centaur/powersaver.c\
\
	NatSemi/identify.c\
\
	RiSE/identify.c\
\
	SiS/identify.c\
\
	x86info.c\
	havecpuid.c\
	cpuid.c\
	dumpregs.c\
	features.c\
	identify.c\
	rdmsr.c\
	binary.c\
	mptable.c\
	get_model_name.c\
	mtrr.c \
	connector.c\
\
	bench/benchmarks.c\
	bench/MHz.c

X86INFO_OBJS = $(X86INFO_SRC:%.c=%.o)

x86info: $(X86INFO_OBJS)
	$(CC) $(CFLAGS) -o x86info $(X86INFO_OBJS)

-include $(X86INFO_SRC:%.c=%.P)


nodes:
	scripts/makenodes

test:
	scripts/testnodes

VERSION=1.23

release:
	git repack -a -d
	git-prune-packed
	git-archive --format=tar --prefix=x86info-$(VERSION)/ HEAD | gzip -9 > x86info-$(VERSION).tgz

clean:
	@find . -name "*.o" -exec rm {} \;
	@find . -name "*~" -exec rm {} \;
	@find . -name "*.P" -exec rm {} \;
	@rm -f x86info x86info.exe
	@rm -f lsmsr $(LSMSR_TMP_HEADERS)

splint:
	splint +posixlib -badflag -fileextensions -type -nullassign -boolops -showcolumn -sysunrecog -fullinitblock -onlytrans -unrecog -usedef -statictrans -compdestroy -predboolint -predboolothers -D__`uname -m`__ $(X86INFO_SRC)
