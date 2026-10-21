/*
 * VTFLib++ KDE thumbnailer
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

#pragma once

#include <QImage>
#include <QSize>

class QByteArray;

namespace VtfThumbnail
{
	// Decodes a .vtf file held in memory into an image no larger than
	// targetSize (aspect ratio kept). Uses the smallest mipmap that still
	// covers the requested size, so large textures stay cheap.
	// Returns a null QImage on failure.
	QImage render(const QByteArray &data, const QSize &targetSize);
}
