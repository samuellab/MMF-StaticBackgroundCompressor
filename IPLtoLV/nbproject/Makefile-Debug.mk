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
	${OBJECTDIR}/IPLtoLV.o


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
LDLIBSOPTIONS=-L../Necessary\ Libraries\ and\ Includes/CV/lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ./IPLtoLV.dll

./IPLtoLV.dll: ${OBJECTFILES}
	${MKDIR} -p .
	${LINK.c} -shared -o ./IPLtoLV.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/IPLtoLV.o: IPLtoLV.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -I../Necessary\ Libraries\ and\ Includes/CV/headers  -MMD -MP -MF $@.d -o ${OBJECTDIR}/IPLtoLV.o IPLtoLV.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ./IPLtoLV.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
