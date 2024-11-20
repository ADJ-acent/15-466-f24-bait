#include "Font.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <iostream>

Load< Font > font(LoadTagDefault, []() -> Font const * {
	return new Font(data_path("font/Fredoka-Medium.ttf"));
});
std::unordered_map<char, Font::Character> Font::characters;
std::unordered_map<std::string, Texture> Font::texts;

Font::Font(std::string font_path)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
    }

    FT_Face face;
    if (FT_New_Face(ft, font_path.c_str(), 0, &face))
    {
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        throw std::runtime_error("ERROR::FREETYTPE: Failed to load Glyph");  
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // create the bitmap vector
        std::vector<uint8_t> bitmap;
        uint32_t bitmap_size = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        bitmap.resize(bitmap_size);
        memcpy(bitmap.data(), face->glyph->bitmap.buffer, bitmap_size);
        // now store character for later use
        Character character = {
            bitmap, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            uint32_t(face->glyph->advance.x)
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

}

Texture Font::create_text(const std::string & string_, std::unordered_map<char, Character> &) const 
{
    Texture texture = Texture();
    int current_x_offset = 0;
    int max_ascent = 0;
    int max_descent = 0;
    for (const char& c : string_) {
        if (characters.find(c) != characters.end()) {
            const Character& ch = characters.at(c);
            texture.width += (ch.Advance >> 6); // Advance is in 1/64th pixels, so shift right by 6
            max_ascent = std::max(max_ascent, ch.Size.y + ch.Bearing.y);
            max_descent = std::max(max_descent, ch.Size.y - ch.Bearing.y);
        }
    }
    texture.height = max_ascent;

    std::vector<uint8_t> composite(texture.width * texture.height, 0);

    for (const char& c : string_) {
        if (characters.find(c) != characters.end()) {
            const Character& ch = characters.at(c);
            for (int y = 0; y < ch.Size.y; ++y) {
                for (int x = 0; x < ch.Size.x; ++x) {
                    int composite_x = current_x_offset + x + ch.Bearing.x;
                    int composite_y = texture.height-((max_ascent - ch.Bearing.y) + y) + max_descent;
                    if (composite_x >= 0 && composite_x < texture.width && composite_y >= 0 && composite_y < texture.height) {
                        composite[composite_y * texture.width + composite_x] = ch.bitmap[y * ch.Size.x + x];
                    }
                }
            }

            current_x_offset += (ch.Advance >> 6);
        }
    }

    // submit to gl
    glGenTextures(1, &texture.handle);
    glBindTexture(GL_TEXTURE_2D, texture.handle);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        texture.width,
        texture.height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        composite.data()
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

Texture* Font::get_text(const std::string &string_) const
{
    if (auto res = texts.find(string_); res != texts.end()) {
        return &res->second; // Return a pointer to the existing texture
    }

    auto inserted_res = texts.insert({string_, create_text(string_, characters)});
    return &inserted_res.first->second; // Return a pointer to the newly inserted texture
}