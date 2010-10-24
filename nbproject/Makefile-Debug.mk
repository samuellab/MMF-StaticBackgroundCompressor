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
	${OBJECTDIR}/BackgroundRemovedImage.o \
	${OBJECTDIR}/tictoc/Timer.o \
	${OBJECTDIR}/tictoc/tictoc.o \
	${OBJECTDIR}/StaticBackgroundCompressor.o \
	${OBJECTDIR}/testmain.o \
	${OBJECTDIR}/LinearStackCompressor.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-LNecessary\ Libraries\ and\ Includes/CV/lib -lcv -lcxcore -lhighgui

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/MinGW-Windows/image_stack_compressor.exe

dist/Debug/MinGW-Windows/image_stack_compressor.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/MinGW-Windows
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/image_stack_compressor ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/BackgroundRemovedImage.o: BackgroundRemovedImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImage.o BackgroundRemovedImage.cpp

${OBJECTDIR}/tictoc/Timer.o: tictoc/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/Timer.o tictoc/Timer.cpp

${OBJECTDIR}/tictoc/tictoc.o: tictoc/tictoc.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/tictoc.o tictoc/tictoc.cpp

${OBJECTDIR}/StaticBackgroundCompressor.o: StaticBackgroundCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressor.o StaticBackgroundCompressor.cpp

${OBJECTDIR}/testmain.o: testmain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/testmain.o testmain.cpp

${OBJECTDIR}/LinearStackCompressor.o: LinearStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinearStackCompressor.o LinearStackCompressor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/MinGW-Windows/image_stack_compressor.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
