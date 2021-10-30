#ifndef BITMAPFONTLOADER_H
#define BITMAPFONTLOADER_H

#include "Resources/ResourceLoader.h"

#include <fstream>

#include "Core/Interfaces/Object.h"

#include "Renderer/Render/Fonts/BitmapFont.h"
#include "Renderer/Render/Fonts/BitmapFontInfo.h"
#include "Renderer/Render/Fonts/BitmapFontCommon.h"
#include "Renderer/Render/Fonts/BitmapFontCharset.h"
#include "Renderer/Render/Fonts/BitmapFontChar.h"

class BitmapFontLoader : public ResourceLoader
{
public:
	virtual Object* Load (const std::string& filename);
private:
	void ProcessBitmapFontInfo (std::ifstream& file, BitmapFont* font);
	void ProcessBitmapFontCommon (std::ifstream& file, BitmapFont* font);
	void ProcessBitmapFontPage (std::ifstream& file, BitmapFont* font, 
		const std::string& filename);
	void ProcessBitmapFontChar (std::ifstream& file, BitmapFont* font);
	void ProcessComment (std::ifstream& file);

	void ProcessFace (const std::string& data, BitmapFontInfo* info);
	void ProcessSize (const std::string& data, BitmapFontInfo* info);
	void ProcessCharset (const std::string& data, BitmapFontInfo* info);
	void ProcessHeightStretch (const std::string& data, BitmapFontInfo* info);
	void ProcessSuperSampling (const std::string& data, BitmapFontInfo* info);
	void ProcessPadding (const std::string& data, BitmapFontInfo* info);
	void ProcessSpacing (const std::string& data, BitmapFontInfo* info);
	void ProcessOutline (const std::string& data, BitmapFontInfo* info);
	void ProcessBold (const std::string& data, BitmapFontInfo* info);
	void ProcessItalic (const std::string& data, BitmapFontInfo* info);
	void ProcessUnicode (const std::string& data, BitmapFontInfo* info);
	void ProcessSmooth (const std::string& data, BitmapFontInfo* info);

	void ProcessLineHeight (const std::string& data, BitmapFontCommon* common);
	void ProcessBase (const std::string& data, BitmapFontCommon* common);
	void ProcessScaleWidth (const std::string& data, BitmapFontCommon* common);
	void ProcessScaleHeight (const std::string& data, BitmapFontCommon* common);
	void ProcessPages (const std::string& data, BitmapFontCommon* common);
	void ProcessPacked (const std::string& data, BitmapFontCommon* common);
	void ProcessAlphaChannel (const std::string& data, BitmapFontCommon* common);
	void ProcessRedChannel (const std::string& data, BitmapFontCommon* common);
	void ProcessGreenChannel (const std::string& data, BitmapFontCommon* common);
	void ProcessBlueChannel (const std::string& data, BitmapFontCommon* common);
	
	void ProcessPageId (const std::string& data, BitmapFontPage* page);
	void ProcessPageTexture (const std::string& data, BitmapFontPage* page,
		const std::string& filename);
	
	void ProcessChar (std::ifstream& file, BitmapFontCharset* charset);
	void ProcessCharId (const std::string& data, BitmapFontChar* ch);
	void ProcessCharPositionX (const std::string& data, BitmapFontChar* ch);
	void ProcessCharPositionY (const std::string& data, BitmapFontChar* ch);
	void ProcessCharSizeWidth (const std::string& data, BitmapFontChar* ch);
	void ProcessCharSizeHeight (const std::string& data, BitmapFontChar* ch);
	void ProcessCharOffsetX (const std::string& data, BitmapFontChar* ch);
	void ProcessCharOffsetY (const std::string& data, BitmapFontChar* ch);
	void ProcessCharXAdvance (const std::string& data, BitmapFontChar* ch);
	void ProcessCharPage (const std::string& data, BitmapFontChar* ch);
	void ProcessCharChannel (const std::string& data, BitmapFontChar* ch);

	std::pair<std::string, std::string> ProcessAttribute (
		const std::string& data, std::size_t& pos);
};

#endif