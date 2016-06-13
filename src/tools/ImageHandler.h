#pragma once

#include "Image.h"

namespace OmochiRenderer
{
  // 画像の入出力、生成を管理するクラス
  class ImageHandler
  {
  public:
    // 画像に対してユニークに付与されるID
    typedef int IMAGE_ID;
    // 有効でない画像ID
    static const IMAGE_ID INVALID_IMAGE_ID = -1;
  private:
    ImageHandler() : m_filenameToImageIndex(), m_images() {}
    ~ImageHandler();

    std::unordered_map<std::string, IMAGE_ID> m_filenameToImageIndex;
    std::vector<Image *> m_images;
  public:
    static ImageHandler & GetInstance() {
      static ImageHandler s;
      return s;
    }

    // 外部ファイルからの読み込み。png, jpg, bmp等に対応 (hdrはここでは非対応)
    IMAGE_ID LoadFromFile(const std::string &fname, bool doReverseGamma = true);
    // 空の画像作成
    IMAGE_ID CreateImage(size_t width, size_t height);
    // 指定したIDに紐づいた実体取得
    Image * GetImage(IMAGE_ID id);
    const Image * GetImage(IMAGE_ID id) const;
    // 画像解放
    void ReleaseImage(IMAGE_ID id);
    // PNG ファイルへの保存
    bool SaveToPngFile(const std::string &fname, const Image *image) const;
    // PPM ファイルへの保存
    bool SaveToPpmFile(const std::string &fname, const Image *image) const;
  };
}
