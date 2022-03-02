

.PHONY:	build-native build-amiga

build-amiga:
	mkdir -p build_amiga
	cd build_amiga && cmake -DCMAKE_TOOLCHAIN_FILE=../src/amigaos-toolchain.cmake ../src && make

build-native:
	mkdir -p build_native
	cd build_native && cmake -DINCLUDE_TESTS=ON ../src && make

.PHONY: clean

clean:
	rm -rf build_amiga build_native
