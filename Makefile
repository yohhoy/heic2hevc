HEIF_LIB=nokiatech-heif

CXXFLAGS = -O2 -std=c++11 -W -Wall
CXXFLAGS += -I$(HEIF_LIB)/srcs/api/common -I$(HEIF_LIB)/srcs/api/reader
LDFLAGS = -L$(HEIF_LIB)/build/srcs/common -L$(HEIF_LIB)/srcs/api/common -L$(HEIF_LIB)/srcs/api/reader -L$(HEIF_LIB)/build/srcs/parser/avcparser -L$(HEIF_LIB)/build/srcs/parser/h265parser -L$(HEIF_LIB)/build/srcs/reader -lcommon -lavcparser -lh265parser -lheifreader

all: heic2hevc

heic2hevc: heic2hevc.cpp $(HEIF_LIB)/build
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(HEIF_LIB)/build: heif-lib

heif-lib:
	@if [ ! -d $(HEIF_LIB)/build ]; then \
		mkdir -p $(HEIF_LIB)/build; \
		fi
	cd $(HEIF_LIB)/build && cmake ../srcs && make

.PHONY: heif-lib
