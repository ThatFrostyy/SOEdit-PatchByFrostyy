#include "stdafx.h"
#include "stdio.h"
#include "parse_mtl.h"

Parse_Mtl::Parse_Mtl()
{
	TextBuffer = NULL;
	ZeroMemory((char*)&TDiffuse, MAX_PATH);
	ZeroMemory((char*)&TBump, MAX_PATH);
	ZeroMemory((char*)&TSpecular, MAX_PATH);	
	AT = new All_Trash();
	AlphaChannel = 0;
}
//---------------------------------------------------------------------------

Parse_Mtl::~Parse_Mtl()
{
	if(TextBuffer)
		{delete[] TextBuffer; TextBuffer = NULL;}
	delete AT;
}
//---------------------------------------------------------------------------

TextPoint Parse_Mtl::ParseMTL(char *TextBuffer)
{
	TextPoint TextPoint = {0, false};
	char token[MAX_PATH] = {0};
	bool layer_load = true;
	_Pos pos;
	TextPoint = AT -> skip_insignificantly(TextBuffer, 0); if(TextPoint.error) {return TextPoint;}
	if(TextBuffer[TextPoint.TP] == '{' && !TextPoint.error)
	{
		TextPoint.TP++;
		ZeroMemory(&pos, sizeof(pos));
		pos = AT -> FindWord(TextBuffer, TextPoint.TP);
		memset(&token, 0, MAX_PATH);
		AT -> Write(TextBuffer, TextPoint.TP, pos.end, token);
		TextPoint.TP = pos.end + 1;
		if(pos.end == '\0')
			{TextPoint.error = true; return TextPoint;}
		if(stricmp(token, "material"))
			{TextPoint.error = true; return TextPoint;}
		TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
		ZeroMemory(&pos, sizeof(pos));
		pos = AT -> FindWord(TextBuffer, TextPoint.TP);
		memset(&token, 0, MAX_PATH);
		AT -> Write(TextBuffer, TextPoint.TP, pos.end, token);
		TextPoint.TP = pos.end + 1;
		if(pos.end == '\0')
			{TextPoint.error = true; return TextPoint;}
		if(stricmp(token, "simple") && stricmp(token, "bump") && stricmp(token, "envmap") && stricmp(token, "multiply"))
			{TextPoint.error = true; return TextPoint;}
		layer_load = false;
		for(;;)
		{
			TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
			if(TextBuffer[TextPoint.TP] == '{')
			{
				TextPoint.TP++;
				TextPoint = ParseParameter(TextBuffer, TextPoint.TP);
				if(TextPoint.error)
					{return TextPoint;}
				layer_load = true;
			}
			else
			if(TextBuffer[TextPoint.TP] == '}')
			{
				if(!layer_load)
					{TextPoint.error = true; return TextPoint;}
				TextPoint.TP++;
				break;
			}
			else
				{TextPoint.error = true; return TextPoint;}
		}
	}
	return TextPoint;
}
//---------------------------------------------------------------------------

