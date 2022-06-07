#pragma once
#include "../datatypes/matrix.h"
#include <DirectXMath.h>
#include <algorithm>
#include <math.h>

#define M_HPI					DirectX::XM_PIDIV2	// half pi
#define M_QPI					DirectX::XM_PIDIV4	// quarter pi
#define M_PI					DirectX::XM_PI		// pi const
#define M_2PI					DirectX::XM_2PI		// pi double
#define M_GPI					1.6180339887498f	// golden ratio
#define M_RADPI					57.295779513082f	// pi in radians
#define RAD2DEG(x)				DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x)				DirectX::XMConvertToRadians(x)
#define D3DXToRadian(degree)	((degree) * (M_PI / 180.0f))

namespace math {
	inline void vector_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out[0] = in1.DotProduct(in2[0]) + in2[0][3];
		out[1] = in1.DotProduct(in2[1]) + in2[1][3];
		out[2] = in1.DotProduct(in2[2]) + in2[2][3];
	}
	inline void vector_angles(const Vector& forward, QAngle& angles)
	{
		float	tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0) {
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
			if (pitch < 0)
				pitch += 360;
		}
		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}
	inline void ClampAngles(QAngle& angles)
	{
		if (angles.x > 89.0f) angles.x = 89.0f;
		else if (angles.x < -89.0f) angles.x = -89.0f;

		if (angles.y > 180.0f) angles.y = 180.0f;
		else if (angles.y < -180.0f) angles.y = -180.0f;

		angles.z = 0;
	}
	template<class T>
	inline void Normalize3(T& vec)
	{
		for (auto i = 0; i < 2; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] > 180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}
	inline void fix_angles(QAngle& angles) {
		Normalize3(angles);
		ClampAngles(angles);
	}
	inline static bool screen_transform(const Vector& in, Vector& out)
	{
		static auto& w2sMatrix = i::Engine->WorldToScreenMatrix();

		out.x = w2sMatrix.flData[0][0] * in.x + w2sMatrix.flData[0][1] * in.y + w2sMatrix.flData[0][2] * in.z + w2sMatrix.flData[0][3];
		out.y = w2sMatrix.flData[1][0] * in.x + w2sMatrix.flData[1][1] * in.y + w2sMatrix.flData[1][2] * in.z + w2sMatrix.flData[1][3];
		out.z = 0.0f;

		float w = w2sMatrix.flData[3][0] * in.x + w2sMatrix.flData[3][1] * in.y + w2sMatrix.flData[3][2] * in.z + w2sMatrix.flData[3][3];

		if (w < 0.001f)
		{
			out.x *= 100000;
			out.y *= 100000;
			return false;
		}

		out.x /= w;
		out.y /= w;

		return true;
	}
	inline bool WorldToScreen(const Vector& in, Vector& out)
	{
		if (screen_transform(in, out))
		{
			int w, h;
			i::Engine->GetScreenSize(w, h);

			out.x = (w / 2.0f) + (out.x * w) / 2.0f;
			out.y = (h / 2.0f) - (out.y * h) / 2.0f;

			return true;
		}
		return false;
	}
	inline void angle2vectors(const QAngle& angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles.x));
		DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles.y));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}
	inline float GetRealDistanceFOV(const float& distance, const QAngle& current, const QAngle& aim)
	{
		QAngle delta = aim - current;
		fix_angles(delta);

		float hyp = sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));

		Vector v_aim, v_current;
		angle2vectors(current, v_current);
		angle2vectors(aim, v_aim);

		float delta_distance = v_current.DistTo(v_aim);

		float real_distance = sin(D3DXToRadian(delta_distance)) * distance;

		float multiplier;

		if (distance <= 100.f)
			multiplier = 2.f;

		if (distance > 100.f && distance <= 200.f)
			multiplier = 1.05f;

		if (distance > 200.f && distance <= 400.f)
			multiplier = 1.05f;

		if (distance > 400.f && distance <= 600.f)
			multiplier = 1.05f;

		if (distance > 600.f)
			multiplier = 1.15f;

		multiplier = std::clamp(multiplier, 0.f, 2.0f);

		float value = sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f)) * real_distance * multiplier;

		value = std::clamp(value, -180.0f, 180.0f);

		return value;
	}
	inline QAngle CalcAngle(const Vector& src, const Vector& dst)
	{
		QAngle vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		vAngle.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
		vAngle.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
		vAngle.z = 0.0f;

		if (delta.x >= 0.0)
			vAngle.y += 180.0f;

		return vAngle;
	}
}