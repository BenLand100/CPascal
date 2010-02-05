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
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Interpreter.o \
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
CCFLAGS=-Wall -g
CXXFLAGS=-Wall -g

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/cpascal

dist/Debug/GNU-Linux-x86/cpascal: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} --eh-frame-hdr -o dist/Debug/${CND_PLATFORM}/cpascal ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Interpreter.o: nbproject/Makefile-${CND_CONF}.mk Interpreter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Interpreter.o Interpreter.cpp

${OBJECTDIR}/Container.o: nbproject/Makefile-${CND_CONF}.mk Container.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Container.o Container.cpp

${OBJECTDIR}/lexer.o: nbproject/Makefile-${CND_CONF}.mk lexer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/lexer.o lexer.cpp

${OBJECTDIR}/Type.o: nbproject/Makefile-${CND_CONF}.mk Type.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Type.o Type.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Expression.o: nbproject/Makefile-${CND_CONF}.mk Expression.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Expression.o Expression.cpp

${OBJECTDIR}/Value.o: nbproject/Makefile-${CND_CONF}.mk Value.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Value.o Value.cpp

${OBJECTDIR}/parser.o: nbproject/Makefile-${CND_CONF}.mk parser.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/parser.o parser.cpp

${OBJECTDIR}/Operator.o: nbproject/Makefile-${CND_CONF}.mk Operator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Operator.o Operator.cpp

${OBJECTDIR}/Variable.o: nbproject/Makefile-${CND_CONF}.mk Variable.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Variable.o Variable.cpp

${OBJECTDIR}/Element.o: nbproject/Makefile-${CND_CONF}.mk Element.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Element.o Element.cpp

${OBJECTDIR}/Exceptions.o: nbproject/Makefile-${CND_CONF}.mk Exceptions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O3 -DDEBUG -MMD -MP -MF $@.d -o ${OBJECTDIR}/Exceptions.o Exceptions.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/cpascal

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
