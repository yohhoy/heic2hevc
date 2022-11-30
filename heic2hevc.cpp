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

using namespace std;
//using namespace HEIF;

//#include "hevcimagefilereader.hpp"
#include "heifreader.h" //Reader API is located here: https://github.com/nokiatech/heif/tree/master/srcs/api/reader in source tree
#include "heifwriter.h"
/*
-l library
           Search the library named library when linking.  (The second
           alternative with the library as a separate argument is only
           for POSIX compliance and is not recommended.)

           The -l option is passed directly to the linker by GCC.  Refer
           to your linker documentation for exact details.  The general
           description below applies to the GNU linker.

           The linker searches a standard list of directories for the
           library.  The directories searched include several standard
           system directories plus any that you specify with -L.

           Static libraries are archives of object files, and have file
           names like liblibrary.a.  Some targets also support shared
           libraries, which typically have names like liblibrary.so.  If
           both static and shared libraries are found, the linker gives
           preference to linking with the shared library unless the
           -static option is used.

           It makes a difference where in the command you write this
           option; the linker searches and processes libraries and
           object files in the order they are specified.  Thus, foo.o
           -lz bar.o searches library z after file foo.o but before
           bar.o.  If bar.o refers to functions in z, those functions
           may not be loaded.


Options for Directory Search
       These options specify directories to search for header files, for
       libraries and for parts of the compiler:




-Ldir
           Add directory dir to the list of directories to be searched
           for -l.
 */





