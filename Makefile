all:
	make -C src
	make -C libsynapse
	make -C testall

clean:
	make -C src clean
	make -C libsynapse clean
	make -C testall clean
	rm -f `find . -name core`
