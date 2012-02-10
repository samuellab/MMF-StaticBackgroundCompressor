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
	${OBJECTDIR}/StackReader.o \
	${OBJECTDIR}/WindowsThreadStackCompressor.o \
	${OBJECTDIR}/BackgroundRemovedImage.o \
	${OBJECTDIR}/ExtraDataWriter.o \
	${OBJECTDIR}/tictoc/Timer.o \
	${OBJECTDIR}/NameValueMetaData.o \
	${OBJECTDIR}/tictoc/tictoc.o \
	${OBJECTDIR}/CompositeImageMetaData.o \
	${OBJECTDIR}/StaticBackgroundCompressor.o \
	${OBJECTDIR}/StaticBackgroundCompressorLoader.o \
	${OBJECTDIR}/BackgroundRemovedImageLoader.o \
	${OBJECTDIR}/MightexMetaData.o \
	${OBJECTDIR}/IplImageLoaderFixedWidth.o \
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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk WindowsBinaries/image_stack_compressor.lib

WindowsBinaries/image_stack_compressor.lib: ${OBJECTFILES}
	${MKDIR} -p WindowsBinaries
	${RM} WindowsBinaries/image_stack_compressor.lib
	${AR} -rv WindowsBinaries/image_stack_compressor.lib ${OBJECTFILES} 
	$(RANLIB) WindowsBinaries/image_stack_compressor.lib

${OBJECTDIR}/StackReader.o: nbproject/Makefile-${CND_CONF}.mk StackReader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackReader.o StackReader.cpp

${OBJECTDIR}/WindowsThreadStackCompressor.o: nbproject/Makefile-${CND_CONF}.mk WindowsThreadStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/WindowsThreadStackCompressor.o WindowsThreadStackCompressor.cpp

${OBJECTDIR}/BackgroundRemovedImage.o: nbproject/Makefile-${CND_CONF}.mk BackgroundRemovedImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImage.o BackgroundRemovedImage.cpp

${OBJECTDIR}/ExtraDataWriter.o: nbproject/Makefile-${CND_CONF}.mk ExtraDataWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/ExtraDataWriter.o ExtraDataWriter.cpp

${OBJECTDIR}/tictoc/Timer.o: nbproject/Makefile-${CND_CONF}.mk tictoc/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/Timer.o tictoc/Timer.cpp

${OBJECTDIR}/NameValueMetaData.o: nbproject/Makefile-${CND_CONF}.mk NameValueMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/NameValueMetaData.o NameValueMetaData.cpp

${OBJECTDIR}/tictoc/tictoc.o: nbproject/Makefile-${CND_CONF}.mk tictoc/tictoc.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/tictoc.o tictoc/tictoc.cpp

${OBJECTDIR}/CompositeImageMetaData.o: nbproject/Makefile-${CND_CONF}.mk CompositeImageMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/CompositeImageMetaData.o CompositeImageMetaData.cpp

${OBJECTDIR}/StaticBackgroundCompressor.o: nbproject/Makefile-${CND_CONF}.mk StaticBackgroundCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressor.o StaticBackgroundCompressor.cpp

${OBJECTDIR}/StaticBackgroundCompressorLoader.o: nbproject/Makefile-${CND_CONF}.mk StaticBackgroundCompressorLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressorLoader.o StaticBackgroundCompressorLoader.cpp

${OBJECTDIR}/BackgroundRemovedImageLoader.o: nbproject/Makefile-${CND_CONF}.mk BackgroundRemovedImageLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImageLoader.o BackgroundRemovedImageLoader.cpp

${OBJECTDIR}/MightexMetaData.o: nbproject/Makefile-${CND_CONF}.mk MightexMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/MightexMetaData.o MightexMetaData.cpp

${OBJECTDIR}/IplImageLoaderFixedWidth.o: nbproject/Makefile-${CND_CONF}.mk IplImageLoaderFixedWidth.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/IplImageLoaderFixedWidth.o IplImageLoaderFixedWidth.cpp

${OBJECTDIR}/ImageMetaDataLoader.o: nbproject/Makefile-${CND_CONF}.mk ImageMetaDataLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImageMetaDataLoader.o ImageMetaDataLoader.cpp

${OBJECTDIR}/LinearStackCompressor.o: nbproject/Makefile-${CND_CONF}.mk LinearStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -INecessary\ Libraries\ and\ Includes -INecessary\ Libraries\ and\ Includes/CV/headers -Itictoc -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinearStackCompressor.o LinearStackCompressor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} WindowsBinaries/image_stack_compressor.lib

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
