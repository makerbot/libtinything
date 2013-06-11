

This is a sketch of a spec that will be fleshed out as the libtinything class is formed.

incrementally parse a zip file - take individual files out, incrementally unzip parts of json-toolpath file : use miniunzip

//make a tinything file
Create (metafile, jsontoolpath, small_png, mid_png, large_png)

//add things to the json file
MetaJsonUnion (path to json meta, overwrite?)  // unions with the current meta

//? a verifier for the meta to check that required keys exist

Package() - create zip of files 
Verify() - check that filepaths are not NULL

//Pull specified file out of package
file_handle - GetBigThumbnail()
file_handle - GetSmallThumbnail()
file_handle - GetMetaData()

// get incremental pieces of the json toolpath
GetIncrementalParse () - this function keeps of the location in the file, each call returns the next XX bytes - when there are no more bytes, returns eg -1 / nothing parsed


//Phase 2
generating png from stls - (use Thingiverse thingy or ASSIMP?)
