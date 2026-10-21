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

// KIO thumbnail plugin: lets Dolphin (and other KDE apps) show .vtf
// thumbnails and previews.

#include "VtfThumbnail.h"

#include <KIO/ThumbnailCreator>
#include <KPluginFactory>

#include <QFile>

class VtfThumbnailCreator : public KIO::ThumbnailCreator
{
	Q_OBJECT

public:
	VtfThumbnailCreator(QObject *pParent, const QVariantList &Args)
		: KIO::ThumbnailCreator(pParent, Args)
	{
	}

	KIO::ThumbnailResult create(const KIO::ThumbnailRequest &Request) override
	{
		// Remote files are downloaded by the thumbnail worker first,
		// so we always get a local path here.
		const QString sPath = Request.url().toLocalFile();
		if(sPath.isEmpty())
		{
			return KIO::ThumbnailResult::fail();
		}

		QFile File(sPath);
		if(!File.open(QIODevice::ReadOnly))
		{
			return KIO::ThumbnailResult::fail();
		}

		// Skip absurdly large files rather than stalling the worker.
		constexpr qint64 iMaxFileSize = 512ll * 1024 * 1024;
		if(File.size() > iMaxFileSize)
		{
			return KIO::ThumbnailResult::fail();
		}

		const QImage Image = VtfThumbnail::render(File.readAll(), Request.targetSize());
		if(Image.isNull())
		{
			return KIO::ThumbnailResult::fail();
		}

		return KIO::ThumbnailResult::pass(Image);
	}
};

K_PLUGIN_CLASS_WITH_JSON(VtfThumbnailCreator, "vtfthumbnail.json")

#include "vtfthumbnail.moc"