/*
int extract(const char* srcfile, const char* dstfile)
{
    std::cout << "convert " << srcfile << " to " << dstfile << std::endl;

    HevcImageFileReader reader;
    reader.initialize(srcfile);
    const auto& props = reader.getFileProperties();
    const uint32_t contextId = props.rootLevelMetaBoxProperties.contextId;

    HevcImageFileReader::IdVector ids;
    reader.getItemListByType(contextId, "master", ids);
    const uint32_t itemId = ids[0];
    std::cout << "itemId=" << itemId << std::endl;

    HevcImageFileReader::ParameterSetMap paramset;
    reader.getDecoderParameterSets(contextId, itemId, paramset);

    HevcImageFileReader::DataVector bitstream;
    reader.getItemDataWithDecoderParameters(contextId, itemId, bitstream);

    std::ofstream ofs(dstfile, std::ios::binary);
    for (const auto& key : {"VPS", "SPS", "PPS"}) {
        const auto& nalu = paramset[key];
        std::cout << key << " len=" << nalu.size() << std::endl;
        ofs.write((const char *)nalu.data(), nalu.size());
    }
    std::cout << "bitstream=" << bitstream.size() << std::endl;
    ofs.write((const char *)bitstream.data(), bitstream.size());

    return 0;
}
*/
//Access and read a cover image
void example1(){
    auto* reader = HEIF::Reader::Create();
    // Input file available from https://github.com/nokiatech/heif_conformance
    reader->initialize("C003.heic");

    HEIF::FileInformation info;
    reader->getFileInformation(info);

    // Find the item ID
    HEIF::ImageId itemId;
    reader->getPrimaryItem(itemId);

    uint64_t memoryBufferSize = 1024 * 1024;
    uint8_t* memoryBuffer        = new uint8_t[memoryBufferSize];
    reader->getItemDataWithDecoderParameters(itemId, memoryBuffer, memoryBufferSize);
    /*
    // Feed 'data' to decoder and display the cover image...
    std::ofstream ofs(dstfile, std::ios::binary);
    for (const auto& key : {"VPS", "SPS", "PPS"}) {
        const auto& nalu = paramset[key];
        std::cout << key << " len=" << nalu.size() << std::endl;
        ofs.write((const char *)nalu.data(), nalu.size());
    }
    std::cout << "bitstream=" << bitstream.size() << std::endl;
    ofs.write((const char *)bitstream.data(), bitstream.size());
*/
    delete[] memoryBuffer;
    HEIF::Reader::Destroy(reader);
}
/*
//Access and read image item and its thumbnail
void example2()
{
    Array<HEIF::ImageId> itemIds;
    uint64_t itemSize = 1024 * 1024;
    char* itemData    = new char[itemSize];

    auto* reader = Reader::Create();

    // Input file available from https://github.com/nokiatech/heif_conformance
    reader->initialize("C012.heic");
    FileInformation info;
    if (reader->getFileInformation(info) != ErrorCode::OK)
    {
        // handle error here...
        delete[] itemData;
        return;
    }

    // Verify that the file has one or several images in the MetaBox
    if (!(info.features & FileFeatureEnum::HasSingleImage ||
          info.features & FileFeatureEnum::HasImageCollection))
    {
        delete[] itemData;
        return;
    }

    // Find the item ID of the first master image
    reader->getMasterImages(itemIds);
    const ImageId masterId = itemIds[0];

    const auto metaBoxFeatures = info.rootMetaBoxInformation.features;  // For convenience
    if (metaBoxFeatures & MetaBoxFeatureEnum::HasThumbnails)
    {
        // Thumbnail references ('thmb') are from the thumbnail image to the master image
        reader->getReferencedToItemListByType(masterId, "thmb", itemIds);
        const auto thumbnailId = itemIds[0];

        if (reader->getItemDataWithDecoderParameters(thumbnailId.get(), itemData, itemSize) == ErrorCode::OK)
        {
            // ...decode data and display the image, show master image later
        }
    }
    else
    {
        // There was no thumbnail, show just the master image
        if (reader->getItemDataWithDecoderParameters(masterId.get(), itemData, itemSize) == ErrorCode::OK)
        {
            // ...decode and display...
        }
    }

    Reader::Destroy(reader);
    delete[] itemData;
}

//Access and read image items and their thumbnails in a collection
void example3()
{
    auto* reader = Reader::Create();
    typedef ImageId MasterItemId;
    typedef ImageId ThumbnailItemId;
    Array<ImageId> itemIds;
    map<MasterItemId, ThumbnailItemId> imageMap;

    // Input file available from https://github.com/nokiatech/heif_conformance
    reader->initialize("C012.heic");
    FileInformation info;
    reader->getFileInformation(info);

    // Find item IDs of master images
    reader->getMasterImages(itemIds);

    // Find thumbnails for each master. There can be several thumbnails for one master image, get narrowest ones here.
    for (const auto masterId : itemIds)
    {
        // Thumbnail references ('thmb') are from the thumbnail image to the master image
        Array<ImageId> thumbIds;
        reader->getReferencedToItemListByType(masterId, "thmb", thumbIds);

        const ImageId thumbId = *std::min_element(thumbIds.begin(), thumbIds.end(),
                                                  [&](ImageId a, ImageId b) {
                                                      uint32_t widthA, widthB;
                                                      reader->getWidth(a, widthA);
                                                      reader->getWidth(b, widthB);
                                                      return (widthA < widthB);
                                                  });
        imageMap[masterId]    = thumbId;
    }

    // We have now item IDs of thumbnails and master images. Decode and show them from imageMap as wanted.
    Reader::Destroy(reader);
}

//Access and read derived image
void example4()
{
    auto* reader = Reader::Create();
    Array<ImageId> itemIds;

    // Input file available from https://github.com/nokiatech/heif_conformance
    reader->initialize("C008.heic");
    FileInformation info;
    reader->getFileInformation(info);

    // Find item IDs of 'iden' (identity transformation) type derived images
    reader->getItemListByType("iden", itemIds);

    const auto itemId = itemIds[0];  // For demo purposes, assume there was one 'iden' item

    // 'dimg' item reference points from the 'iden' derived item to the input(s) of the derivation
    Array<ImageId> referencedImages;
    reader->getReferencedFromItemListByType(itemId, "dimg", referencedImages);
    const ImageId sourceItemId = referencedImages[0];  // For demo purposes, assume there was one

    // Get 'iden' item properties to find out what kind of derivation it is
    Array<ItemPropertyInfo> propertyInfos;
    reader->getItemProperties(itemId, propertyInfos);

    unsigned int rotation = 0;
    for (const auto& property : propertyInfos)
    {
        // For example, handle 'irot' transformational property is anti-clockwise rotation
        if (property.type == ItemPropertyType::IROT)
        {
            // Get property struct by index to access rotation angle
            Rotate irot;
            reader->getProperty(property.index, irot);
            rotation = irot.angle;
            break;  // Assume only one property
        }
    }

    cout << "To render derived image item ID " << itemId << ":" << endl;
    cout << "-retrieve data for source image item ID " << sourceItemId << endl;
    cout << "-rotating image " << rotation << " degrees." << endl;

    Reader::Destroy(reader);
}

//Access and read media track samples, thumbnail track samples and timestamps
void example5()
{
    auto* reader = Reader::Create();
    Array<uint32_t> itemIds;

    // Input file available from https://github.com/nokiatech/heif_conformance
    reader->initialize("C032.heic");
    FileInformation info;
    reader->getFileInformation(info);

    // Print information for every track read
    for (const auto& trackProperties : info.trackInformation)
    {
        const auto sequenceId = trackProperties.trackId;
        cout << "Track ID " << sequenceId << endl;  // Context ID corresponds to the track ID

        if (trackProperties.features & TrackFeatureEnum::IsMasterImageSequence)
        {
            cout << "This is a master image sequence." << endl;
        }

        if (trackProperties.features & TrackFeatureEnum::IsThumbnailImageSequence)
        {
            // Assume there is only one type track reference, so check reference type and master track ID(s) from the first one.
            const auto tref = trackProperties.referenceTrackIds[0];
            cout << "Track reference type is '" << tref.type.value << "'" << endl;
            cout << "This is a thumbnail track for track ID ";
            for (const auto masterTrackId : tref.trackIds)
            {
                cout << masterTrackId << endl;
            }
        }

        Array<TimestampIDPair> timestamps;
        reader->getItemTimestamps(sequenceId, timestamps);
        cout << "Sample timestamps:" << endl;
        for (const auto& timestamp : timestamps)
        {
            cout << " Timestamp=" << timestamp.timeStamp << "ms, sample ID=" << timestamp.itemId << endl;
        }

        for (const auto& sampleProperties : trackProperties.sampleProperties)
        {
            // A sample might have decoding dependencies. The simplest way to handle this is just to always ask and
            // decode all dependencies.
            Array<SequenceImageId> itemsToDecode;
            reader->getDecodeDependencies(sequenceId, sampleProperties.sampleId, itemsToDecode);
            for (auto dependencyId : itemsToDecode)
            {
                uint64_t size    = 1024 * 1024;
                char* sampleData = new char[size];
                reader->getItemDataWithDecoderParameters(sequenceId, dependencyId, sampleData, size);

                // Feed data to decoder...

                delete[] sampleData;
            }
            // Store or show the image...
        }
    }
    Reader::Destroy(reader);
}
//Access and read media alternative
void example6()
{
    auto* reader = Reader::Create();
    Array<uint32_t> itemIds;

    // Input file available from https://github.com/nokiatech/heif_conformance
    reader->initialize("C032.heic");
    FileInformation info;
    reader->getFileInformation(info);

    SequenceId trackId = 0;

    if (info.trackInformation.size > 0)
    {
        const auto& trackInfo = info.trackInformation[0];
        trackId               = trackInfo.trackId;

        if (trackInfo.features & TrackFeatureEnum::HasAlternatives)
        {
            const SequenceId alternativeTrackId = trackInfo.alternateTrackIds[0];  // Take the first alternative
            uint32_t alternativeWidth;
            reader->getDisplayWidth(alternativeTrackId, alternativeWidth);
            uint32_t trackWidth;
            reader->getDisplayWidth(trackId, trackWidth);

            if (trackWidth > alternativeWidth)
            {
                cout << "The alternative track has wider display width, let's use it from now on..." << endl;
                trackId = alternativeTrackId;
            }
        }
    }
    Reader::Destroy(reader);
}

//Read Exif metadata
void example8()
{
    auto* reader = Reader::Create();

    // Try opening a file with an "Exif" item.
    // The file is available from https://github.com/nokiatech/heif_conformance
    if (reader->initialize("C034.heic") != ErrorCode::OK)
    {
        return;
    }

    FileInformation fileInfo{};
    reader->getFileInformation(fileInfo);

    // Find the primary item ID.
    ImageId primaryItemId;
    reader->getPrimaryItem(primaryItemId);

    // Find item(s) referencing to the primary item with "cdsc" (content describes) item reference.
    Array<ImageId> metadataIds;
    reader->getReferencedToItemListByType(primaryItemId, "cdsc", metadataIds);
    ImageId exifItemId = metadataIds[0];

    // Optional: verify the item ID we got is really of "Exif" type.
    FourCC itemType;
    reader->getItemType(exifItemId, itemType);
    if (itemType != "Exif")
    {
        return;
    }

    // Get item size from parsed information. For simplicity, assume it is the first and only non-image item in the file.
    if (fileInfo.rootMetaBoxInformation.itemInformations[0].itemId != exifItemId)
    {
        return;
    }
    auto itemSize = fileInfo.rootMetaBoxInformation.itemInformations[0].size;

    // Request item data.
    char* memoryBuffer = new char[itemSize];
    reader->getItemData(metadataIds[0], memoryBuffer, itemSize);

    // Write Exif item data to a file.
    // Note this data does not contain Exif payload only. The payload is preceded by 4-byte exif_tiff_header_offset field
    // as defined by class ExifDataBlock() in ISO/IEC 23008-12:2017.
    ofstream file("exifdata.bin", ios::out | ios::binary);
    file.write(memoryBuffer, static_cast<std::streamsize>(itemSize));
    delete[] memoryBuffer;

    Reader::Destroy(reader);
}
 */
 /*
int main(int argc, char* argv[])
{

    if (argc < 3) {
        std::cout
            << "Usage: heic2hevc <input.heic> <output.265>" << std::endl;
        return 0;
    }
    return extract(argv[1], argv[2]);

}
*/
int main()
{

    example1();
    return 0;
}