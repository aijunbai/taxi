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
	if [ -d Release ]; then \
		cd Release; make clean; \
	fi
	if [ -d Debug ]; then \
		cd Debug; make clean; \
	fi
	rm -f maxq_op
	rm -fr *.tbl *.log *.dot
	rm -fr data/

