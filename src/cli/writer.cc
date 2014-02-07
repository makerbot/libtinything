// Copyright 2014 MakerBot Industries

#include <cstdio>

#include "tinything/TinyThingWriter.hh"

enum ExitCode {
  ExitCodeSuccess = 0,
  ExitCodeInvalidArgs,
  ExitCodeException
};

static void printUsage() {
  printf(
      "usage: makerbot_tinything_writer "
      "<output> "
      "<toolpath> "
      "<metadata> "
      "<thumbnails>\n\n"
      "<output> is the path of the \".makerbot\" file that will be written\n"
      "<toolpath> is the \"print.jsontoolpath\" input file\n"
      "<metadata> is the \"meta.json\" input file\n"
      "<thumbnails> is the *directory* containing the three PNGs\n");
}

int main(int argc, char **argv) {
  try {
    if (argc == 5) {
      // Parse arguments
      const std::string outputPath(argv[1]);
      const std::string toolpathPath(argv[2]);
      const std::string metaJsonPath(argv[3]);
      const std::string thumbnailsPath(argv[4]);

      // Prepare writer
      LibTinyThing::TinyThingWriter writer(outputPath);
      writer.setThumbnailDirectory(thumbnailsPath);
      writer.setToolpathFile(toolpathPath);
      writer.setMetadataFile(metaJsonPath);

      // Ship it
      writer.zip();

      return ExitCodeSuccess;
    } else {
      printUsage();
      printf("\nerror: wrong number of arguments\n");
      return ExitCodeInvalidArgs;
    }
  } catch (const std::exception &e) {
    // NOLINT
    fprintf(stderr, "unhandled exception: %s\n", e.what());
    return ExitCodeException;
  }
}
