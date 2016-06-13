#include "stdafx.h"

#include "HDRImage.h"

#include <fstream>

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

    m_imageInfo = header;
    m_width = header.width;
    m_height = header.height;
    return true;
  }

  bool HDRImage::WriteToRadianceFile(const std::string &file) {
    ofstream ofs(file.c_str(), ios::binary);

    if (!ofs || ofs.bad()) {
      return false;
    }

    m_imageInfo.width  = static_cast<int>(m_width);
    m_imageInfo.height = static_cast<int>(m_height);
    /*m_imageInfo.exposure = 0.0;
    m_imageInfo.valid |= RGBE_Header::RGBE_VALID_EXPOSURE;
    m_imageInfo.gamma = 1.0;
    m_imageInfo.valid |= RGBE_Header::RGBE_VALID_GAMMA;*/
    m_imageInfo.programtype = "RADIANCE";
    m_imageInfo.valid |= RGBE_Header::RGBE_VALID_PROGRAMTYPE;
    
    if (!WriteHeaderToRadianceFile(ofs, m_imageInfo)) return false;
    if (!WritePixels_RLE(ofs, this->m_image.data(), static_cast<int>(m_width), static_cast<int>(m_height))) return false;

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

  bool HDRImage::WriteHeaderToRadianceFile(std::ofstream &ifs, const RGBE_Header &header)
  {
    std::string programtype = "RGBE";

    if (header.valid & RGBE_Header::RGBE_VALID_PROGRAMTYPE)
      programtype = header.programtype;
    
    ifs << "#?" << programtype << "\n";

    /* The #? is to identify file type, the programtype is optional. */
    if (header.valid & RGBE_Header::RGBE_VALID_GAMMA) {
      ifs << "GAMMA=" << header.gamma << "\n";
    }
    if (header.valid & RGBE_Header::RGBE_VALID_EXPOSURE) {
      ifs << "EXPOSURE=" << header.exposure << "\n";
    }
    ifs << "FORMAT=32-bit_rle_rgbe\n\n";

    ifs << "-Y " << header.height << " +X " << header.width << "\n";

    return true;
  }

  bool HDRImage::WritePixelsToRadianceFile(std::ofstream &ofs, const Color *data, int numpixels)
  {
    unsigned char rgbe[4];

    while (numpixels-- > 0) {
      Color2RGBE(rgbe, *data);
      data++;

      ofs.write(reinterpret_cast<char *>(&rgbe[0]), sizeof(rgbe));
    }
    return true;
  }

  bool HDRImage::WriteBytes_RLE(std::ofstream &ofs, unsigned char *data, int numbytes)
  {
    const static int MINRUNLENGTH = 4;

    int cur, beg_run, run_count, old_run_count, nonrun_count;
    unsigned char buf[2];

    cur = 0;
    while (cur < numbytes) {
      beg_run = cur;
      /* find next run of length at least 4 if one exists */
      run_count = old_run_count = 0;
      while ((run_count < MINRUNLENGTH) && (beg_run < numbytes)) {
        beg_run += run_count;
        old_run_count = run_count;
        run_count = 1;
        while ((beg_run + run_count < numbytes) && 
          (data[beg_run] == data[beg_run + run_count]) &&
          (run_count < 127))
          run_count++;
      }
      /* if data before next big run is a short run then write it as such */
      //if ((old_run_count > 1) && (old_run_count == beg_run - cur) && cur < numbytes) {
      //  buf[0] = 128 + old_run_count;   /*write short run*/
      //  buf[1] = data[cur];
      //  ofs.write(reinterpret_cast<const char *>(buf), sizeof(buf[0]));
      //  cur = beg_run;
      //}
      /* write out bytes until we reach the start of the next run */
      while (cur < beg_run && cur < numbytes) {
        nonrun_count = beg_run - cur;
        if (nonrun_count > 128)
          nonrun_count = 128;
        buf[0] = nonrun_count;
        ofs.write(reinterpret_cast<const char *>(buf), sizeof(buf[0]));
        ofs.write(reinterpret_cast<const char *>(&data[cur]), sizeof(data[0])*nonrun_count);
        cur += nonrun_count;
      }
      /* write out next run if one was found */
      if (run_count >= MINRUNLENGTH && beg_run < numbytes) {
        buf[0] = 128 + run_count;
        buf[1] = data[beg_run];
        ofs.write(reinterpret_cast<const char *>(buf), sizeof(buf[0]) * 2);
        cur += run_count;
      }
    }
    return true;

  }

  bool HDRImage::WritePixels_RLE(std::ofstream &ofs, const Color *data, int scanline_width,
    int num_scanlines)
  {
    unsigned char rgbe[4];
    unsigned char *buffer;
    int i;

    if ((scanline_width < 8) || (scanline_width > 0x7fff))
      /* run length encoding is not allowed so write flat*/
      return WritePixelsToRadianceFile(ofs, data, scanline_width*num_scanlines);
    buffer = (unsigned char *)malloc(sizeof(unsigned char)* 4 * scanline_width);
    if (buffer == NULL)
      /* no buffer space so write flat */
      return WritePixelsToRadianceFile(ofs, data, scanline_width*num_scanlines);
    while (num_scanlines-- > 0) {
      rgbe[0] = 2;
      rgbe[1] = 2;
      rgbe[2] = static_cast<unsigned char>(scanline_width >> 8);
      rgbe[3] = scanline_width & 0xFF;
      ofs.write(reinterpret_cast<const char *>(rgbe), sizeof(rgbe));

      for (i = 0; i<scanline_width; i++) {
        Color2RGBE(rgbe, *data);
        buffer[i] = rgbe[0];
        buffer[i + scanline_width] = rgbe[1];
        buffer[i + 2 * scanline_width] = rgbe[2];
        buffer[i + 3 * scanline_width] = rgbe[3];
        data++;
      }
      /* write out each of the four channels separately run length encoded */
      /* first red, then green, then blue, then exponent */
      for (i = 0; i<4; i++) {
        if (!WriteBytes_RLE(ofs, &buffer[i*scanline_width], scanline_width)) {
          free(buffer);
          return false;
        }
      }
    }
    free(buffer);
    return true;
  }
}
