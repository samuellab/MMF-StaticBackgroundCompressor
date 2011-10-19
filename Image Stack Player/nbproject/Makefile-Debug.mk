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
FC=
AS=as.exe

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

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
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib ../image_stack_compressor.lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ./mmf_player.exe

./mmf_player.exe: ../image_stack_compressor.lib

./mmf_player.exe: ${OBJECTFILES}
	${MKDIR} -p .
	${LINK.cc} -o ./mmf_player ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackPlayer.o: StackPlayer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../Necessary\ Libraries\ and\ Includes/CV/headers -I.. -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackPlayer.o StackPlayer.cpp

# Subprojects
.build-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ./mmf_player.exe

# Subprojects
.clean-subprojects:
	cd .. && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