TextPoint Parse_Mtl::ParseParameter(char *TextBuffer, unsigned long sh)
{
	TextPoint TextPoint = {0, false};
	TextPoint.TP = sh;
	bool diffuse = false, bump = false, specular = false, lightmap = false, bumpVolume = false, diffuse1 = false;
	bool mask = false, simple = false, height = false, envmap = false;
	bool tile = false, glow = false, nolight = false, full_specular = false, emitsheat = false, translucency = false, alphatocoverage = false, no_outlines = false, FakeReflection = false;
	bool blend = false, color = false, gloss_scale = false, alpharef = false, specular_intensity = false, period = false, envamount = false, parallax_scale = false, amount = false;
	_Pos pos;
	char token[MAX_PATH] = {0};
	TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
	memset(&token, 0, MAX_PATH);
	ZeroMemory(&pos, sizeof(pos));
	pos = AT -> FindWord(TextBuffer, TextPoint.TP);
	AT -> Write(TextBuffer, TextPoint.TP, pos.end, token);
	TextPoint.TP = pos.end;
	TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
	if(!stricmp(token, "diffuse") || !stricmp(token, "bump") || !stricmp(token, "specular") || !stricmp(token, "lightmap") || !stricmp(token, "bumpVolume") \
	|| !stricmp(token, "mask") || !stricmp(token, "simple") || !stricmp(token, "height") || !stricmp(token, "envmap") || !stricmp(token, "diffuse1"))// 
	{
		if(TextBuffer[TextPoint.TP] != '"')
			{TextPoint.error = true; return TextPoint;}
		TextPoint.TP++;
		pos.start = TextPoint.TP;
		TextPoint = AT -> find_symbol(TextBuffer, '"', TextPoint.TP);
		if(TextPoint.error)
			{return TextPoint;}
		if(!stricmp(token, "diffuse"))
		{
			if(diffuse == true)
				{TextPoint.error = true; return TextPoint;}
			AT -> Write(TextBuffer, pos.start, TextPoint.TP, (char*)&TDiffuse);
			diffuse = true;
		}
		else
		if(!stricmp(token, "bump"))
		{
			if(bump == true)
				{TextPoint.error = true; return TextPoint;}
			AT -> Write(TextBuffer, pos.start, TextPoint.TP, (char*)&TBump);
			bump = true;
		}
		else
		if(!stricmp(token, "specular"))
		{
			if(specular == true)
				{TextPoint.error = true; return TextPoint;}
			AT -> Write(TextBuffer, pos.start, TextPoint.TP, (char*)&TSpecular);
			specular = true;
		}
		else
		if(!stricmp(token, "diffuse1"))
			{if(diffuse1 == true) {TextPoint.error = true; return TextPoint;} diffuse1 = true;}
		else
		if(!stricmp(token, "lightmap"))
			{if(lightmap == true) {TextPoint.error = true; return TextPoint;} lightmap = true;}
		else
		if(!stricmp(token, "bumpVolume"))
			{if(bumpVolume == true) {TextPoint.error = true; return TextPoint;} bumpVolume = true;}
		else
		if(!stricmp(token, "mask"))
			{if(mask == true) {TextPoint.error = true; return TextPoint;} mask = true;}
		else
		if(!stricmp(token, "simple"))
			{if(simple == true) {TextPoint.error = true; return TextPoint;} simple = true;}
		else
		if(!stricmp(token, "height"))
			{if(height == true) {TextPoint.error = true; return TextPoint;} height = true;}
		else
		if(!stricmp(token, "envmap"))
			{if(envmap == true) {TextPoint.error = true; return TextPoint;} envmap = true;}
		ZeroMemory(&pos, sizeof(pos));
		TextPoint.TP++;
		TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
		bool MipMap = false, Axis = false, AlphaChannel = false;
		for(;;)
		{
			if(TextBuffer[TextPoint.TP] == '{')
			{
				TextPoint.TP++;
				TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
				memset(&token, 0, MAX_PATH);
				ZeroMemory(&pos, sizeof(pos));
				pos = AT -> FindWord(TextBuffer, TextPoint.TP);
				AT -> Write(TextBuffer, TextPoint.TP, pos.end, token);
				TextPoint.TP = pos.end;
				TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
				if(TextBuffer[TextPoint.TP] < '0' || TextBuffer[TextPoint.TP] > '9')
					{TextPoint.error = true; return TextPoint;}
				if(!stricmp(token, "MipMap") || !stricmp(token, "AlphaChannel"))
				{
					if(!stricmp(token, "MipMap"))
					{
						if(MipMap)
							{TextPoint.error = true; return TextPoint;}
						MipMap = true;
					}
					else
					{
						if(AlphaChannel)
							{TextPoint.error = true; return TextPoint;}
						AlphaChannel = true;
					}
					ZeroMemory(&pos, sizeof(pos));
					pos = AT -> FindWord(TextBuffer, TextPoint.TP);
					TextPoint.TP = pos.end;
					TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
					if(TextBuffer[TextPoint.TP] != '}')
						{TextPoint.error = true; return TextPoint;}
					//else
					//	{break;}
				}
				else
				if(!stricmp(token, "Axis"))
				{
					if(Axis)
						{TextPoint.error = true; return TextPoint;}
					Axis = true;
					ZeroMemory(&pos, sizeof(pos));
					pos = AT -> FindWord(TextBuffer, TextPoint.TP);
					TextPoint.TP = pos.end;
					TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
					if(TextBuffer[TextPoint.TP] < '0' || TextBuffer[TextPoint.TP] > '9')
						{TextPoint.error = true; return TextPoint;}
					ZeroMemory(&pos, sizeof(pos));
					pos = AT -> FindWord(TextBuffer, TextPoint.TP);
					TextPoint.TP = pos.end;
					TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
					if(TextBuffer[TextPoint.TP] != '}')
						{TextPoint.error = true; return TextPoint;}
					//else
					//	{break;}
				}
				else
					{TextPoint.error = true; return TextPoint;}
			}
			else
			if(TextBuffer[TextPoint.TP] != '}')
				{TextPoint.error = true; return TextPoint;}
			else
				{break;}
			TextPoint.TP = pos.end + 1;
			TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
		}
		TextPoint.TP++;
	}
	else
	if(!stricmp(token, "tile") || !stricmp(token, "glow") || !stricmp(token, "nolight") \
	|| !stricmp(token, "full_specular") || !stricmp(token, "emitsheat") || !stricmp(token, "translucency") \
	|| !stricmp(token, "alphatocoverage") || !stricmp(token, "no_outlines") || !stricmp(token, "FakeReflection"))
	{
		if(!stricmp(token, "tile"))
			{if(tile == true) {TextPoint.error = true; return TextPoint;} tile = true;}
		else
		if(!stricmp(token, "glow"))
			{if(glow == true) {TextPoint.error = true; return TextPoint;} glow = true;}
		else
		if(!stricmp(token, "nolight"))
			{if(nolight == true) {TextPoint.error = true; return TextPoint;} nolight = true;}
		else
		if(!stricmp(token, "full_specular"))
			{if(full_specular == true) {TextPoint.error = true; return TextPoint;} full_specular = true;}
		else
		if(!stricmp(token, "emitsheat"))
			{if(emitsheat == true) {TextPoint.error = true; return TextPoint;} emitsheat = true;}
		else
		if(!stricmp(token, "translucency"))
			{if(translucency == true) {TextPoint.error = true; return TextPoint;} translucency = true;}
		else
		if(!stricmp(token, "alphatocoverage"))
			{if(alphatocoverage == true) {TextPoint.error = true; return TextPoint;} alphatocoverage = true;}
		else
		if(!stricmp(token, "no_outlines"))
			{if(no_outlines == true) {TextPoint.error = true; return TextPoint;} no_outlines = true;}
		else
		if(!stricmp(token, "FakeReflection"))
			{if(FakeReflection == true) {TextPoint.error = true; return TextPoint;} FakeReflection = true;}
		if(TextBuffer[TextPoint.TP] != '}')
			{TextPoint.error = true; return TextPoint;}
		TextPoint.TP++;
	}
	else
	if(!stricmp(token, "blend") || !stricmp(token, "color") || !stricmp(token, "gloss_scale") \
	|| !stricmp(token, "alpharef") || !stricmp(token, "specular_intensity") \
	|| !stricmp(token, "period") || !stricmp(token, "envamount") || !stricmp(token, "parallax_scale") || !stricmp(token, "amount"))
	{
		char c = token[0], q = 0;
		if(!stricmp(token, "parallax_scale"))
			{if(parallax_scale == true) {TextPoint.error = true; return TextPoint;} parallax_scale = true;}
		else
		if(!stricmp(token, "period"))
			{if(period == true) {TextPoint.error = true; return TextPoint;} period = true;}
		else
		if(!stricmp(token, "amount"))
			{if(amount == true) {TextPoint.error = true; return TextPoint;} amount = true;}
		else
		if(!stricmp(token, "alpharef"))
			{if(alpharef == true) {TextPoint.error = true; return TextPoint;} alpharef = true;}
		memset(&token, 0, MAX_PATH);
		ZeroMemory(&pos, sizeof(pos));
		q = TextBuffer[TextPoint.TP];
		if(q != '"')
		{
			pos = AT -> FindWord(TextBuffer, TextPoint.TP);
			AT -> Write(TextBuffer, TextPoint.TP, pos.end, token);
			TextPoint.TP = pos.end;
		}
		if(c == 'b')
		{
			if(blend == true) {TextPoint.error = true; return TextPoint;}
			if(!stricmp(token, "test"))
				{AlphaChannel = 1;}
			else
			if(!stricmp(token, "blend"))
				{AlphaChannel = 2;}
			else
			if(!stricmp(token, "add") || !stricmp(token, "sub"))
				{AlphaChannel = 3;}
			else
			if(!stricmp(token, "none"))
				{AlphaChannel = 0;}
			else
				{TextPoint.error = true; return TextPoint;}
			blend = true;
		}
		else
		if(c == 'c')
		{
			if(color == true) {TextPoint.error = true; return TextPoint;}
			if(q != '"')
			{
				if(token[0] < '0' || token[0] > '9')
					{TextPoint.error = true; return TextPoint;}
			}
			else
			{
				TextPoint.TP++;
				TextPoint = AT -> find_symbol(TextBuffer, '"', TextPoint.TP);
				if(TextPoint.error)
					{return TextPoint;}
				TextPoint.TP++;
			}
			color = true;
		}
		else
		{
			if(token[0] < '0' || token[0] > '9')
				{TextPoint.error = true; return TextPoint;}
			else
			if(c == 'e')
				{if(envamount == true) {TextPoint.error = true; return TextPoint;} envamount = true;}
			else
			if(c == 'g')
				{if(gloss_scale == true) {TextPoint.error = true; return TextPoint;} gloss_scale = true;}
		}
		TextPoint = AT -> skip_insignificantly(TextBuffer, TextPoint.TP); if(TextPoint.error) {return TextPoint;}
		if(TextBuffer[TextPoint.TP] != '}')
			{TextPoint.error = true; return TextPoint;}
		TextPoint.TP++;
	}



	/*
	{blend none}
	{blend blend}
	{blend test}
	{blend add}
	{blend sub}

	{color "229 229 229 25"}
	{color 0xE5161A19}
	{gloss_scale 1}
	{alpharef 155}
	{specular_intensity 2.5}
	{period 1}
	{envamount 1.0}
	{parallax_scale 1.5}
	{amount 0.3}

	{tile}
	{glow}
	{nolight}
	{full_specular}
	{emitsheat}
	{translucency}
	{alphatocoverage}
	{no_outlines}
	{FakeReflection}

	{diffuse "$/model/tetrarch"}
	{bump "$/model/tetrarch_bp"}
	{specular "$/model/tetrarch_sp"}
	{mask "$/inventory/weapon/holo_glass_sp"}
	{simple "$/landscape/water/water3"}
	{height "/texture/land/sand/erodedsand_hm"}
	*/
	return TextPoint;
}
//---------------------------------------------------------------------------














