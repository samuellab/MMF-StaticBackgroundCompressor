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
	${OBJECTDIR}/StackPlayer.o


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
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/linuxlib ../LinuxBinaries/image_stack_compressor.lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../LinuxBinaries/mmf_player

../LinuxBinaries/mmf_player: ../LinuxBinaries/image_stack_compressor.lib

../LinuxBinaries/mmf_player: ${OBJECTFILES}
	${MKDIR} -p ../LinuxBinaries
	${LINK.cc} -static-libgcc -static-libstdc++ -o ../LinuxBinaries/mmf_player ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackPlayer.o: nbproject/Makefile-${CND_CONF}.mk StackPlayer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Necessary\ Libraries\ and\ Includes/CV/headers -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackPlayer.o StackPlayer.cpp

# Subprojects
.build-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=linux

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../LinuxBinaries/mmf_player

# Subprojects
.clean-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=linux clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
