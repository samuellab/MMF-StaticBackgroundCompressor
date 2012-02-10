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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Linux
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/StackDecimator.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-static-libgcc -static-libstdc++
CXXFLAGS=-static-libgcc -static-libstdc++

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib -L.. ../LinuxBinaries/image_stack_compressor.lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../LinuxBinaries/decimatestack.exe

../LinuxBinaries/decimatestack.exe: ../LinuxBinaries/image_stack_compressor.lib

../LinuxBinaries/decimatestack.exe: ${OBJECTFILES}
	${MKDIR} -p ../LinuxBinaries
	${LINK.cc} -static-libgcc -static-libstdc++ -o ../LinuxBinaries/decimatestack.exe ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackDecimator.o: StackDecimator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I.. -I../Necessary\ Libraries\ and\ Includes/CV/headers -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackDecimator.o StackDecimator.cpp

# Subprojects
.build-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=linux

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../LinuxBinaries/decimatestack.exe

# Subprojects
.clean-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=linux clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
