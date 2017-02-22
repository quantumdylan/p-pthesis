os = $(shell uname -s)

#PYTHIA8DIR = /usr/include/Pythia8
PYTHIA8DIR = /media/scratch/pythia/include

#INCFLAGS      = -I$(ROOTSYS)/include -I$(FASTJETDIR)/include -I$(PYTHIA8DIR)/include -I$(STARPICOPATH)
#INCFLAGS      = -I$(ROOTSYS)/include -I$(FASTJETDIR)/include -I$(PYTHIA8DIR)/include -I$(PYTHIA8DIR)/include/Pythia8/ -I$(STARPICOPATH)
INCFLAGS      = -I$(ROOTSYS)/include  -I$(PYTHIA8DIR) -I$(PYTHIA8DIR)/Pythia8 -I$(FASTJETDIR)/include
INCFLAGS      += -I./src

ifeq ($(os),Linux)
CXXFLAGS      = $(shell root-config --cflags)
else
CXXFLAGS      = -O -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
## for debugging:
# CXXFLAGS      = -g -O0 -fPIC -pipe -Wall -Wno-deprecated-writable-strings -Wno-unused-variable -Wno-unused-private-field -Wno-gnu-static-float-init
endif

ifeq ($(os),Linux)
LDFLAGS       = $(shell root-config --ldflags)
LDFLAGSS      = -g --shared 
else
LDFLAGS       = -O -Xlinker -bind_at_load -flat_namespace
LDFLAGSS      = -flat_namespace -undefined suppress
LDFLAGSSS     = -bundle
endif

ifeq ($(os),Linux)
CXX          = g++ 
else
CXX          = clang
endif


ROOTLIBS      = $(shell root-config --libs)

#LIBPATH       = $(ROOTLIBS) -L$(FASTJETDIR)/lib -L$(PYTHIA8DIR)/lib -L$(STARPICOPATH)
#LIBPATH       = $(ROOTLIBS)  -L$(PYTHIA8DIR)
LIBPATH        = $(ROOTLIBS) -L$(PYTHIA)/lib -L$(FASTJET)/lib
#LIBS          = -lfastjet -lfastjettools -lpythia8  -llhapdfdummy -lTStarJetPico
#LIBS          = $(ROOTLIBS) -L/usr/local/lib -lfastjet -lfastjettools -lpythia8  -llhapdfdummy
LIBS          = $(ROOTLIBS) -L/media/scratch/fastjet/lib -lfastjet -lfastjettools -ldl -L/media/scratch/pythia/lib -lpythia8 -ldl

# for cleanup
SDIR          = src
ODIR          = src/obj
BDIR          = bin


###############################################################################
################### Remake when these headers are touched #####################
###############################################################################
# INCS = $(SDIR)/G_Linkdef.h $(SDIR)/GParticle.h 
INCS = 

###############################################################################
# standard rules
$(ODIR)/%.o : $(SDIR)/%.cxx $(INCS)
	@echo 
	@echo COMPILING
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -c $< -o $@

$(BDIR)/%  : $(ODIR)/%.o 
	@echo 
	@echo LINKING
	$(CXX) $^ $(LDFLAGS) $(LIBS) -o $@

###############################################################################

###############################################################################
############################# Main Targets ####################################
###############################################################################
all    : $(BDIR)/RunPythia \
	 $(BDIR)/RunFastJet	\
	 $(BDIR)/analysis \
	 $(BDIR)/PlotStuff \
	 $(BDIR)/combine
	 
#$(ODIR)/GParticle.o 		: $(SDIR)/GParticle.cxx $(INCS)
#$(ODIR)/GEvent.o 	 	: $(SDIR)/GEvent.cxx $(INCS) $(SDIR)/GEvent.h

$(BDIR)/RunPythia			: $(ODIR)/RunPythia.o $(ODIR)/GParticle.o $(ODIR)/GEvent.o 
$(BDIR)/RunPythia			: $(ODIR)/RunPythia.o $(SDIR)/GParticle_cxx.so $(SDIR)/GEvent_cxx.so 
$(BDIR)/RunFastJet			: $(ODIR)/RunFastJet.o $(SDIR)/GParticle_cxx.so $(SDIR)/GEvent_cxx.so 
$(BDIR)/analysis			: $(ODIR)/analysis.o $(SDIR)/GParticle_cxx.so $(SDIR)/GEvent_cxx.so $(SDIR)/Results_cxx.so 
$(BDIR)/PlotStuff			: $(ODIR)/PlotStuff.o $(SDIR)/GParticle_cxx.so $(SDIR)/GEvent_cxx.so 
$(BDIR)/combine				: $(ODIR)/combine.o $(SDIR)/GParticle_cxx.so $(SDIR)/GEvent_cxx.so $(SDIR)/Results_cxx.so 

$(SDIR)/GParticle_cxx.so 	: $(SDIR)/GParticle.cxx $(INCS)
	./myroothelper.sh src/GParticle.cxx

$(SDIR)/GEvent_cxx.so 	: $(SDIR)/GEvent.cxx $(INCS)
	./myroothelper.sh src/GEvent.cxx
	
$(SDIR)/Results_cxx.so 	: $(SDIR)/Results.cxx $(INCS)
	./myroothelper.sh src/Results.cxx

###############################################################################
##################################### MISC ####################################
###############################################################################

clean :
	@echo 
	@echo CLEANING
	rm -vf $(ODIR)/*.o
	rm -vf $(BDIR)/*
	rm -vf lib/*
	rm -vf src/*_cxx.d src/*_cxx.so


.PHONY : clean 
