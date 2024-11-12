#pragma once
#include "Texture.hpp"
#include "GL.hpp"
#include <ft2build.h>

#include <hb.h>
#include <hb-ft.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <vector>

/**
 * Handles loading fonts and uploading textures to GL, 
 * adpated from https://learnopengl.com/In-Practice/Text-Rendering
 * 
 */
struct Font {
    struct Character {
        std::vector<uint8_t> bitmap;  // ID handle of the glyph texture
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        uint32_t Advance;    // Offset to advance to next glyph
    };

    static std::unordered_map<char, Character> characters;

    Texture create_text(const std::string &, std::unordered_map<char, Character> &) const;

    static std::unordered_map<std::string, Texture> texts;

    Texture& get_text(const std::string &) const;

    Font(std::string font_path);
};