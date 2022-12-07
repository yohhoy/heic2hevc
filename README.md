# heic2hevc
convert HEIC file to H.265 bitstream(Annex.B)

# Usage

convert HEIC to PNG format (w/ FFmpeg):

```
$ ./heic2hevc input.heic output.265
$ ffmpeg -i output.265 output.png
```

The program will export:
* Exif file: <output_exif.265>
* Cover image: <output.265>
* Master images: <output_master<n>.265>

# Possible errors
## Errors caused by the nokia library
```
error: ‘numeric_limits’ is not a member of ‘std’
```
### solution:
add the following lines:
```
#include <stddef.h>
#include <limits>
```
### reference
[Stack overflow](https://stackoverflow.com/questions/71296302/numeric-limits-is-not-a-member-of-std).

## Error caused by the enviroment

### Create the executable by linking shared library
```shell
gcc -L<path to .SO file> -Wall -o code main.c -l<library name>
```


### Make shared library available at runtime
```shell
export LD_LIBRARY_PATH=<path to .SO file>:$LD_LIBRARY_PATH
```


### Run executable
```shell
./a.out
```
