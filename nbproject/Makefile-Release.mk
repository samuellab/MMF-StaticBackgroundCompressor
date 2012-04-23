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
CND_CONF=Release
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
	${OBJECTDIR}/WindowsThreadedStaticBackgroundCompressor.o \
	${OBJECTDIR}/MightexMetaData.o \
	${OBJECTDIR}/IplImageLoaderFixedWidth.o \
	${OBJECTDIR}/ImageMetaDataLoader.o \
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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libImage-Stack-Compressor.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libImage-Stack-Compressor.dll: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -shared -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libImage-Stack-Compressor.dll ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/StackReader.o: StackReader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StackReader.o StackReader.cpp

${OBJECTDIR}/WindowsThreadStackCompressor.o: WindowsThreadStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/WindowsThreadStackCompressor.o WindowsThreadStackCompressor.cpp

${OBJECTDIR}/BackgroundRemovedImage.o: BackgroundRemovedImage.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImage.o BackgroundRemovedImage.cpp

${OBJECTDIR}/ExtraDataWriter.o: ExtraDataWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/ExtraDataWriter.o ExtraDataWriter.cpp

${OBJECTDIR}/tictoc/Timer.o: tictoc/Timer.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/Timer.o tictoc/Timer.cpp

${OBJECTDIR}/NameValueMetaData.o: NameValueMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/NameValueMetaData.o NameValueMetaData.cpp

${OBJECTDIR}/tictoc/tictoc.o: tictoc/tictoc.cpp 
	${MKDIR} -p ${OBJECTDIR}/tictoc
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/tictoc/tictoc.o tictoc/tictoc.cpp

${OBJECTDIR}/CompositeImageMetaData.o: CompositeImageMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/CompositeImageMetaData.o CompositeImageMetaData.cpp

${OBJECTDIR}/StaticBackgroundCompressor.o: StaticBackgroundCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressor.o StaticBackgroundCompressor.cpp

${OBJECTDIR}/StaticBackgroundCompressorLoader.o: StaticBackgroundCompressorLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/StaticBackgroundCompressorLoader.o StaticBackgroundCompressorLoader.cpp

${OBJECTDIR}/BackgroundRemovedImageLoader.o: BackgroundRemovedImageLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/BackgroundRemovedImageLoader.o BackgroundRemovedImageLoader.cpp

${OBJECTDIR}/WindowsThreadedStaticBackgroundCompressor.o: WindowsThreadedStaticBackgroundCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/WindowsThreadedStaticBackgroundCompressor.o WindowsThreadedStaticBackgroundCompressor.cpp

${OBJECTDIR}/MightexMetaData.o: MightexMetaData.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/MightexMetaData.o MightexMetaData.cpp

${OBJECTDIR}/IplImageLoaderFixedWidth.o: IplImageLoaderFixedWidth.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/IplImageLoaderFixedWidth.o IplImageLoaderFixedWidth.cpp

${OBJECTDIR}/ImageMetaDataLoader.o: ImageMetaDataLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/ImageMetaDataLoader.o ImageMetaDataLoader.cpp

${OBJECTDIR}/LinearStackCompressor.o: LinearStackCompressor.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2  -MMD -MP -MF $@.d -o ${OBJECTDIR}/LinearStackCompressor.o LinearStackCompressor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libImage-Stack-Compressor.dll

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
