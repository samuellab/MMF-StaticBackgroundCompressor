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
CND_CONF=Windows
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
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib ../WindowsBinaries/image_stack_compressor.lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../WindowsBinaries/mmf_player.exe

../WindowsBinaries/mmf_player.exe: ../WindowsBinaries/image_stack_compressor.lib

../WindowsBinaries/mmf_player.exe: ${OBJECTFILES}
	${MKDIR} -p ../WindowsBinaries
	${LINK.cc} -static-libgcc -static-libstdc++ -o ../WindowsBinaries/mmf_player ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackPlayer.o: nbproject/Makefile-${CND_CONF}.mk StackPlayer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Necessary\ Libraries\ and\ Includes/CV/headers -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackPlayer.o StackPlayer.cpp

# Subprojects
.build-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Windows

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../WindowsBinaries/mmf_player.exe

# Subprojects
.clean-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Windows clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
