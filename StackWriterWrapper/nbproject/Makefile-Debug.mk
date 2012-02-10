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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/wtscWrapper.o \
	${OBJECTDIR}/StackWriterWrapper.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-DBUILD_DLL -static-libgcc -static-libstdc++
CXXFLAGS=-DBUILD_DLL -static-libgcc -static-libstdc++

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib -L.. -limage_stack_compressor -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ./StackWriterWrapper.dll

./StackWriterWrapper.dll: ${OBJECTFILES}
	${MKDIR} -p .
	${LINK.cc} -shared -o ./StackWriterWrapper.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/wtscWrapper.o: wtscWrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I.. -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/wtscWrapper.o wtscWrapper.cpp

${OBJECTDIR}/StackWriterWrapper.o: StackWriterWrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I.. -I../Necessary\ Libraries\ and\ Includes/CV/headers -I../tictoc  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackWriterWrapper.o StackWriterWrapper.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ./StackWriterWrapper.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
