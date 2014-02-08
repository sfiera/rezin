NINJA=ninja -C out/cur

all:
	@$(NINJA)

clean:
	@$(NINJA) -t clean

dist:
	scripts/dist.py

distclean:
	rm -Rf out/

.PHONY: all clean dist distclean
