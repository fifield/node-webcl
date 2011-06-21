build:
	mkdir build
	make -C src/wrapper all
	node-waf configure
	node-waf build

clean:
	rm -rf build/
	rm _webcl.node
