#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/EFence.o \
	${OBJECTDIR}/Interpreter.o \
	${OBJECTDIR}/LeakTracer.o \
	${OBJECTDIR}/Container.o \
	${OBJECTDIR}/lexer.o \
	${OBJECTDIR}/Type.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Expression.o \
	${OBJECTDIR}/Value.o \
	${OBJECTDIR}/parser.o \
	${OBJECTDIR}/Operator.o \
	${OBJECTDIR}/Variable.o \
	${OBJECTDIR}/Element.o \
	${OBJECTDIR}/Exceptions.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-Wall
CXXFLAGS=-Wall

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/cpascal

dist/Release/GNU-Linux-x86/cpascal: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} --eh-frame-hdr -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cpascal -s ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/EFence.o: nbproject/Makefile-${CND_CONF}.mk EFence.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/EFence.o EFence.cpp

${OBJECTDIR}/Interpreter.o: nbproject/Makefile-${CND_CONF}.mk Interpreter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Interpreter.o Interpreter.cpp

${OBJECTDIR}/LeakTracer.o: nbproject/Makefile-${CND_CONF}.mk LeakTracer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/LeakTracer.o LeakTracer.cpp

${OBJECTDIR}/Container.o: nbproject/Makefile-${CND_CONF}.mk Container.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Container.o Container.cpp

${OBJECTDIR}/lexer.o: nbproject/Makefile-${CND_CONF}.mk lexer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/lexer.o lexer.cpp

${OBJECTDIR}/Type.o: nbproject/Makefile-${CND_CONF}.mk Type.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Type.o Type.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Expression.o: nbproject/Makefile-${CND_CONF}.mk Expression.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Expression.o Expression.cpp

${OBJECTDIR}/Value.o: nbproject/Makefile-${CND_CONF}.mk Value.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Value.o Value.cpp

${OBJECTDIR}/parser.o: nbproject/Makefile-${CND_CONF}.mk parser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/parser.o parser.cpp

${OBJECTDIR}/Operator.o: nbproject/Makefile-${CND_CONF}.mk Operator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Operator.o Operator.cpp

${OBJECTDIR}/Variable.o: nbproject/Makefile-${CND_CONF}.mk Variable.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Variable.o Variable.cpp

${OBJECTDIR}/Element.o: nbproject/Makefile-${CND_CONF}.mk Element.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Element.o Element.cpp

${OBJECTDIR}/Exceptions.o: nbproject/Makefile-${CND_CONF}.mk Exceptions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.cc) -O3 -s -o ${OBJECTDIR}/Exceptions.o Exceptions.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/cpascal

# Subprojects
.clean-subprojects:
