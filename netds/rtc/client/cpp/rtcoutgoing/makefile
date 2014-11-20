!include <disable.mak>

# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# Copyright (C) 1998-2001  Microsoft Corporation.  All Rights Reserved.
#
#
# Processor independent makefile

# Nmake macros for building Windows 32-Bit apps
!include <win32.mak>

PROJ = RTCOutgoing

all: $(OUTDIR) $(OUTDIR)\$(PROJ).exe 

#----- If OUTDIR does not exist, then create directory
$(OUTDIR) :
    if not exist "$(OUTDIR)/$(NULL)" mkdir $(OUTDIR)

# Define project specific macros
PROJ_OBJS  = $(OUTDIR)\Utils.obj $(OUTDIR)\RTCOutgoing.obj $(OUTDIR)\RTCOutgoingDlg.obj 
BASE_OBJS  =
EXTRA_LIBS = version.lib ole32.lib oleaut32.lib uuid.lib kernel32.lib advapi32.lib 
GLOBAL_DEP = resource.h
RC_DEP     = resource.h


CFLG=$(cflags) -DUNICODE -D_UNICODE -D_AFXDLL


# Inference rule for updating the object files

.cpp{$(OUTDIR)}.obj:
  $(cc) $(cdebug) $(CFLG) $(CVARSDLL) /EHsc /MD /Fo"$(OUTDIR)\\" /Fd"$(OUTDIR)\\" $**


# Build rule for resource file
$(OUTDIR)\$(PROJ).res: $(PROJ).rc $(RC_DEP)
    $(rc) $(rcflags) $(rcvars) /fo $(OUTDIR)\$(PROJ).res $(PROJ).rc


# Build rule for EXE

!ifndef NODEBUG

$(OUTDIR)\$(PROJ).EXE: $(BASE_OBJS) $(PROJ_OBJS) $(OUTDIR)\$(PROJ).res
    $(link) $(linkdebug) $(guilflags) $(guilibsdll)  /entry:"wWinMainCRTStartup" /subsystem:windows \
    /pdb:"$(OUTDIR)\$(PROJ).pdb" /NODEFAULTLIB:LIBCD $(BASE_OBJS) $(PROJ_OBJS) \
    $(OUTDIR)\$(PROJ).res $(guilibs) $(EXTRA_LIBS) -out:$(OUTDIR)\$(PROJ).exe $(MAPFILE)

!else

$(OUTDIR)\$(PROJ).EXE: $(BASE_OBJS) $(PROJ_OBJS) $(OUTDIR)\$(PROJ).res
    $(link) $(linkdebug) $(guilflags) $(guilibsdll)  /entry:"wWinMainCRTStartup" /subsystem:windows \
    /pdb:"$(OUTDIR)\$(PROJ).pdb" /NODEFAULTLIB:LIBC $(BASE_OBJS) $(PROJ_OBJS) \
    $(OUTDIR)\$(PROJ).res $(guilibs) $(EXTRA_LIBS) -out:$(OUTDIR)\$(PROJ).exe $(MAPFILE)

!endif

# Rules for cleaning out those old files
clean:
        $(CLEANUP)
