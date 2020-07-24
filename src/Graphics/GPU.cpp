/**************************************************************************

GPU.hpp

Created by Matthew Clarke

Rhe graphics control, processing and output system for the emulator

 **************************************************************************/

#include "GPU.hpp"

#include "../COutSys.hpp"

namespace ig = icarus::graphics;

sf::Font ig::GPU::Font;
bool ig::GPU::FontLoaded = false;

void ig::GPU::InitializeFont() {
	if (!Font.loadFromFile("Consolas.ttf")) {
		icarus::COutSys::Println("GPU failed to load Consolas.ttf font", icarus::COutSys::LEVEL_ERR);
		return;
	}
	FontLoaded = true;
}

ig::GPU::GPU() {
	m_resMode = GPU::GPUResMode::MEMORY_TXT80x25;
	m_colorMode = GPU::GPUColorMode::MONO;
	m_memoryBase = 0x8000;

	m_texture.create(640, 400);
	m_textObj.setFont(Font);
	m_textObj.setCharacterSize(16);
	m_textObj.setString("01234567890123456789012345678901234567890123456789012345678901234567890123456789");
}

void ig::GPU::initialize() {

}

sf::RenderTexture& ig::GPU::getTexture() {
	return m_texture;
}