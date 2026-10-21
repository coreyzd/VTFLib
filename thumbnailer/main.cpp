/*
 * VTFLib++ freedesktop thumbnailer
 * Copyright (C) 2026 VTFEdit++ contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

// Command-line thumbnailer for the freedesktop.org thumbnailer spec, used
// by Nautilus, Nemo, Caja, Thunar (via tumbler), PCManFM and others.
//
//   vtf-thumbnailer [-s SIZE] INPUT.vtf OUTPUT.png
//
// GNOME runs this inside a bubblewrap sandbox with no display, so it must
// not create a QGuiApplication; QImage works fine without one.

#include "VtfThumbnail.h"

#include <QFile>
#include <QImage>
#include <QString>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace
{
	constexpr int DefaultSize = 256;
	constexpr qint64 MaxFileSize = 512ll * 1024 * 1024;

	int usage(const char *pProgram)
	{
		std::fprintf(stderr, "Usage: %s [-s SIZE] INPUT.vtf OUTPUT.png\n", pProgram);
		return 2;
	}
}

int main(int argc, char *argv[])
{
	int iSize = DefaultSize;
	const char *pInput = nullptr;
	const char *pOutput = nullptr;

	for(int i = 1; i < argc; i++)
	{
		if(std::strcmp(argv[i], "-s") == 0 || std::strcmp(argv[i], "--size") == 0)
		{
			if(++i >= argc)
			{
				return usage(argv[0]);
			}
			char *pEnd = nullptr;
			const long lSize = std::strtol(argv[i], &pEnd, 10);
			if(pEnd == argv[i] || *pEnd != '\0' || lSize <= 0 || lSize > 4096)
			{
				std::fprintf(stderr, "vtf-thumbnailer: invalid size '%s'\n", argv[i]);
				return 2;
			}
			iSize = static_cast<int>(lSize);
		}
		else if(std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0)
		{
			usage(argv[0]);
			return 0;
		}
		else if(pInput == nullptr)
		{
			pInput = argv[i];
		}
		else if(pOutput == nullptr)
		{
			pOutput = argv[i];
		}
		else
		{
			return usage(argv[0]);
		}
	}

	if(pInput == nullptr || pOutput == nullptr)
	{
		return usage(argv[0]);
	}

	QFile File(QString::fromLocal8Bit(pInput));
	if(!File.open(QIODevice::ReadOnly))
	{
		std::fprintf(stderr, "vtf-thumbnailer: cannot open '%s'\n", pInput);
		return 1;
	}
	if(File.size() > MaxFileSize)
	{
		std::fprintf(stderr, "vtf-thumbnailer: '%s' is too large\n", pInput);
		return 1;
	}

	const QImage Image = VtfThumbnail::render(File.readAll(), QSize(iSize, iSize));
	if(Image.isNull())
	{
		std::fprintf(stderr, "vtf-thumbnailer: cannot decode '%s'\n", pInput);
		return 1;
	}

	// Name the format explicitly: the output path may not end in ".png".
	if(!Image.save(QString::fromLocal8Bit(pOutput), "PNG"))
	{
		std::fprintf(stderr, "vtf-thumbnailer: cannot write '%s'\n", pOutput);
		return 1;
	}

	return 0;
}
