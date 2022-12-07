//
// Created by mattia on 07/12/22.
//
#include <cstring>
#include "hevcimagefilereader.hpp"

using namespace std;

unsigned int getRotation(HevcImageFileReader *reader, uint32_t contextId, uint32_t itemId) {
    const auto itemProperties = reader->getItemProperties(contextId, itemId);
    for (const auto& property : itemProperties) {
        if (property.type == ImageFileReaderInterface::ItemPropertyType::IROT) {
            return reader->getPropertyIrot(contextId, property.index).rotation;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " [-info] /path/to/image.heic [/path/to/image.bitstream]" << endl;
        return 1;
    }

    string srcFile;
    string dstFile;

    bool showImageInfo = !strcmp(argv[1], "-info");
    if (showImageInfo) {
        srcFile = argv[2];
    } else {
        srcFile = argv[1];
        dstFile = argv[2];
    }

    HevcImageFileReader reader;
    reader.initialize(srcFile);

    const auto& properties = reader.getFileProperties();

    bool fullImage = properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasSingleImage);
    bool tiledImage = properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasImageCollection);

    if (!properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasRootLevelMetaBox)) {
        fprintf(stderr, "no meta_box\n");
        return 2;
    } else if (!fullImage && ! tiledImage) {
        fprintf(stderr, "no image\n");
        return 3;
    }

    const uint32_t contextId = properties.rootLevelMetaBoxProperties.contextId;

    ImageFileReaderInterface::IdVector itemIds;
    reader.getItemListByType(contextId, "master", itemIds);
    if (itemIds.empty()) {
        fprintf(stderr, "no master_list\n");
        return 4;
    }
    const uint32_t firstItemId = itemIds.at(0);

    auto imageWidth = reader.getWidth(contextId, firstItemId);
    auto imageHeight = reader.getHeight(contextId, firstItemId);

    ImageFileReaderInterface::GridItem gridItem;
    ImageFileReaderInterface::IdVector gridItemIds;
    uint32_t gridItemId = 0;
    if (!fullImage) {
        reader.getItemListByType(contextId, "grid", gridItemIds);
        gridItemId = gridItemIds.at(0);
        gridItem = reader.getItemGrid(contextId, gridItemId);
    }

    if (!fullImage) {
        imageWidth = gridItem.outputWidth;
        imageHeight = gridItem.outputHeight;
    }

    if (showImageInfo) {
        printf("brand=%s\nversion=%d\nwidth=%d\nheight=%d\ncodec=%s\nrotation=%d\ntiles=%ld\n",
               reader.getMajorBrand().c_str(),
               reader.getMinorVersion(),
               imageWidth,
               imageHeight,
               reader.getDecoderCodeType(contextId, firstItemId).c_str(),
               getRotation(&reader, contextId, (fullImage ? firstItemId : gridItemId)),
               itemIds.size()
        );
        if (!fullImage) {
            printf("rows=%d\ncols=%d\n",
                   gridItem.rowsMinusOne+1,
                   gridItem.columnsMinusOne+1
            );
        }
        return 0;
    }

    if (fullImage) { // master
        ImageFileReaderInterface::DataVector data;
        reader.getItemDataWithDecoderParameters(contextId, firstItemId, data);

        auto fd = fopen(dstFile.c_str(), "wb");
        fwrite(data.data(), data.size(), 1, fd);
        fclose(fd);
    } else { // tiles
        char tail[2000];
        for (auto itemId : itemIds) {
            ImageFileReaderInterface::DataVector data;
            reader.getItemDataWithDecoderParameters(contextId, itemId, data);

            snprintf(tail, 2000, ".%03d.tile", itemId);

            auto fd = fopen((dstFile + tail).c_str(), "wb");
            fwrite(data.data(), data.size(), 1, fd);
            fclose(fd);
        }
    }

    return 0;
}
