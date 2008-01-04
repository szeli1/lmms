/*
 * mallets.cpp - tuned instruments that one would bang upon
 *
 * Copyright (c) 2006-2008 Danny McRae <khjklujn/at/users.sourceforge.net>
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


#include "mallets.h"

#include <QtCore/QDir>
#include <QtGui/QMessageBox>

#include "BandedWG.h"
#include "ModalBar.h"
#include "TubeBell.h"

#include "engine.h"
#include "gui_templates.h"

#undef SINGLE_SOURCE_COMPILE
#include "embed.cpp"


extern "C"
{

plugin::descriptor malletsstk_plugin_descriptor =
{
	STRINGIFY_PLUGIN_NAME( PLUGIN_NAME ),
	"Mallets",
	QT_TRANSLATE_NOOP( "pluginBrowser",
				"Tuneful things to bang on" ),
	"Danny McRae <khjklujn/at/users.sf.net>",
	0x0100,
	plugin::Instrument,
	new QPixmap( PLUGIN_NAME::getIconPixmap( "logo" ) ),
	NULL
} ;

}


malletsInstrument::malletsInstrument( instrumentTrack * _channel_track ):
	instrument( _channel_track, &malletsstk_plugin_descriptor ),
	m_hardnessModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_positionModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_vibratoGainModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_vibratoFreqModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_stickModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_modulatorModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_crossfadeModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_lfoSpeedModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_lfoDepthModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_adsrModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_pressureModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_motionModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_velocityModel(64.0f, 0.0f, 128.0f, 0.1f, this),
	m_strikeModel( FALSE, FALSE, TRUE, boolModel::defaultRelStep(), this ),
	m_presetsModel(this),
	m_spreadModel(0, 0, 255, 1, this)
{
	m_hardnessModel.setTrack( _channel_track );
	m_positionModel.setTrack( _channel_track );
	m_vibratoGainModel.setTrack( _channel_track );
	m_vibratoFreqModel.setTrack( _channel_track );
	m_stickModel.setTrack( _channel_track );
	m_modulatorModel.setTrack( _channel_track );
	m_crossfadeModel.setTrack( _channel_track );
	m_lfoSpeedModel.setTrack( _channel_track );
	m_lfoDepthModel.setTrack( _channel_track );
	m_adsrModel.setTrack( _channel_track );
	m_pressureModel.setTrack( _channel_track );
	m_motionModel.setTrack( _channel_track );
	m_velocityModel.setTrack( _channel_track );
	m_strikeModel.setTrack( _channel_track );
	m_spreadModel.setTrack( _channel_track );
	
	// ModalBar
	m_presetsModel.addItem( tr( "Marimba" ) );
	m_scalers.append( 4.0 );
	m_presetsModel.addItem( tr( "Vibraphone" ) );
	m_scalers.append( 4.0 );
	m_presetsModel.addItem( tr( "Agogo" ) );
	m_scalers.append( 5.0 );
	m_presetsModel.addItem( tr( "Wood1" ) );
	m_scalers.append( 4.0 );
	m_presetsModel.addItem( tr( "Reso" ) );
	m_scalers.append( 2.5 );
	m_presetsModel.addItem( tr( "Wood2" ) );
	m_scalers.append( 5.0 );
	m_presetsModel.addItem( tr( "Beats" ) );
	m_scalers.append( 20.0 );
	m_presetsModel.addItem( tr( "Two Fixed" ) );
	m_scalers.append( 5.0 );
	m_presetsModel.addItem( tr( "Clump" ) );
	m_scalers.append( 4.0 );
	
	// TubeBell
	m_presetsModel.addItem( tr( "Tubular Bells" ) );
	m_scalers.append( 1.8 );
	
	// BandedWG
	m_presetsModel.addItem( tr( "Uniform Bar" ) );
	m_scalers.append( 25.0 );
	m_presetsModel.addItem( tr( "Tuned Bar" ) );
	m_scalers.append( 10.0 );
	m_presetsModel.addItem( tr( "Glass" ) );
	m_scalers.append( 16.0 );
	m_presetsModel.addItem( tr( "Tibetan Bowl" ) );
	m_scalers.append( 7.0 );

	m_buffer = new sampleFrame[engine::getMixer()->framesPerPeriod()];
}




malletsInstrument::~malletsInstrument()
{
	delete[] m_buffer;
}




void malletsInstrument::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	// ModalBar
	m_hardnessModel.saveSettings( _doc, _this, "hardness" );
	m_positionModel.saveSettings( _doc, _this, "position" );
	m_vibratoGainModel.saveSettings( _doc, _this, "vib_gain" );
	m_vibratoFreqModel.saveSettings( _doc, _this, "vib_freq" );
	m_stickModel.saveSettings( _doc, _this, "stick_mix" );

	// TubeBell
	m_modulatorModel.saveSettings( _doc, _this, "modulator" );
	m_crossfadeModel.saveSettings( _doc, _this, "crossfade" );
	m_lfoSpeedModel.saveSettings( _doc, _this, "lfo_speed" );
	m_lfoDepthModel.saveSettings( _doc, _this, "lfo_depth" );
	m_adsrModel.saveSettings( _doc, _this, "adsr" );
	
	// BandedWG
	m_pressureModel.saveSettings( _doc, _this, "pressure" );
	m_motionModel.saveSettings( _doc, _this, "motion" );
	m_vibratoModel.saveSettings( _doc, _this, "vibrato" );
	m_velocityModel.saveSettings( _doc, _this, "velocity" );
	m_strikeModel.saveSettings( _doc, _this, "strike" );

	m_presetsModel.saveSettings( _doc, _this, "preset" );
	m_spreadModel.saveSettings( _doc, _this, "spread" );
}




void malletsInstrument::loadSettings( const QDomElement & _this )
{
	// ModalBar
	m_hardnessModel.loadSettings( _this, "hardness" );
	m_positionModel.loadSettings( _this, "position" );
	m_vibratoGainModel.loadSettings( _this, "vib_gain" );
	m_vibratoFreqModel.loadSettings( _this, "vib_freq" );
	m_stickModel.loadSettings( _this, "stick_mix" );

	// TubeBell
	m_modulatorModel.loadSettings( _this, "modulator" );
	m_crossfadeModel.loadSettings( _this, "crossfade" );
	m_lfoSpeedModel.loadSettings( _this, "lfo_speed" );
	m_lfoDepthModel.loadSettings( _this, "lfo_depth" );
	m_adsrModel.loadSettings( _this, "adsr" );
	
	// BandedWG
	m_pressureModel.loadSettings( _this, "pressure" );
	m_motionModel.loadSettings( _this, "motion" );
	m_vibratoModel.loadSettings( _this, "vibrato" );
	m_velocityModel.loadSettings( _this, "velocity" );
	m_strikeModel.loadSettings( _this, "strike" );

	m_presetsModel.loadSettings( _this, "preset" );
	m_spreadModel.loadSettings( _this, "spread" );
}




QString malletsInstrument::nodeName( void ) const
{
	return( malletsstk_plugin_descriptor.name );
}




void malletsInstrument::playNote( notePlayHandle * _n, bool )
{
	if( m_filesMissing )
	{
		return;
	}

	int p = m_presetsModel.value();
	
	const float freq = _n->frequency();
	if ( _n->totalFramesPlayed() == 0 )
	{
		float vel = static_cast<float>( _n->getVolume() ) / 100.0f;
		
		if( p < 9 )
		{
			_n->m_pluginData = new malletsSynth( freq,
								vel,
								m_vibratoGainModel.value(),
								m_hardnessModel.value(),
								m_positionModel.value(),
								m_stickModel.value(),
								m_vibratoFreqModel.value(),
								p,
								(Uint8) m_spreadModel.value(),
								engine::getMixer()->sampleRate() );
		}
		else if( p == 9 )
		{
			_n->m_pluginData = new malletsSynth( freq,
								vel,
								p,
								m_lfoDepthModel.value(),
								m_modulatorModel.value(),
								m_crossfadeModel.value(),
								m_lfoSpeedModel.value(),
								m_adsrModel.value(),
								(Uint8) m_spreadModel.value(),
								engine::getMixer()->sampleRate() );
		}
		else
		{
			_n->m_pluginData = new malletsSynth( freq,
								vel,
								m_pressureModel.value(),
								m_motionModel.value(),
								m_vibratoModel.value(),
								p - 10,
								m_strikeModel.value() * 128.0,
								m_velocityModel.value(),
								(Uint8) m_spreadModel.value(),
								engine::getMixer()->sampleRate() );
		}
	}

	const fpp_t frames = _n->framesLeftForCurrentPeriod();

	malletsSynth * ps = static_cast<malletsSynth *>( _n->m_pluginData );
	sample_t add_scale = 0.0f;
	if( p == 10 )
	{
		add_scale = static_cast<sample_t>( m_strikeModel.value() ) * freq * 2.5f;
	}
	for( fpp_t frame = 0; frame < frames; ++frame )
	{
		const sample_t left = ps->nextSampleLeft() * 
					( m_scalers[m_presetsModel.value()] + add_scale );
		const sample_t right = ps->nextSampleRight() * 
					( m_scalers[m_presetsModel.value()] + add_scale );
		for( Uint8 chnl = 0; chnl < DEFAULT_CHANNELS / 2; ++chnl )
		{
			m_buffer[frame][chnl * DEFAULT_CHANNELS / 2] = left;
			m_buffer[frame][( chnl + 1 ) * DEFAULT_CHANNELS / 2] = right;
		}
	}
	
	getInstrumentTrack()->processAudioBuffer( m_buffer, frames, _n );
}




void malletsInstrument::deleteNotePluginData( notePlayHandle * _n )
{
	delete static_cast<malletsSynth *>( _n->m_pluginData );
}




instrumentView * malletsInstrument::createView( QWidget * _parent )
{
	return( new malletsInstrumentView( this, _parent ) );
}




malletsInstrumentView::malletsInstrumentView( malletsInstrument * _instrument, QWidget * _parent ) :
	instrumentView( _instrument, _parent )
{
	_instrument->m_filesMissing =
		!QDir( configManager::inst()->stkDir() ).exists() ||
		!QFileInfo( configManager::inst()->stkDir() + QDir::separator()
			+ "sinewave.raw" ).exists();

	// for some reason this crashes...???
	if( _instrument->m_filesMissing )
	{
		QMessageBox::information( 0, tr( "Missing files" ),
				tr( "Your Stk-installation seems to be "
					"incomplete. Please make sure "
					"the full Stk-package is installed!" ),
				QMessageBox::Ok );
	}

	m_modalBarWidget = setupModalBarControls( this );
	setWidgetBackground( m_modalBarWidget, "artwork" );
	
	m_tubeBellWidget = setupTubeBellControls( this );
	setWidgetBackground( m_tubeBellWidget, "artwork" );
	m_tubeBellWidget->hide();
	
	m_bandedWGWidget = setupBandedWGControls( this );
	setWidgetBackground( m_bandedWGWidget, "artwork" );
	m_bandedWGWidget->hide();
	
	m_presetsCombo = new comboBox( this, tr( "Instrument" ) );
	m_presetsCombo->setGeometry( 64, 157, 99, 22 );
	m_presetsCombo->setFont( pointSize<8>( m_presetsCombo->font() ) );
	
	connect( &_instrument->m_presetsModel, SIGNAL( dataChanged() ),
		 this, SLOT( changePreset() ) );
	
	m_spreadKnob = new knob( knobBright_26, this, tr( "Spread" ) );
	m_spreadKnob->setLabel( tr( "Spread" ) );
	m_spreadKnob->move( 178, 173 );
	m_spreadKnob->setHintText( tr( "Spread:" ) + " ", "" );
}




malletsInstrumentView::~malletsInstrumentView()
{
}



void malletsInstrumentView::setWidgetBackground( QWidget * _widget, const QString & _pic )
{
	_widget->setAutoFillBackground( TRUE );
	QPalette pal;
	pal.setBrush( _widget->backgroundRole(),
		PLUGIN_NAME::getIconPixmap( _pic.toAscii().constData() ) );
	_widget->setPalette( pal );
}




QWidget * malletsInstrumentView::setupModalBarControls( QWidget * _parent )
{
	QWidget * widget = new QWidget( _parent );
	widget->setFixedSize( 250, 250 );
		
	m_hardnessKnob = new knob( knobBright_26, widget, tr( "Hardness" ) );
	m_hardnessKnob->setLabel( tr( "Hardness" ) );
	m_hardnessKnob->move( 145, 24 );
	m_hardnessKnob->setHintText( tr( "Hardness:" ) + " ", "" );

	m_positionKnob = new knob( knobBright_26, widget, tr( "Position" ) );
	m_positionKnob->setLabel( tr( "Position" ) );
	m_positionKnob->move( 195, 24 );
	m_positionKnob->setHintText( tr( "Position:" ) + " ", "" );

	m_vibratoGainKnob = new knob( knobBright_26, widget, tr( "Vibrato Gain" ) );
	m_vibratoGainKnob->setLabel( tr( "Vib Gain" ) );
	m_vibratoGainKnob->move( 56, 86 );
	m_vibratoGainKnob->setHintText( tr( "Vib Gain:" ) + " ", "" );

	m_vibratoFreqKnob = new knob( knobBright_26, widget, tr( "Vibrato Freq" ) );
	m_vibratoFreqKnob->setLabel( tr( "Vib Freq" ) );
	m_vibratoFreqKnob->move( 117, 86 );
	m_vibratoFreqKnob->setHintText( tr( "Vib Freq:" ) + " ", "" );

	m_stickKnob = new knob( knobBright_26, widget, tr( "Stick Mix" ) );
	m_stickKnob->setLabel( tr( "Stick Mix" ) );
	m_stickKnob->move( 178, 86 );
	m_stickKnob->setHintText( tr( "Stick Mix:" ) + " ", "" );
	
	return( widget );
}




QWidget * malletsInstrumentView::setupTubeBellControls( QWidget * _parent )
{
	QWidget * widget = new QWidget( _parent );
	widget->setFixedSize( 250, 250 );
	
	m_modulatorKnob = new knob( knobBright_26, widget, tr( "Modulator" ) );
	m_modulatorKnob->setLabel( tr( "Modulator" ) );
	m_modulatorKnob->move( 145, 24 );
	m_modulatorKnob->setHintText( tr( "Modulator:" ) + " ", "" );

	m_crossfadeKnob = new knob( knobBright_26, widget, tr( "Crossfade" ) );
	m_crossfadeKnob->setLabel( tr( "Crossfade" ) );
	m_crossfadeKnob->move( 195, 24 );
	m_crossfadeKnob->setHintText( tr( "Crossfade:" ) + " ", "" );
	
	m_lfoSpeedKnob = new knob( knobBright_26, widget, tr( "LFO Speed" ) );
	m_lfoSpeedKnob->setLabel( tr( "LFO Speed" ) );
	m_lfoSpeedKnob->move( 56, 86 );
	m_lfoSpeedKnob->setHintText( tr( "LFO Speed:" ) + " ", "" );
	
	m_lfoDepthKnob = new knob( knobBright_26, widget, tr( "LFO Depth" ) );
	m_lfoDepthKnob->setLabel( tr( "LFO Depth" ) );
	m_lfoDepthKnob->move( 117, 86 );
	m_lfoDepthKnob->setHintText( tr( "LFO Depth:" ) + " ", "" );
	
	m_adsrKnob = new knob( knobBright_26, widget, tr( "ADSR" ) );
	m_adsrKnob->setLabel( tr( "ADSR" ) );
	m_adsrKnob->move( 178, 86 );
	m_adsrKnob->setHintText( tr( "ADSR:" ) + " ", "" );

	return( widget );
}




QWidget * malletsInstrumentView::setupBandedWGControls( QWidget * _parent )
{
	// BandedWG
	QWidget * widget = new QWidget( _parent );
	widget->setFixedSize( 250, 250 );
	
	m_strikeLED = new ledCheckBox( tr( "Bowed" ), widget, tr( "Bowed" ) );
	m_strikeLED->move( 165, 30 );

	m_pressureKnob = new knob( knobBright_26, widget, tr( "Pressure" ) );
	m_pressureKnob->setLabel( tr( "Pressure" ) );
	m_pressureKnob->move( 56, 86 );
	m_pressureKnob->setHintText( tr( "Pressure:" ) + " ", "" );

	m_motionKnob = new knob( knobBright_26, widget, tr( "Motion" ) );
	m_motionKnob->setLabel( tr( "Motion" ) );
	m_motionKnob->move( 117, 86 );
	m_motionKnob->setHintText( tr( "Motion:" ) + " ", "" );
	
	m_velocityKnob = new knob( knobBright_26, widget, tr( "Speed" ) );
	m_velocityKnob->setLabel( tr( "Speed" ) );
	m_velocityKnob->move( 178, 86 );
	m_velocityKnob->setHintText( tr( "Speed:" ) + " ", "" );
	
	m_vibratoKnob = new knob( knobBright_26, widget, tr( "Vibrato" ) );
	m_vibratoKnob->setLabel( tr( "Vibrato" ) );
	m_vibratoKnob->move( 178, 129 );
	m_vibratoKnob->setHintText( tr( "Vibrato:" ) + " ", "" );
	
	return( widget );
}




void malletsInstrumentView::modelChanged( void )
{
	malletsInstrument * inst = castModel<malletsInstrument>();
	m_hardnessKnob->setModel( &inst->m_hardnessModel );
	m_positionKnob->setModel( &inst->m_positionModel );
	m_vibratoGainKnob->setModel( &inst->m_vibratoGainModel );
	m_vibratoFreqKnob->setModel( &inst->m_vibratoFreqModel );
	m_stickKnob->setModel( &inst->m_stickModel );
	m_modulatorKnob->setModel( &inst->m_modulatorModel );
	m_crossfadeKnob->setModel( &inst->m_crossfadeModel );
	m_lfoSpeedKnob->setModel( &inst->m_lfoSpeedModel );
	m_lfoDepthKnob->setModel( &inst->m_lfoDepthModel );
	m_adsrKnob->setModel( &inst->m_adsrModel );
	m_pressureKnob->setModel( &inst->m_pressureModel );
	m_motionKnob->setModel( &inst->m_motionModel );
	m_vibratoKnob->setModel( &inst->m_vibratoModel );
	m_velocityKnob->setModel( &inst->m_velocityModel );
	m_strikeLED->setModel( &inst->m_strikeModel );
	m_presetsCombo->setModel( &inst->m_presetsModel );
	m_spreadKnob->setModel( &inst->m_spreadModel );
}




void malletsInstrumentView::changePreset()
{
	malletsInstrument * inst = castModel<malletsInstrument>();
	int _preset = inst->m_presetsModel.value();
	
	printf("malletsInstrumentView %d\n", _preset);
	if( _preset < 9 )
	{
		m_tubeBellWidget->hide();
		m_bandedWGWidget->hide();
		m_modalBarWidget->show();
	}
	else if( _preset == 9 )
	{
		m_modalBarWidget->hide();
		m_bandedWGWidget->hide();
		m_tubeBellWidget->show();
	}
	else
	{
		m_modalBarWidget->hide();
		m_tubeBellWidget->hide();
		m_bandedWGWidget->show();
	}		
}



// ModalBar
malletsSynth::malletsSynth( const StkFloat _pitch,
				const StkFloat _velocity,
				const StkFloat _control1,
				const StkFloat _control2,
				const StkFloat _control4,
				const StkFloat _control8,
				const StkFloat _control11,
				const int _control16,
				const Uint8 _delay,
				const sample_rate_t _sample_rate )
{
	try
	{
		Stk::setSampleRate( _sample_rate );
		Stk::setRawwavePath( configManager::inst()->stkDir()
						.toAscii().constData() );
	
		m_voice = new ModalBar();
	
		m_voice->controlChange( 1, _control1 );
		m_voice->controlChange( 2, _control2 );
		m_voice->controlChange( 4, _control4 );
		m_voice->controlChange( 8, _control8 );
		m_voice->controlChange( 11, _control11 );
		m_voice->controlChange( 16, _control16 );
		m_voice->controlChange( 128, 128.0f );
		
		m_voice->noteOn( _pitch, _velocity );
	}
	catch( ... )
	{
		m_voice = NULL;
	}
	
	m_delay = new StkFloat[256];
	m_delayRead = 0;
	m_delayWrite = _delay;
	for( Uint16 i = 0; i < 256; i++ )
	{
		m_delay[i] = 0.0;
	}
}




// TubeBell
malletsSynth::malletsSynth( const StkFloat _pitch,
				const StkFloat _velocity,
				const int _preset,
				const StkFloat _control1,
				const StkFloat _control2,
				const StkFloat _control4,
				const StkFloat _control11,
				const StkFloat _control128,
				const Uint8 _delay,
				const sample_rate_t _sample_rate )
{
	try
	{
		Stk::setSampleRate( _sample_rate );
		Stk::setRawwavePath( configManager::inst()->stkDir()
						.toAscii().constData() );
	
		m_voice = new TubeBell();
	
		m_voice->controlChange( 1, _control1 );
		m_voice->controlChange( 2, _control2 );
		m_voice->controlChange( 4, _control4 );
		m_voice->controlChange( 11, _control11 );
		m_voice->controlChange( 128, _control128 );
	
		m_voice->noteOn( _pitch, _velocity );
	}
	catch( ... )
	{
		m_voice = NULL;
	}
	
	m_delay = new StkFloat[256];
	m_delayRead = 0;
	m_delayWrite = _delay;
	for( Uint16 i = 0; i < 256; i++ )
	{
		m_delay[i] = 0.0;
	}
}




// BandedWG
malletsSynth::malletsSynth( const StkFloat _pitch,
				const StkFloat _velocity,
				const StkFloat _control2,
				const StkFloat _control4,
				const StkFloat _control11,
				const int _control16,
				const StkFloat _control64,
				const StkFloat _control128,
				const Uint8 _delay,
				const sample_rate_t _sample_rate )
{
	try
	{
		Stk::setSampleRate( _sample_rate );
		Stk::setRawwavePath( configManager::inst()->stkDir()
						.toAscii().constData() );

		m_voice = new BandedWG();
	
		m_voice->controlChange( 1, 128.0 );
		m_voice->controlChange( 2, _control2 );
		m_voice->controlChange( 4, _control4 );
		m_voice->controlChange( 11, _control11 );
		m_voice->controlChange( 16, _control16 );
		m_voice->controlChange( 64, _control64 );
		m_voice->controlChange( 128, _control128 );
	
		m_voice->noteOn( _pitch, _velocity );
	}
	catch( ... )
	{
		m_voice = NULL;
	}
	
	m_delay = new StkFloat[256];
	m_delayRead = 0;
	m_delayWrite = _delay;
	for( Uint16 i = 0; i < 256; i++ )
	{
		m_delay[i] = 0.0;
	}
}




extern "C"
{

// neccessary for getting instance out of shared lib
plugin * lmms_plugin_main( void * _data )
{
	return( new malletsInstrument( static_cast<instrumentTrack *>( _data ) ) );
}


}


#include "mallets.moc"


