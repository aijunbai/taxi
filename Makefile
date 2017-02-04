all: debug release
	ln -sf Release/maxq_op .

release: 
	mkdir -p Release
	cd Release;	cmake -DCMAKE_BUILD_TYPE=Release ..; make -j `nproc`
	ln -sf Release/maxq_op .

debug: 
	mkdir -p Debug
	cd Debug; cmake -DCMAKE_BUILD_TYPE=Debug ..; make -j `nproc`
	ln -sf Debug/maxq_op .

clean:
	rm -fr Release
	rm -fr Debug
	rm -f maxq_op
	rm -fr *.tbl *.log *.dot

cleanall: clean
	rm -fr data
