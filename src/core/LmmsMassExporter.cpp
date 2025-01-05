/*
 * LmmsMassExporter.cpp - exports files in .flac format on an other thread
 *
 * Copyright (c) 2024 - 2025 szeli1
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

#include <QFileInfo>

#include "LmmsMassExporter.h"

#include "FlacExporter.h"
#include "SampleBuffer.h"

namespace lmms
{

LmmsMassExporter::LmmsMassExporter() :
	m_abortExport(false),
	m_isThreadRunning(false),
	m_readMutex(),
	m_thread(nullptr)
{}

LmmsMassExporter::~LmmsMassExporter()
{
	stopExporting();
}


void LmmsMassExporter::startExporting(const QString& outputLocationAndName, std::shared_ptr<const SampleBuffer> buffer, callbackFn callbackFunction, void* callbackObject)
{
	QString filteredName(QFileInfo(outputLocationAndName).absolutePath() + "/" + QFileInfo(outputLocationAndName).baseName()+ ".flac");
	m_readMutex.lock();
	m_buffers.push_back(std::make_tuple(filteredName, buffer, callbackFunction, callbackObject));
	m_readMutex.unlock();

	if (m_isThreadRunning == false)
	{
		stopExporting();
		m_isThreadRunning = true;
		m_thread = new std::thread(&LmmsMassExporter::threadedExportFunction, this, &m_abortExport);
	}
}

void LmmsMassExporter::stopExporting()
{
	if (m_thread != nullptr)
	{
		if (m_isThreadRunning == true)
		{
			m_abortExport = true;
		}
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
		m_isThreadRunning = false;
		m_abortExport = false;
	}
}


void LmmsMassExporter::threadedExportFunction(LmmsMassExporter* thisExporter, volatile std::atomic<bool>* abortExport)
{
	thisExporter->m_isThreadRunning = true;

	while (*abortExport == false)
	{
		std::tuple<QString, std::shared_ptr<const SampleBuffer>, callbackFn, void*> curBuffer = std::make_tuple(QString(""), nullptr, nullptr, nullptr);
		thisExporter->m_readMutex.lock();
		bool shouldExit = thisExporter->m_buffers.size() <= 0;
		if (shouldExit == false)
		{
			curBuffer = thisExporter->m_buffers[thisExporter->m_buffers.size() - 1];
			thisExporter->m_buffers.pop_back();
		}
		thisExporter->m_readMutex.unlock();
		if (shouldExit) { break; }

		// important new scope
		{
			FlacExporter exporter(std::get<1>(curBuffer)->sampleRate(), 24, std::get<0>(curBuffer));
			if (exporter.getIsSuccesful())
			{
				exporter.writeThisBuffer(std::get<1>(curBuffer)->data(), std::get<1>(curBuffer)->size());
			}
		}

		if (std::get<2>(curBuffer))
		{
			// calling callback funcion
			std::get<2>(curBuffer)(std::get<3>(curBuffer));
		}
	}

	thisExporter->m_isThreadRunning = false;
}

} // namespace lmms
