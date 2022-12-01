/*
 * heic2hevc.cpp -- convert HEIC file to H.265 bitstream(Annex.B)
 * Copyright(c) 2017 yohhoy
 *
 * depends https://github.com/nokiatech/heif
 */
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>

using namespace std;
//using namespace HEIF;
//#include "hevcimagefilereader.hpp"
#include "heifreader.h" //Reader API is located here: https://github.com/nokiatech/heif/tree/master/srcs/api/reader in source tree
#include "heifwriter.h"


int exportExif(const char *filename, const char *dstfile) {
    auto *reader = HEIF::Reader::Create();

    // Try opening a file with an "Exif" item.
    if (reader->initialize(filename) != HEIF::ErrorCode::OK) {
        HEIF::Reader::Destroy(reader);
        return -1;
    }

    HEIF::FileInformation fileInfo{};
    reader->getFileInformation(fileInfo);

    // Find the primary item ID.
    HEIF::ImageId primaryItemId;
    reader->getPrimaryItem(primaryItemId);

    // Find item(s) referencing to the primary item with "cdsc" (content describes) item reference.
    HEIF::Array<HEIF::ImageId> metadataIds;
    reader->getReferencedToItemListByType(primaryItemId, "cdsc", metadataIds);
    HEIF::ImageId exifItemId = metadataIds[0];

    // Optional: verify the item ID we got is really of "Exif" type.
    HEIF::FourCC itemType;
    reader->getItemType(exifItemId, itemType);
    if (itemType != "Exif") {
        return -1;
    }

    // Get item size from parsed information.
    const auto it = std::find_if(fileInfo.rootMetaBoxInformation.itemInformations.begin(),
                                 fileInfo.rootMetaBoxInformation.itemInformations.end(),
                                 [exifItemId](HEIF::ItemInformation a) { return a.itemId == exifItemId; });
    if (it == fileInfo.rootMetaBoxInformation.itemInformations.end()) {
        HEIF::Reader::Destroy(reader);
        return -1;
    }
    auto itemSize = it->size;

    // Request item data.
    auto *memoryBuffer = new uint8_t[itemSize];
    reader->getItemData(metadataIds[0], memoryBuffer, itemSize);

    // Write Exif item data to a file.
    // Note this data does not contain Exif payload only. The payload is preceded by 4-byte exif_tiff_header_offset
    // field as defined by class ExifDataBlock() in ISO/IEC 23008-12:2017.
    std::cout
            << "Note this data does not contain Exif payload only. The payload is preceded by 4-byte exif_tiff_header_offset"
            << std::endl;
    ofstream file(dstfile, ios::out | ios::binary);
    file.write(reinterpret_cast<char *>(memoryBuffer), static_cast<std::streamsize>(itemSize));
    delete[] memoryBuffer;

    HEIF::Reader::Destroy(reader);
    return 0;
}

void exportCoverImage(const char *srcfile, const char *dstfile) {
    auto *reader = HEIF::Reader::Create();
    reader->initialize(srcfile);

    HEIF::FileInformation info;
    reader->getFileInformation(info);

    // Find the item ID
    HEIF::ImageId itemId;
    reader->getPrimaryItem(itemId);
    std::cout << "static constructor\n";
    uint64_t memoryBufferSize = 1024 * 1024;
    uint8_t *memoryBuffer = new uint8_t[memoryBufferSize];
    reader->getItemDataWithDecoderParameters(itemId, memoryBuffer, memoryBufferSize);

    std::ofstream ofs(dstfile, std::ios::binary);

    std::cout << "bitstream=" << memoryBufferSize << std::endl;
    ofs.write((const char *) memoryBuffer, memoryBufferSize);
    delete[] memoryBuffer;
    HEIF::Reader::Destroy(reader);
}

void exportImageItemsAndThumbnails(const char *srcfile, const char *dstfile) {
    auto *reader = HEIF::Reader::Create();
    typedef HEIF::ImageId MasterItemId;
    typedef HEIF::ImageId ThumbnailItemId;
    HEIF::Array<HEIF::ImageId> itemIds;
    map<MasterItemId, ThumbnailItemId> imageMap;

    if (reader->initialize(srcfile) != HEIF::ErrorCode::OK) {
        HEIF::Reader::Destroy(reader);
        return;
    }

    HEIF::FileInformation info;
    reader->getFileInformation(info);

    // Find item IDs of master images
    reader->getMasterImages(itemIds);

    // Find thumbnails for each master. There can be several thumbnails for one master image, get narrowest ones
    // here.
    for (const auto masterId: itemIds) {
        // Thumbnail references ('thmb') are from the thumbnail image to the master image
        HEIF::Array<HEIF::ImageId> thumbIds;
        reader->getReferencedToItemListByType(masterId, "thmb", thumbIds);

        const auto thumbId = std::min_element(thumbIds.begin(), thumbIds.end(), [&](HEIF::ImageId a, HEIF::ImageId b) {
            uint32_t widthA, widthB;
            reader->getWidth(a, widthA);
            reader->getWidth(b, widthB);
            return (widthA < widthB);
        });
        if (thumbId != thumbIds.end())  // For images without thumbnail thumbId equals to end()
        {
            imageMap[masterId] = thumbId->get();
        }
    }
    int count=0;
    for(const auto masterId:itemIds){
        uint64_t memoryBufferSize = 1024 * 1024;
        uint8_t *memoryBuffer = new uint8_t[memoryBufferSize];
        reader->getItemDataWithDecoderParameters(masterId, memoryBuffer, memoryBufferSize);
        string dstfileString=dstfile;
        dstfileString.append(".master_"+count);
        std::ofstream ofs(dstfileString.c_str(), std::ios::binary);

        std::cout << "bitstream=" << memoryBufferSize << std::endl;
        ofs.write((const char *) memoryBuffer, memoryBufferSize);
        count++;
    }
    // We have now item IDs of thumbnails and master images. Decode and show them from imageMap as wanted.

    HEIF::Reader::Destroy(reader);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout
                << "Usage: heic2hevc <input.heic> <output.265>" << std::endl;
        return 0;
    }
    string dstFile_exif = argv[2];
    dstFile_exif = dstFile_exif.find('.') + "_exifdata.bin";
    exportExif(argv[1], dstFile_exif.c_str());
    exportCoverImage(argv[1], argv[2]);
    exportImageItemsAndThumbnails(argv[1], argv[2]);
    return 0;
}

