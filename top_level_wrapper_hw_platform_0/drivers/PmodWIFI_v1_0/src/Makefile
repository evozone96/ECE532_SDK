COMPILER=
ARCHIVER=
CP=cp
COMPILER_FLAGS=
EXTRA_COMPILER_FLAGS=
LIB=libxil.a

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-I./. -I${INCLUDEDIR} -I${INCLUDEDIR}/MRF24G -I./DEIPcK -I./DEIPcK/utility -I./DEWFcK -I./DEIPcK -I./MRF24G -I./MRF24G/utility -I./MRF24G/utility/ud_inc/internal -I./MRF24G/utility/ud_inc/shared

INCLUDEFILES=*.h
LIBSOURCES=*.c DEIPcK/*.cpp DEIPcK/utility/*.c DEWFcK/*.cpp MRF24G/*.c MRF24G/*.cpp MRF24G/utility/*.c MRF24G/utility/fpga/*.c MRF24G/utility/fpga/*.cpp
OUTS = *.o

libs:
	echo "Compiling PmodWIFI..."
	$(COMPILER) $(COMPILER_FLAGS) $(EXTRA_COMPILER_FLAGS) -Wno-attributes $(INCLUDES) $(LIBSOURCES)
	$(ARCHIVER) -r ${RELEASEDIR}/${LIB} ${OUTS}
	make clean

include:
	${CP} $(INCLUDEFILES) $(INCLUDEDIR)
	mkdir -p ../../../include/DEIPcK
	mkdir -p ../../../include/DEIPcK/utility
	mkdir -p ../../../include/DEIPcK/utility/fpga
	mkdir -p ../../../include/DEWFcK
	mkdir -p ../../../include/MRF24G
	mkdir -p ../../../include/MRF24G/utility
	mkdir -p ../../../include/MRF24G/utility/ud_inc
	mkdir -p ../../../include/MRF24G/utility/ud_inc/internal
	mkdir -p ../../../include/MRF24G/utility/ud_inc/shared
	cp DEIPcK/*.h $(INCLUDEDIR)/DEIPcK
	cp DEIPcK/utility/*.h $(INCLUDEDIR)/DEIPcK/utility
	cp DEIPcK/utility/fpga/*.h $(INCLUDEDIR)/DEIPcK/utility/fpga
	cp DEWFcK/*.h $(INCLUDEDIR)/DEWFcK
	cp MRF24G/*.h $(INCLUDEDIR)/MRF24G
	cp MRF24G/utility/*.h $(INCLUDEDIR)/MRF24G/utility
	cp MRF24G/utility/ud_inc/internal/*.h $(INCLUDEDIR)/MRF24G/utility/ud_inc/internal
	cp MRF24G/utility/ud_inc/shared/*.h $(INCLUDEDIR)/MRF24G/utility/ud_inc/shared
	cp DEWFcK/DEWFcK.h $(INCLUDEDIR)
	cp DEIPcK/DEIPcK.h $(INCLUDEDIR)
	cp MRF24G/AdaptorClass.h $(INCLUDEDIR)
	cp Networkprofile.x $(INCLUDEDIR)

clean:
	rm -rf ${OUTS}
