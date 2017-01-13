NINJA=ninja -C out/cur

all:
	@$(NINJA)

clean:
	@$(NINJA) -t clean

distclean:
	rm -Rf out/
	rm -f build/lib/scripts/gn

.PHONY: all clean dist distclean
