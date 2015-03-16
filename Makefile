NINJA=ninja -C out/cur

all:
	@$(NINJA)

clean:
	@$(NINJA) -t clean

dist:
	scripts/dist.py

distclean:
	rm -Rf out/

man/rezin.1: man/rezin.1.ronn
	ronn -r --pipe $< >$@

README: man/rezin.1
	man $< |col -b >$@

.PHONY: all clean dist distclean
