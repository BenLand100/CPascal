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

# Macros
PLATFORM=GNU-Linux-x86

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Release/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/EFence.o \
	${OBJECTDIR}/Interpreter.o \
	${OBJECTDIR}/LeakTracer.o \
	${OBJECTDIR}/Container.o \
	${OBJECTDIR}/lexer.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Type.o \
	${OBJECTDIR}/Expression.o \
	${OBJECTDIR}/parser.o \
	${OBJECTDIR}/Value.o \
	${OBJECTDIR}/Operator.o \
	${OBJECTDIR}/Variable.o \
	${OBJECTDIR}/Element.o \
	${OBJECTDIR}/Exceptions.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-Wall -pedantic -ansi
CXXFLAGS=-Wall -pedantic -ansi

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/${PLATFORM}/cpascal

dist/Release/${PLATFORM}/cpascal: ${OBJECTFILES}
	${MKDIR} -p dist/Release/${PLATFORM}
	${LINK.cc} --eh-frame-hdr -o dist/Release/${PLATFORM}/cpascal -s ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/EFence.o: EFence.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/EFence.o EFence.cpp

${OBJECTDIR}/Interpreter.o: Interpreter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Interpreter.o Interpreter.cpp

${OBJECTDIR}/LeakTracer.o: LeakTracer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/LeakTracer.o LeakTracer.cpp

${OBJECTDIR}/Container.o: Container.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Container.o Container.cpp

${OBJECTDIR}/lexer.o: lexer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexer.o lexer.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Type.o: Type.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Type.o Type.cpp

${OBJECTDIR}/Expression.o: Expression.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Expression.o Expression.cpp

${OBJECTDIR}/parser.o: parser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/parser.o parser.cpp

${OBJECTDIR}/Value.o: Value.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Value.o Value.cpp

${OBJECTDIR}/Operator.o: Operator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Operator.o Operator.cpp

${OBJECTDIR}/Variable.o: Variable.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Variable.o Variable.cpp

${OBJECTDIR}/Element.o: Element.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Element.o Element.cpp

${OBJECTDIR}/Exceptions.o: Exceptions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -MMD -MP -MF $@.d -o ${OBJECTDIR}/Exceptions.o Exceptions.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/${PLATFORM}/cpascal

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
