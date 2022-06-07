#pragma once
// used: std::array
#include <array>
#include <d3d9.h>
#include <winnt.h>

enum
{
	COLOR_R = 0,
	COLOR_G = 1,
	COLOR_B = 2,
	COLOR_A = 3
};

struct ColorRGBExp32
{
	std::byte r, g, b;
	signed char exponent;
};

class Color
{
public:
	Color() = default;

	/* default color constructor (in: 0 - 255) */
	constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) :
		arrColor({ r, g, b, a }) { }

	constexpr Color(int r, int g, int b, int a = 255) :
		arrColor({ static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(a) }) { }
	
	explicit Color(float* rgb) : Color(rgb[0], rgb[1], rgb[2], 1.0f) {}

	D3DCOLOR ToD3D() {
		return D3DCOLOR_RGBA(COLOR_R, COLOR_G, COLOR_B, COLOR_A);
	}

	/* float color constructor (in: 0.0 - 1.0) */
	constexpr Color(float r, float g, float b, float a = 1.0f) :
		arrColor({ static_cast<std::uint8_t>(r * 255.f), static_cast<std::uint8_t>(g * 255.f), static_cast<std::uint8_t>(b * 255.f), static_cast<std::uint8_t>(a * 255.f) }) { }

	/* output color to given variables */
	void Get(std::uint8_t& r, std::uint8_t& g, std::uint8_t& b, std::uint8_t& a) const
	{
		r = arrColor.at(COLOR_R);
		g = arrColor.at(COLOR_G);
		b = arrColor.at(COLOR_B);
		a = arrColor.at(COLOR_A);
	}

	int r() const { return  arrColor.at(COLOR_R); }
	int g() const { return  arrColor.at(COLOR_G); }
	int b() const { return  arrColor.at(COLOR_B); }
	int a() const { return  arrColor.at(COLOR_A); }

	void SetColor(int _r, int _g, int _b, int _a)
	{
		arrColor.at(COLOR_R) = (unsigned char)_r;
		arrColor.at(COLOR_G) = (unsigned char)_g;
		arrColor.at(COLOR_B) = (unsigned char)_b;
		arrColor.at(COLOR_A) = (unsigned char)_a;
	}

	/* convert color to directx argb */
	[[nodiscard]] D3DCOLOR GetD3D() const
	{
		return D3DCOLOR_ARGB(arrColor.at(COLOR_A), arrColor.at(COLOR_R), arrColor.at(COLOR_G), arrColor.at(COLOR_B));
	}

	std::uint8_t& operator[](const std::size_t i)
	{
		return this->arrColor.data()[i];
	}

	const std::uint8_t& operator[](const std::size_t i) const
	{
		return this->arrColor.data()[i];
	}

	bool operator==(const Color& colSecond) const
	{
		return *const_cast<Color*>(this) == *const_cast<Color*>(&colSecond);
	}

	bool operator!=(const Color& colSecond) const
	{
		return !(operator==(colSecond));
	}

	Color& operator=(const Color& colFrom)
	{
		arrColor.at(0) = colFrom.arrColor.at(COLOR_R);
		arrColor.at(1) = colFrom.arrColor.at(COLOR_G);
		arrColor.at(2) = colFrom.arrColor.at(COLOR_B);
		arrColor.at(3) = colFrom.arrColor.at(COLOR_A);
		return *this;
	}

	/* returns certain R/G/B/A value */
	template <std::size_t N>
	[[nodiscard]] std::uint8_t Get() const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "given index is out of range");
		return arrColor[N];
	}

	/* returns copy of color with changed certain R/G/B/A value to given value */
	template <std::size_t N>
	[[nodiscard]] Color Set(const std::uint8_t nValue) const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "given index is out of range");

		Color colCopy = *this;
		colCopy.arrColor[N] = nValue;
		return colCopy;
	}

	/* returns copy of color with multiplied certain R/G/B/A value by given value */
	template <std::size_t N>
	[[nodiscard]] Color Multiplier(const float flValue) const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "given index is out of range");

		Color colCopy = *this;
		colCopy.arrColor[N] = static_cast<std::uint8_t>(static_cast<float>(colCopy.arrColor[N]) * flValue);
		return colCopy;
	}

	/* returns copy of color with divided certain R/G/B/A value by given value */
	template <std::size_t N>
	[[nodiscard]] Color Divider(const int iValue) const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "given index is out of range");

		Color colCopy = *this;
		colCopy.arrColor[N] /= iValue;
		return colCopy;
	}

	/* returns certain R/G/B/A float value (in: 0 - 255, out: 0.0 - 1.0) */
	template <std::size_t N>
	[[nodiscard]] float Base() const
	{
		static_assert(N >= COLOR_R && N <= COLOR_A, "given index is out of range");
		return arrColor[N] / 255.f;
	}

	/* convert color to float array (in: 0 - 255, out: 0.0 - 1.0) */
	[[nodiscard]] std::array<float, 3U> Base() const
	{
		std::array<float, 3U> arrBaseColor = { };
		arrBaseColor.at(COLOR_R) = this->Base<COLOR_R>();
		arrBaseColor.at(COLOR_G) = this->Base<COLOR_G>();
		arrBaseColor.at(COLOR_B) = this->Base<COLOR_B>();
		return arrBaseColor;
	}

	/* set color from float array (in: 0.0 - 1.0, out: 0 - 255) */
	static Color FromBase3(float arrBase[3])
	{
		return Color(arrBase[0], arrBase[1], arrBase[2]);
	}

	/* convert color to float array w/ alpha (in: 0 - 255, out: 0.0 - 1.0) */
	[[nodiscard]] std::array<float, 4U> BaseAlpha() const
	{
		std::array<float, 4U> arrBaseColor = { };
		arrBaseColor.at(COLOR_R) = this->Base<COLOR_R>();
		arrBaseColor.at(COLOR_G) = this->Base<COLOR_G>();
		arrBaseColor.at(COLOR_B) = this->Base<COLOR_B>();
		arrBaseColor.at(COLOR_A) = this->Base<COLOR_A>();
		return arrBaseColor;
	}

	/* set color from float array w/ alpha (in: 0.0 - 1.0, out: 0 - 255) */
	static Color FromBase4(float arrBase[4])
	{
		return Color(arrBase[0], arrBase[1], arrBase[2], arrBase[3]);
	}

	/* return RGB color converted from HSB/HSV color */
	static Color FromHSB(float flHue, float flSaturation, float flBrightness)
	{
		const float h = flHue == 1.0f ? 0 : flHue * 6.0f;
		const int i = static_cast<int>(h);
		const float f = h - static_cast<float>(i);
		const float p = flBrightness * (1.0f - flSaturation);
		const float q = flBrightness * (1.0f - flSaturation * f);
		const float t = flBrightness * (1.0f - flSaturation * (1.0f - f));

		float r = 0.0f, g = 0.0f, b = 0.0f;

		switch (i)
		{
		case 0:
			r = flBrightness, g = t, b = p;
			break;
		case 1:
			r = q, g = flBrightness, b = p;
			break;
		case 2:
			r = p, g = flBrightness, b = t;
			break;
		case 3:
			r = p, g = q, b = flBrightness;
			break;
		case 4:
			r = t, g = p, b = flBrightness;
			break;
		case 5:
		default:
			r = flBrightness, g = p, b = q;
			break;
		}

		return Color(r, g, b);
	}

private:
	std::array<std::uint8_t, 4U> arrColor;
};
