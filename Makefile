all: debug release
	ln -sf Release/taxi .

release: 
	mkdir -p Release
	cd Release;	cmake -DCMAKE_BUILD_TYPE=Release ..; make -j `nproc`
	ln -sf Release/taxi .

debug: 
	mkdir -p Debug
	cd Debug; cmake -DCMAKE_BUILD_TYPE=Debug ..; make -j `nproc`
	ln -sf Debug/taxi .

clean:
	rm -fr Release
	rm -fr Debug
	rm -f taxi
	rm -fr *.tbl *.log *.dot

cleanall: clean
	rm -fr data
