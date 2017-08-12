HEIF_LIB=nokiatech-heif

CXXFLAGS = -O2 -std=c++11 -W -Wall
CXXFLAGS += -I$(HEIF_LIB)/Srcs/common -I$(HEIF_LIB)/Srcs/reader
LDFLAGS = -L$(HEIF_LIB)/build/Srcs/common -L$(HEIF_LIB)/build/Srcs/parser/avcparser -L$(HEIF_LIB)/build/Srcs/parser/h265parser -L$(HEIF_LIB)/build/Srcs/reader -lcommon -lavcparser -l h265parser -lheifreader

all: heic2hevc

heic2hevc: heic2hevc.cpp $(HEIF_LIB)/build
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(HEIF_LIB)/build: heif-lib

heif-lib:
	@if [ ! -d $(HEIF_LIB)/build ]; then \
		mkdir -p $(HEIF_LIB)/build; \
		fi
	cd $(HEIF_LIB)/build && cmake .. && make

.PHONY: heif-lib
