/*
* Copyright (c) 2011 Erin Catto http://box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <Box2D/Common/b2Timer.h>

float64 b2Timer::s_invFrequency = 0.0f;

#if defined(WIN32)

#include <Windows.h>

b2Timer::b2Timer()
{
	LARGE_INTEGER largeInteger;

	if (s_invFrequency == 0.0f)
	{
		QueryPerformanceFrequency(&largeInteger);
		s_invFrequency = float64(largeInteger.QuadPart);
		if (s_invFrequency > 0.0f)
		{
			s_invFrequency = 1000.0f / s_invFrequency;
		}
	}

	QueryPerformanceCounter(&largeInteger);
	m_start = float64(largeInteger.QuadPart);
}

void b2Timer::Reset()
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	m_start = float64(largeInteger.QuadPart);
}

float32 b2Timer::GetMilliseconds() const
{
	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	float64 count = float64(largeInteger.QuadPart);
	float32 ms = float32(s_invFrequency * (count - m_start));
	return ms;
}

#else

b2Timer::b2Timer()
{
}

void b2Timer::Reset()
{
}

float32 b2Timer::GetMilliseconds() const
{
	return 0.0f;
}

#endif
