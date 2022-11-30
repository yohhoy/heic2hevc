HEIF_LIB=./nokiatech-heif

CXXFLAGS = -O2 -std=c++11 -W -Wall
CXXFLAGS += -I$(HEIF_LIB)/srcs/api/common -I$(HEIF_LIB)/srcs/api/reader -I$(HEIF_LIB)/srcs/api/writer
LDFLAGS = -L$(HEIF_LIB)/build/reader/CMakeFiles/heif_shared.dir -L$(HEIF_LIB)/build/reader/CMakeFiles/heif_static.dir -L$(HEIF_LIB)/build/api-cpp/CMakeFiles/heifpp.dir -L$(HEIF_LIB)/build/common/CMakeFiles/common.dir -L$(HEIF_LIB)/build/lib  -L$(HEIF_LIB)/build/writer/CMakeFiles/heif_writer_shared.dir -L$(HEIF_LIB)/build/writer/CMakeFiles/heif_writer_static.dir  -lheif_shared -lheif_writer_shared

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
