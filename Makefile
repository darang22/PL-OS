CXX=g++
CFLAGS=-pthread
CXXFLAGS=-std=c++11
BINDIR=bin

all: bin/Planificador ${BINDIR}/PCP ${BINDIR}/PLP

${BINDIR}:
	mkdir bin

bin/Planificador: src/Planificador.cpp ${BINDIR}
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o ${BINDIR}/Planificador src/Planificador.cpp

src/Planificador: src/Planificador.cpp src/Comandos.h src/msg.h

bin/PLP: src/PLP.cpp ${BINDIR}
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o ${BINDIR}/PLP src/PLP.cpp

src/PLP: src/PLP.cpp src/msg.h

bin/PCP: src/PCP.cpp ${BINDIR}
	$(CXX) $(CFLAGS) $(CXXFLAGS) -o ${BINDIR}/PCP src/PCP.cpp
src/PCP: src/PCP.cpp src/msg.h

clean:
	rm -R bin/PLP bin/PCP bin/Planificador
	rm -R -f ${BINDIR}
