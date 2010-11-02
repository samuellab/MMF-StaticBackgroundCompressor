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
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/StackReader.o \
	${OBJECTDIR}/BackgroundRemovedImage.o \
	${OBJECTDIR}/tictoc/Timer.o \
	${OBJECTDIR}/tictoc/tictoc.o \
	${OBJECTDIR}/StaticBackgroundCompressor.o \
	${OBJECTDIR}/testmain.o \
	${OBJECTDIR}/MightexMetaData.o \
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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk dist/Release/MinGW-Windows/libImage-Stack-Compressor.dll

dist/Release/MinGW-Windows/libImage-Stack-Compressor.dll: ${OBJECTFILES}
	${MKDIR} -p dist/Release/MinGW-Windows
	${LINK.cc} -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libImage-Stack-Compressor.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackReader.o: StackReader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackReader.o StackReader.cpp

${OBJECTDIR}/BackgroundRemovedImage.o: BackgroundRemovedImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImage.o BackgroundRemovedImage.cpp

${OBJECTDIR}/tictoc/Timer.o: tictoc/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/Timer.o tictoc/Timer.cpp

${OBJECTDIR}/tictoc/tictoc.o: tictoc/tictoc.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/tictoc.o tictoc/tictoc.cpp

${OBJECTDIR}/StaticBackgroundCompressor.o: StaticBackgroundCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressor.o StaticBackgroundCompressor.cpp

${OBJECTDIR}/testmain.o: testmain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/testmain.o testmain.cpp

${OBJECTDIR}/MightexMetaData.o: MightexMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/MightexMetaData.o MightexMetaData.cpp

${OBJECTDIR}/LinearStackCompressor.o: LinearStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinearStackCompressor.o LinearStackCompressor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/MinGW-Windows/libImage-Stack-Compressor.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
