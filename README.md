# heic2hevc
convert HEIC file to H.265 bitstream(Annex.B)

# usage

convert HEIC to PNG format (w/ FFmpeg):

```
$ ./heic2hevc input.heic output.265
$ ffmpeg -i output.265 output.png
```

# possible errors
```
error: ‘numeric_limits’ is not a member of ‘std’
```
solution:

add the following lines:
```
#include <stddef.h>
#include <limits>
```
[Stack overflow](https://stackoverflow.com/questions/71296302/numeric-limits-is-not-a-member-of-std).