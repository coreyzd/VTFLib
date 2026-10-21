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

#include "VtfThumbnail.h"

#include "../lib/VTFLib.h"

#include <QByteArray>
#include <QMutex>
#include <QMutexLocker>

#include <algorithm>
#include <limits>
#include <vector>

namespace
{
	// VTFLib options (like HDR exposure) are global, so serialise access.
	QMutex g_Mutex;

	// Matches VTFEdit++'s default exposure slider position (2000 / 100).
	constexpr vlSingle HdrExposure = 20.0f;

	bool isHdrFormat(VTFImageFormat Format)
	{
		return Format == IMAGE_FORMAT_RGBA16161616F
			|| Format == IMAGE_FORMAT_BC6H
			|| Format == IMAGE_FORMAT_BC6H_UNSIGNED
			|| VTFLib::CVTFFile::IsFloatFormat(Format) != vlFalse;
	}
}

namespace VtfThumbnail
{
	QImage render(const QByteArray &data, const QSize &targetSize)
	{
		if(data.isEmpty() || static_cast<quint64>(data.size()) > std::numeric_limits<vlUInt>::max())
		{
			return QImage();
		}

		QMutexLocker Lock(&g_Mutex);

		if(vlGetVersion() != VL_VERSION)
		{
			return QImage();
		}

		VTFLib::CVTFFile File;
		if(!File.Load(data.constData(), static_cast<vlUInt>(data.size()), vlFalse))
		{
			return QImage();
		}

		const vlUInt uiWidth = File.GetWidth();
		const vlUInt uiHeight = File.GetHeight();
		const vlUInt uiMipmaps = std::max<vlUInt>(File.GetMipmapCount(), 1);
		const VTFImageFormat Format = File.GetDecodeFormat();

		if(uiWidth == 0 || uiHeight == 0)
		{
			return QImage();
		}

		// Pick the smallest mipmap whose larger side still covers the request.
		const vlUInt uiWanted = static_cast<vlUInt>(std::max(1, std::max(targetSize.width(), targetSize.height())));
		vlUInt uiMipmap = 0;
		vlUInt uiMipWidth = uiWidth, uiMipHeight = uiHeight;
		for(vlUInt i = 1; i < uiMipmaps; i++)
		{
			vlUInt w = 0, h = 0, d = 0;
			VTFLib::CVTFFile::ComputeMipmapDimensions(uiWidth, uiHeight, 1, i, w, h, d);
			if(std::max(w, h) < uiWanted)
			{
				break;
			}
			uiMipmap = i;
			uiMipWidth = w;
			uiMipHeight = h;
		}

		// Animated: start frame. Cubemaps: first face. Volumes: first slice.
		vlUInt uiFrame = File.GetStartFrame();
		if(uiFrame >= std::max<vlUInt>(File.GetFrameCount(), 1))
		{
			uiFrame = 0;
		}

		vlByte *lpSource = File.GetData(uiFrame, 0, 0, uiMipmap);
		if(lpSource == nullptr)
		{
			return QImage();
		}

		QImage Result(static_cast<int>(uiMipWidth), static_cast<int>(uiMipHeight), QImage::Format_RGBA8888);
		if(Result.isNull())
		{
			return QImage();
		}

		// RGBA8888 rows are always width * 4 bytes (already 4-byte aligned),
		// so QImage's buffer is contiguous.
		if(isHdrFormat(Format))
		{
			// Same path as VTFEdit++: decode to float, then tone map at
			// the default exposure.
			std::vector<vlSingle> Float(static_cast<size_t>(uiMipWidth) * uiMipHeight * 4);

			vlSetFloat(VTFLIB_FP16_HDR_EXPOSURE, HdrExposure);

			if(!VTFLib::CVTFFile::Convert(lpSource, reinterpret_cast<vlByte *>(Float.data()),
					uiMipWidth, uiMipHeight, Format, IMAGE_FORMAT_RGBA32323232F) ||
			   !VTFLib::CVTFFile::Convert(reinterpret_cast<vlByte *>(Float.data()), Result.bits(),
					uiMipWidth, uiMipHeight, IMAGE_FORMAT_RGBA32323232F, IMAGE_FORMAT_RGBA8888))
			{
				return QImage();
			}
		}
		else if(!VTFLib::CVTFFile::ConvertToRGBA8888(lpSource, Result.bits(), uiMipWidth, uiMipHeight, Format))
		{
			return QImage();
		}

		if(targetSize.isValid() &&
		   (Result.width() > targetSize.width() || Result.height() > targetSize.height()))
		{
			Result = Result.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}

		return Result;
	}
}
