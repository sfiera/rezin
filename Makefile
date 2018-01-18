NINJA=build/lib/bin/ninja -C out/cur

.PHONY: all
all:
	@$(NINJA)

.PHONY: test
test: all
	python3 -m pytest test

.PHONY: clean
clean:
	@$(NINJA) -t clean

.PHONY: distclean
distclean:
	rm -Rf out/
