#pragma once

/**************************************************************************

GPU.hpp

Created by Matthew Clarke

Rhe graphics control, processing and output system for the emulator

 **************************************************************************/

#include "../Bus/Bus.hpp"
#include "../Memory/MMU.hpp"
#include "../Type.hpp"

#include "SFML/Graphics.hpp"

namespace icarus{

	namespace graphics {

		class GPU {
		public:
			enum class GPUResMode {
				MEMORY_TXT80x25
			};
			enum class GPUColorMode {
				MONO
			};

			static sf::Font Font;
			static bool FontLoaded;
			static void InitializeFont();

			/*
			Default constructor
			Initializes in MEMORY_80x25 and MONO mode, with base at 0x8000 (codegolf spec that was used to build this emulator initially)
			*/
			GPU();

			unsigned int getXWidth();
			unsigned int getYWidth();

			/*
			sf::Texture& getTexture()
			Returns the texture for rendering
			*/
			sf::RenderTexture& getTexture();

			/*
			void update()
			Allows the GPU to consider updating
			*/
			void update(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus, icarus::memory::MMU& mmu) {
				uint32_t dBusOld = dBus.readData();
				uint32_t aBusOld = aBus.readData();
				
				if (m_updateClock.getElapsedTime().asMicroseconds() >= m_updateIntervalMicros) {
					m_updateClock.restart();

					// Update the graphics texture
					m_texture.clear(sf::Color::Blue);

					if (m_resMode == GPUResMode::MEMORY_TXT80x25 && m_colorMode == GPUColorMode::MONO) {
						m_textObj.setFillColor(sf::Color::White);
						float y = -2.0f;
						uint8_t v = 0;
						for (int i = 0; i < 25; i++) {
							// Compile the string
							std::string lineStr = "";
							for (int y = 0; y < 80; y++) {
								aBus.putData(m_memoryBase + (i * 80) + y);
								mmu.readByte(dBus, aBus);
								v = (uint8_t)dBus.readData();
								if (v < ' ' || v > '~')
									v = ' ';
								lineStr += v;
							}
							m_textObj.setString(lineStr);
							m_textObj.setPosition(0, y); y += m_textObj.getCharacterSize();
							m_texture.draw(m_textObj);
						}

					}

					m_texture.display();
				}

				// We preserve the values before we screwed with them drawing the graphics
				dBus.putData(dBusOld);
				aBus.putData(aBusOld);
			}
			
		private:
			static constexpr int FONT_PT = 16;

			void initialize();

			sf::Image m_buffer;
			sf::RenderTexture m_texture;
			sf::Text m_textObj;

			unsigned int m_xWidth = 0;
			unsigned int m_yWidth = 0;

			GPUResMode m_resMode;
			GPUColorMode m_colorMode;
			size_t m_memoryBase = 0;

			sf::Clock m_updateClock;
			sf::Int64 m_updateIntervalMicros = 10000;
		};

	}

}