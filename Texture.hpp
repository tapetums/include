#pragma once

//---------------------------------------------------------------------------//
//
// テクスチャオブジェクト
//  Texture.hpp
//   Copyright (C) 2013-2016 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <vector>

#include <gl/gl.h>
#include <gl/glext.h>
#pragma comment(lib, "opengl32.lib")

//---------------------------------------------------------------------------//

using float64_t = double;

//---------------------------------------------------------------------------//
// 前方宣言
//---------------------------------------------------------------------------//

namespace tapetums
{
    struct TextureDesc;
    class  OpenGLTexture;
}

//---------------------------------------------------------------------------//
// 構造体
//---------------------------------------------------------------------------//

// テクスチャ情報を保持する構造体
struct tapetums::TextureDesc
{
    int32_t   width;
    int32_t   height;
    float64_t dpiX;
    float64_t dpiY;
    int32_t   interpolation_min;
    int32_t   interpolation_max;
    int32_t   repeat_s;
    int32_t   repeat_t;
    int32_t   envi;
};

//---------------------------------------------------------------------------//
// クラス
//---------------------------------------------------------------------------//

class tapetums::OpenGLTexture
{
private:
    TextureDesc          m_desc;
    std::vector<uint8_t> m_buffer;
    uint32_t             m_texture { 0 };

public:
    OpenGLTexture() = default;
    ~OpenGLTexture() { Uninit(); }

    OpenGLTexture(const TextureDesc& desc, const void* const buffer, size_t size)
    { Init(desc, buffer, size); }

public:
    const TextureDesc& desc()    const noexcept { return m_desc; }
    const uint8_t*     buffer()  const noexcept { return m_buffer.data(); }
    size_t             size()    const noexcept { return m_buffer.size(); }
    uint32_t           texture() const noexcept { return m_texture; }

public:
    void Init(const TextureDesc& desc, const void* const buffer, size_t size);
    void Uninit();
};

//---------------------------------------------------------------------------//

inline void tapetums::OpenGLTexture::Init
(
    const TextureDesc& desc, const void* const buffer, size_t size
)
{
    m_desc = desc;

    m_buffer.resize(size);
    ::memcpy(m_buffer.data(), buffer, size);

    // テクスチャを設定
    ::glGenTextures(1, &m_texture);
    if ( m_texture == 0 )
    {
        printf("glGenTextures error: 0x%04x", glGetError());
    }

    ::glBindTexture  (GL_TEXTURE_2D,                        m_texture);
    ::glPixelStorei  (GL_UNPACK_ALIGNMENT,                  32 / 8);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_desc.interpolation_min);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_desc.interpolation_min);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     m_desc.repeat_s);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     m_desc.repeat_t);
    ::glTexEnvi      (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  m_desc.envi);
    ::glTexImage2D
    (
        GL_TEXTURE_2D, 0, GL_RGBA,
        m_desc.width, m_desc.height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, m_buffer.data()
    );
    ::glBindTexture(GL_TEXTURE_2D, 0);
}

//---------------------------------------------------------------------------//

inline void tapetums::OpenGLTexture::Uninit()
{
    if ( m_texture )
    {
        ::glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }

    m_buffer.resize(0);
}

//---------------------------------------------------------------------------//

// Texture.hpp