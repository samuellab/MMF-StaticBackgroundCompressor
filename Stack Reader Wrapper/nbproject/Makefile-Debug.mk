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
	${OBJECTDIR}/StackReaderWrapper.o


# C Compiler Flags
CFLAGS=-DBUILD_DLL

# CC Compiler Flags
CCFLAGS=-DBUILD_DLL
CXXFLAGS=-DBUILD_DLL

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib -L.. ../image_stack_compressor.lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ./StackReaderWrapper.dll

./StackReaderWrapper.dll: ../image_stack_compressor.lib

./StackReaderWrapper.dll: ${OBJECTFILES}
	${MKDIR} -p .
	${LINK.cc} -DBUILD_DLL -static-libgcc -static-libstdc++ -shared -o ./StackReaderWrapper.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackReaderWrapper.o: StackReaderWrapper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I.. -I../Necessary\ Libraries\ and\ Includes/CV/headers  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackReaderWrapper.o StackReaderWrapper.cpp

# Subprojects
.build-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ./StackReaderWrapper.dll

# Subprojects
.clean-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
