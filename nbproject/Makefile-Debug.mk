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
	${OBJECTDIR}/StackReader.o \
	${OBJECTDIR}/WindowsThreadStackCompressor.o \
	${OBJECTDIR}/BackgroundRemovedImage.o \
	${OBJECTDIR}/ExtraDataWriter.o \
	${OBJECTDIR}/tictoc/Timer.o \
	${OBJECTDIR}/NameValueMetaData.o \
	${OBJECTDIR}/tictoc/tictoc.o \
	${OBJECTDIR}/CompositeImageMetaData.o \
	${OBJECTDIR}/StaticBackgroundCompressor.o \
	${OBJECTDIR}/testmain.o \
	${OBJECTDIR}/StaticBackgroundCompressorLoader.o \
	${OBJECTDIR}/BackgroundRemovedImageLoader.o \
	${OBJECTDIR}/MightexMetaData.o \
	${OBJECTDIR}/ImageMetaDataLoader.o \
	${OBJECTDIR}/LinearStackCompressor.o


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk image_stack_compressor.lib

image_stack_compressor.lib: ${OBJECTFILES}
	${RM} image_stack_compressor.lib
	${AR} -rv image_stack_compressor.lib ${OBJECTFILES} 
	$(RANLIB) image_stack_compressor.lib

${OBJECTDIR}/StackReader.o: StackReader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackReader.o StackReader.cpp

${OBJECTDIR}/WindowsThreadStackCompressor.o: WindowsThreadStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/WindowsThreadStackCompressor.o WindowsThreadStackCompressor.cpp

${OBJECTDIR}/BackgroundRemovedImage.o: BackgroundRemovedImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImage.o BackgroundRemovedImage.cpp

${OBJECTDIR}/ExtraDataWriter.o: ExtraDataWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/ExtraDataWriter.o ExtraDataWriter.cpp

${OBJECTDIR}/tictoc/Timer.o: tictoc/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/Timer.o tictoc/Timer.cpp

${OBJECTDIR}/NameValueMetaData.o: NameValueMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/NameValueMetaData.o NameValueMetaData.cpp

${OBJECTDIR}/tictoc/tictoc.o: tictoc/tictoc.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/tictoc.o tictoc/tictoc.cpp

${OBJECTDIR}/CompositeImageMetaData.o: CompositeImageMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/CompositeImageMetaData.o CompositeImageMetaData.cpp

${OBJECTDIR}/StaticBackgroundCompressor.o: StaticBackgroundCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressor.o StaticBackgroundCompressor.cpp

${OBJECTDIR}/testmain.o: testmain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/testmain.o testmain.cpp

${OBJECTDIR}/StaticBackgroundCompressorLoader.o: StaticBackgroundCompressorLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressorLoader.o StaticBackgroundCompressorLoader.cpp

${OBJECTDIR}/BackgroundRemovedImageLoader.o: BackgroundRemovedImageLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImageLoader.o BackgroundRemovedImageLoader.cpp

${OBJECTDIR}/MightexMetaData.o: MightexMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/MightexMetaData.o MightexMetaData.cpp

${OBJECTDIR}/ImageMetaDataLoader.o: ImageMetaDataLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImageMetaDataLoader.o ImageMetaDataLoader.cpp

${OBJECTDIR}/LinearStackCompressor.o: LinearStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinearStackCompressor.o LinearStackCompressor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} image_stack_compressor.lib

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
