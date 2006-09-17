/*
 * ladspa_base.h - basic declarations concerning LADSPA
 *
 * Copyright (c) 2006 Danny McRae <khjklujn/at/users.sourceforge.net>
 * Copyright (c) 2006 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
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

#ifndef _LADSPA_BASE_H
#define _LADSPA_BASE_H

#include "ladspa_manager.h"

#ifdef LADSPA_SUPPORT

#include "plugin.h"

class ladspaControl;


typedef enum bufferRates
{
	CHANNEL_IN,
	CHANNEL_OUT,
	AUDIO_RATE_INPUT,
	AUDIO_RATE_OUTPUT,
	CONTROL_RATE_INPUT,
	CONTROL_RATE_OUTPUT
} buffer_rate_t;

typedef enum bufferData
{
	TOGGLED,
	INTEGER,
	FLOAT,
	TIME,
	NONE
} buffer_data_t;

typedef struct portDescription
{
	QString name;
	ch_cnt_t proc;
	Uint16 port_id;
	Uint16 control_id;
	buffer_rate_t rate;
	buffer_data_t data_type;
	float scale;
	LADSPA_Data max;
	LADSPA_Data min;
	LADSPA_Data def;
	LADSPA_Data value;
	LADSPA_Data * buffer;
	ladspaControl * control;
} port_desc_t;


inline ladspa_key_t subPluginKeyToLadspaKey(
		const plugin::descriptor::subPluginFeatures::key & _key )
{
	return( ladspa_key_t( _key.user.toStringList().first(),
				_key.user.toStringList().last() ) );
}

inline plugin::descriptor::subPluginFeatures::key ladspaKeyToSubPluginKey(
						plugin::descriptor * _desc,
						const QString & _name,
						const ladspa_key_t & _key )
{
	return( plugin::descriptor::subPluginFeatures::key( _desc, _name,
		QVariant( QStringList() << _key.first << _key.second ) ) );
}


#endif

#endif
