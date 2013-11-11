
#include "HDRImage.h"

#include <fstream>
#include <memory>

using namespace std;

namespace OmochiRenderer {


  bool HDRImage::ReadFromRadianceFile(const std::string &file) {
    ifstream ifs(file.c_str(), ios::binary);

    if (!ifs || ifs.bad()) {
      return false;
    }

    RGBE_Header header;
    if (!ReadHeaderFromRadianceFile(ifs, header)) return false;
    if (!ReadRLEPixelsFromRadianceFile(ifs, header.width, header.height)) return false;

    return true;
  }

  bool HDRImage::ReadHeaderFromRadianceFile(std::ifstream &ifs, RGBE_Header &header_result) {
    string line;

    header_result.valid = 0;
    header_result.gamma = header_result.exposure = 1.0f;

    std::getline(ifs, line);
    if (line.find("#?") != 0) return false;  // invalid start of radiance files

    header_result.valid |= RGBE_Header::RGBE_VALID_PROGRAMTYPE;
    header_result.programtype = line.substr(2, line.find(' ') - 2);

    std::getline(ifs, line);

    while (!ifs.eof()) {
      if (line.size() == 0 || line[0] == '\n') {
        // blank line
        break;
      }
      if (line.find("FORMAT=32-bit_rle_rgbe") == 0) {
        // support only 32-bit RLE RGBE format
      } else if (line.find("GAMMA=") == 0) {
        istringstream iss(line);
        string str;
        iss >> str >> header_result.gamma;
        header_result.valid |= RGBE_Header::RGBE_VALID_GAMMA;
      }
      else if (line.find("EXPOSURE=") == 0) {
        istringstream iss(line);
        string str;
        iss >> str >> header_result.exposure;
        header_result.valid |= RGBE_Header::RGBE_VALID_EXPOSURE;
      }

      std::getline(ifs, line);
    }
    
    // support only -Y height +X width format
    std::getline(ifs, line); if(ifs.eof()) return false;
    istringstream iss(line);
    string str1, str2;
    iss >> str1 >> header_result.height >> str2 >> header_result.width;

    return true;
  }
  bool HDRImage::ReadPixelsFromRadianceFile(std::ifstream &ifs, int pixel_count, int startoffset) {
    m_image.resize(pixel_count);

    unsigned char rgbe[4];

    for (int i=startoffset; i<pixel_count; i++) {
      if (ifs.eof()) return false;
      ifs.read(reinterpret_cast<char *>(rgbe), sizeof(rgbe));
      RGBE2Color(rgbe, m_image[i]);
    }
    return true;
  }
  bool HDRImage::ReadRLEPixelsFromRadianceFile(std::ifstream &ifs, int width, int height) {

    m_image.resize(width*height);

    if (width < 8 || width > 0x7fff) {
      // run-length encode is not alloweded
      return ReadPixelsFromRadianceFile(ifs, width, height);
    }

    unsigned char rgbe[4];
    char *rgbe_buffer = reinterpret_cast<char *>(rgbe);

    std::vector<unsigned char> line_buffer;
    unsigned char *bptr;

    int imageIndex = 0;

    for (int y = 0; y < height; y++) {
      ifs.read(rgbe_buffer, sizeof(rgbe)); if (ifs.eof()) return false;

      if ((rgbe[0] != 2) || (rgbe[1] != 2) || (rgbe[2] & 0x80)) {
        // not run length encoded
        RGBE2Color(rgbe, m_image[imageIndex]);
        return ReadPixelsFromRadianceFile(ifs, width*height, 1+imageIndex);
      }

      if ((((int)rgbe[2]) << 8 | rgbe[3]) != width) {
        return false;
      }

      line_buffer.resize(4 * width);
      bptr = line_buffer.data();

      // read line
      for (int i = 0; i < 4; i++) {
        unsigned char *bptr_end = line_buffer.data() + (i + 1)*width;
        while (bptr < bptr_end) {
          unsigned char buf[2];
          ifs.read(reinterpret_cast<char *>(buf), sizeof(buf));
          if (ifs.eof()) return false;

          int count;
          if (buf[0] > 128) {
            // buf[1] continues: (buf[0]-128) times
            count = buf[0] - 128;
            if ((count == 0) || (count > bptr_end - bptr)) {
              return false;
            }
            while (count-- > 0) {
              *bptr++ = buf[1];
            }
          } else {
            // not continue
            count = buf[0];
            if ((count == 0) || (count > bptr_end - bptr)) {
              return false;
            }
            *bptr++ = buf[1];
            if (--count > 0) {
              ifs.read(reinterpret_cast<char *>(bptr), sizeof(*bptr)*count);
              bptr += count;
            }
          }
        }
      }

      // convert buffer data to Color
      for (int i = 0; i < width; i++) {
        rgbe[0] = line_buffer[i];
        rgbe[1] = line_buffer[i + width];
        rgbe[2] = line_buffer[i + 2*width];
        rgbe[3] = line_buffer[i + 3*width];

        RGBE2Color(rgbe, m_image[imageIndex]);
        imageIndex++;
      }
    }
    return true;
  }
}
