/*
 * ProjectRenderer.h - ProjectRenderer class for easily rendering projects
 *
 * Copyright (c) 2008-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef LMMS_PROJECT_RENDERER_H
#define LMMS_PROJECT_RENDERER_H

#include <functional>
#include <vector>

#include "AudioFileDevice.h"
#include "lmmsconfig.h"
#include "AudioEngine.h"
#include "OutputSettings.h"
#include "SampleFrame.h"

#include "lmms_export.h"

namespace lmms
{


class LMMS_EXPORT ProjectRenderer : public QThread
{
	Q_OBJECT
public:
	using BufferFn = std::function<void(std::vector<SampleFrame>*, void*)>;
	using EndFn = std::function<void(void*)>;
	enum class ExportFileFormat : int
	{
		Wave,
		Flac,
		Ogg,
		MP3,
		Count
	} ;
	constexpr static auto NumFileFormats = static_cast<std::size_t>(ExportFileFormat::Count);

	struct FileEncodeDevice
	{
		bool isAvailable() const { return m_getDevInst != nullptr; }

		ExportFileFormat m_fileFormat;
		const char * m_description;
		const char * m_extension;
		AudioFileDeviceInstantiaton m_getDevInst;
	} ;


	ProjectRenderer( const AudioEngine::qualitySettings & _qs,
				const OutputSettings & _os,
				ExportFileFormat _file_format,
				const QString & _out_file,
				const fpp_t defaultFrameCount,
				BufferFn getBufferFunction,
				EndFn endFunction,
				void* getBufferData);
	~ProjectRenderer() override = default;

	bool isReady() const
	{
		return m_fileDev != nullptr;
	}

	static ExportFileFormat getFileFormatFromExtension(
							const QString & _ext );

	static QString getFileExtensionFromFormat( ExportFileFormat fmt );

	static const std::array<FileEncodeDevice, 5> fileEncodeDevices;
	
	// returns false if finished
	bool processNextBuffer();
	bool processThisBuffer(SampleFrame* frameBuffer, const fpp_t frameCount);

public slots:
	void startProcessing();
	void abortProcessing();

signals:
	void progressChanged();


private:
	void run() override;

	AudioFileDevice * m_fileDev;
	AudioEngine::qualitySettings m_qualitySettings;

	volatile bool m_abort;

	// called if not nullptr
	// while run()
	// if returned buffer.size() <= 0 then break; end
	BufferFn m_getBufferFunction;
	// called at the end of run(), can be nullptr
	EndFn m_endFunction;
	void* m_getBufferData;
	std::vector<SampleFrame> m_buffer;
} ;


} // namespace lmms

#endif // LMMS_PROJECT_RENDERER_H
