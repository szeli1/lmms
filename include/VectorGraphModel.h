/*
 * VecorGraphModel.h - Vector graph model implementation
 *
 * Copyright (c) 2024 - 2025 szeli1 </at/gmail/dot/com> TODO
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

#ifndef LMMS_GUI_VECTORGRAPHMODEL_H
#define LMMS_GUI_VECTORGRAPHMODEL_H

#include <array>
#include <cinttypes>
#include <vector>

#include <QCursor>
#include <QMenu>
#include <QMutex>
#include <QPainterPath>
#include <QWidget>

#include "AutomatableModel.h"
#include "JournallingObject.h"
#include "lmms_basics.h"
#include "Model.h"
#include "ModelView.h"
#include "SubWindow.h"

//#include "VectorGraphView.h"

namespace lmms
{

class FloatModel;
class VectorGraphDataArray;
//class lmms::gui::VectorGraphView;

using PointF = std::pair<float, float>;

class LMMS_EXPORT VectorGraphModel : public Model, public JournallingObject
{
Q_OBJECT
public:
	VectorGraphModel(size_t arrayMaxLength, Model* parent, bool defaultConstructed);
	~VectorGraphModel();

	inline size_t getDataArraySize()
	{
		return m_dataArrays.size();
	}
	inline VectorGraphDataArray* getDataArray(size_t arrayLocation)
	{
		return &m_dataArrays[arrayLocation];
	}
	inline size_t getMaxLength()
	{
		return m_maxLength;
	}
	inline void setMaxLength(size_t arrayMaxLength)
	{
		if (m_maxLength != arrayMaxLength)
		{
			m_maxLength = arrayMaxLength;
			emit dataChanged();
			emit updateGraphView(false);
		}
	}
	// returns added VectorGraphDataArray location
	size_t addDataArray();
	// deletes VectorGraphDataArray at arrayLocation
	// preservs the order
	void deleteDataArray(size_t arrayLocation);
	inline void clearDataArray()
	{
		m_dataArrays.clear();
		emit dataChanged();
		emit updateGraphView(false);
	}
	// if the id is not found then it will return 0
	int getDataArrayLocationFromId(int arrayId);
	int getDataArrayNewId();

	// save, load
	QString nodeName() const override
	{
		return "VectorGraphModel";
	}
	virtual void saveSettings(QDomDocument& doc, QDomElement& element, const QString& name);
	virtual void loadSettings(const QDomElement& element, const QString& name);
	virtual void saveSettings(QDomDocument& doc, QDomElement& element);
	virtual void loadSettings(const QDomElement& element);
	void lockGetSamplesAccess();
	void unlockGetSamplesAccess();
	void lockBakedSamplesAccess();
	void unlockBakedSamplesAccess();
	
	// addJournalCheckpoint
	void modelAddJournalCheckPoint();
signals:
	// point changed inside VectorGraphDataArray m_dataArray or m_maxLength changed
	void dataChanged();
	void updateGraphView(bool shouldUseGetLastSamples);
	// signals when a dataArray gets to 0 element size
	// arrayLocation is the location of the VectorGraphDataArray
	// arrayLocation can be -1
	void clearedEvent(int arrayLocation);
	// style changed inside m_dataArray
	void styleChanged();
public slots:
	void dataArrayChanged();
	void updateGraphModel(bool shouldUseGetLastSamples);
	void dataArrayClearedEvent(int arrayId);
	void dataArrayStyleChanged();
private:

	std::vector<VectorGraphDataArray> m_dataArrays;
	size_t m_maxLength;

	// block threads that want to access
	// a dataArray's getSamples() at the same time
	QMutex m_getSamplesAccess;
	QMutex m_bakedSamplesAccess;
	//friend class lmms::gui::VectorGraphView;
};

class LMMS_EXPORT VectorGraphDataArray
{

public:
	// avoid using this or run updateConnections() after initialization
	VectorGraphDataArray();
	VectorGraphDataArray(
	bool isFixedSize, bool isFixedX, bool isFixedY, bool isNonNegative,
	bool isFixedEndPoints, bool isSelectable, bool isEditableAttrib, bool isAutomatable,
	bool isEffectable, bool isSaveable, VectorGraphModel* parent, int arrayId);
	~VectorGraphDataArray();

	void updateConnections(VectorGraphModel* parent);

	// see descriptions in privete
	void setIsFixedSize(bool bValue);
	void setIsFixedX(bool bValue);
	void setIsFixedY(bool bValue);
	void setIsFixedEndPoints(bool bValue);
	void setIsSelectable(bool bValue);
	void setIsEditableAttrib(bool bValue);
	void setIsAutomatable(bool bValue);
	void setIsEffectable(bool bValue);
	void setIsSaveable(bool bValue);
	void setIsNonNegative(bool bValue);
	void setLineColor(QColor color);
	void setActiveColor(QColor color);
	void setFillColor(QColor color);
	void setAutomatedColor(QColor color);

	// sets a dataArray as an effector to this dataArray
	// returns true if successful
	// if callDataChanged then it will call dataChanged() --> paintEvent()
	bool setEffectorArrayLocation(int arrayLocation, bool callDataChanged);
	void setIsAnEffector(bool bValue);

	bool getIsFixedSize();
	bool getIsFixedX();
	bool getIsFixedY();
	bool getIsFixedEndPoints();
	bool getIsSelectable();
	bool getIsEditableAttrib();
	bool getIsAutomatable();
	bool getIsEffectable();
	bool getIsSaveable();
	bool getIsNonNegative();
	QColor* getLineColor();
	QColor* getActiveColor();
	QColor* getFillColor();
	QColor* getAutomatedColor();

	// returns -1 if it has no effector
	int getEffectorArrayLocation();
	bool getIsAnEffector();
	int getId();


	// array: -------------------
	// checks m_isFixedSize (== false) and m_maxLength
	// returns the location of added point, -1 if not found or can not be added
	// returns the location of found point if there is a point already at newX
	int add(float newX);
	// checks m_isFixedSize (== false)
	// deletes the point in pointLocation location
	void deletePoint(size_t pointLocation);
	// clears m_dataArray without any checks
	inline void clear()
	{
		m_dataArray.clear();
		m_needsUpdating.clear();
		// m_automationModelArray should not be cleared without FloatModel destruction
		clearedEvent();
		getUpdatingFromPoint(-1);
		dataChanged();
	}
	inline size_t size()
	{
		return m_dataArray.size();
	}
	// clamps down the values to 0 - 1, -1 - 1
	// sorts array, removes duplicated x positions, calls dataChanged() if callDataChanged
	// clamp: should clamp, sort: should sort
	void formatArray(std::vector<PointF>* dataArrayOut, bool shouldClamp, bool shouldRescale, bool shouldSort, bool callDataChanged);


	// get attribute: -------------------
	inline float getX(size_t pointLocation)
	{
		return m_dataArray[pointLocation].m_x;
	}
	inline float getY(size_t pointLocation)
	{
		return m_dataArray[pointLocation].m_y;
	}
	inline float getC(size_t pointLocation)
	{
		return m_dataArray[pointLocation].m_c;
	}
	inline float getValA(size_t pointLocation)
	{
		return m_dataArray[pointLocation].m_valA;
	}
	inline float getValB(size_t pointLocation)
	{
		return m_dataArray[pointLocation].m_valB;
	}
	inline unsigned int getType(size_t pointLocation)
	{
		return m_dataArray[pointLocation].m_type;
	}
	// returns attribLocation: 0 = m_y, 1 = m_c, 2 = m_valA, 3 = m_valB (int VectorGraphPoint)
	uint8_t getAutomatedAttribLocation(size_t pointLocation);
	uint8_t getEffectedAttribLocation(size_t pointLocation);
	// returns true when m_effectPoints is true or
	// when getEffectedAttribLocation() > 0 (y is uneffected)
	bool getEffectPoints(size_t pointLocation);
	// returns true when m_effectLines is true and
	// when getEffectedAttribLocation() == 0 (y is effected)
	bool getEffectLines(size_t pointLocation);
	// returns the effect type of the selected id or slot
	// effectSlot: which effect slot (m_effectTypeA / B / C)
	unsigned int getEffect(size_t pointLocation, size_t effectSlot);
	// true when the automationModel's value changed since last check
	bool getIsAutomationValueChanged(size_t pointLocation);
	// can return nullptr
	FloatModel* getAutomationModel(size_t pointLocation);


	// get: -------------------
	// returns -1 when position is not found
	int getLocation(float searchX);
	// gets the nearest data location to the position,
	// foundOut is true when the nearest position = searchXIn,
	// reurns -1 when search failed
	int getNearestLocation(float searchXIn, bool* foundOut, bool* isBeforeOut);


	// returns the latest updated graph values
	// targetSizeIn is the retuned vector's size
	void getSamples(size_t targetSizeIn, std::vector<float>* sampleBufferOut);
	// returns m_bakedSamples without updating
	void getLastSamples(std::vector<float>* sampleBufferOut);
	std::vector<int> getEffectorArrayLocations();


	// set: -------------------
	// sets / adds m_dataArray points
	// .first = x, .second = y coords
	// isCurved -> should set curve automatically
	// clear -> clear m_dataArray before setting
	// clamp -> clamp input positions
	// rescale -> scale input positions
	// sort -> sort input positions
	// callDataChanged -> call dataChanged() after -> paintEvent()
	// PointF = std::pair<float, float>
	// ()the std::vector<PointF>* inputDataArray modifies the array)
	void setDataArray(std::vector<PointF>* inputDataArray, bool shouldCurve, bool shouldClear, bool shouldClamp, bool shouldRescale, bool shouldSort, bool callDataChanged);
	void setDataArray(std::vector<float>* inputDataArray, bool shouldCurve, bool shouldClear, bool shouldClamp, bool shouldRescale, bool callDataChanged);
	void setDataArray(float* inputDataArray, size_t size, bool shouldCurve, bool shouldClear, bool shouldClamp, bool shouldRescale, bool callDataChanged);


	// set attribute: -------------------
	// checks m_isFixedX (== false)
	// sets x position, returns final location
	// returns the location of found point if there is a point already at newX
	size_t setX(size_t pointLocation, float newX);
	// checks m_isFixedY (== false)
	// sets y position
	void setY(size_t pointLocation, float newY);
	// checks m_isEditableAttrib
	// sets curve
	void setC(size_t pointLocation, float newC);
	// checks m_isEditableAttrib
	// sets 1. attribute value
	void setValA(size_t pointLocation, float fValue);
	// checks m_isEditableAttrib
	// sets 2. attribute value
	void setValB(size_t pointLocation, float fValue);
	// checks m_isEditableAttrib
	// sets line type
	void setType(size_t pointLocation, unsigned int newType);
	// checks m_isAutomatable and m_isEditableAttrib
	// sets what attribute gets automated (by point's FloatModel)
	void setAutomatedAttrib(size_t pointLocation, uint8_t attribLocation);
	// checks m_isEffectable and m_isEditableAttrib
	// sets what attribute gets effected (by effector array)
	void setEffectedAttrib(size_t pointLocation, uint8_t attribLocation);
	// checks m_isEffectable and m_isEditableAttrib
	// if bValue is true then the effector array will effect the point's attributes
	void setEffectPoints(size_t pointLocation, bool bValue);
	// checks m_isEffectable and m_isEditableAttribs
	// if bValue is true then the effector array will effect the line's individual samples (only when y is effected)
	void setEffectLines(size_t pointLocation, bool bValue);
	// checks m_isEffectable and m_isEditableAttrib
	// sets the point's effect type
	// effectSlot: which effect slot (m_effectTypeA / B / C), effectType: what kind of effect (add, exct)
	void setEffect(size_t pointLocation, size_t effectSlot, unsigned int effectType);
	// checks m_isAutomatable
	// if bValue is true then make a new FloatModel and connect it, else delete
	// the currently used FloatModel
	void setAutomated(size_t pointLocation, bool bValue);


// signals: // not qt
	void dataChanged();
	// runs when m_dataArray.size() gets to 0
	void clearedEvent();
	// color
	void styleChanged();
private:
	// returns m_automationModelArray
	std::vector<FloatModel*>* getAutomationModelArray();
	// delete automationModels in m_automationModelArray
	// that are not used by points (there should be 0 cases like this)
	void deleteUnusedAutomation();
	// encodes m_dataArray to QString
	QString getSavedDataArray();
	// decodes and sets m_dataArray from QString
	void loadDataArray(QString data, size_t arraySize, bool callDataChanged);

	class VectorGraphPoint
	{
	public:
		inline VectorGraphPoint()
		{
		}
		inline VectorGraphPoint(float x, float y)
		{
			m_x = x;
			m_y = y;
		}
		// 0 - 1
		float m_x = 0.0f;
		// -1 - 1, getAutomatedAttrib() -> 0
		float m_y = 0.0f;
		// curve, -1 - 1, getAutomatedAttrib() -> 1
		float m_c = 0.0f;
		// valueA, -1 - 1, getAutomatedAttrib() -> 2
		float m_valA = 0.0f;
		// valueB, -1 - 1, getAutomatedAttrib() -> 3
		float m_valB = 0.0f;
		// line type:
		// 0 - none
		// 1 - bezier
		// 2 - sine
		// 3 - sineB
		// 4 - peak
		// 5 - steps
		// 6 - random
		unsigned int m_type = 0;
		// the automated attrib location and
		// the effected attrib location is
		// stored here
		// use getAutomatedAttrib or getEffectedAttrib to get it
		//unsigned int m_automatedEffectedAttribLocations = 0;
		uint8_t m_automatedAttribute = 0;
		uint8_t m_effectedAttribute = 0;

		// what effect will be applyed if effected
		// effects:
		// 0 - none
		// 1 - add
		// 2 - subtract
		// 3 - multiply
		// 4 - divide
		// 5 - power
		// 6 - log
		// 7 - sine
		// 8 - lower clamp
		// 9 - upper clamp
		unsigned int m_effectTypeA = 0;
		unsigned int m_effectTypeB = 0;
		unsigned int m_effectTypeC = 0;

		// if the point attributes should be effected,
		// getEffectPoints() will return true when
		// effected attrib location > 0
		bool m_effectPoints = false;
		// if the line (each sample) should be effected (only works when y is effected)
		bool m_effectLines = true;

		// stores m_automationModel->value(), used in getSamples() when updating
		float m_bufferedAutomationValue = 0.0f;
		// automation: connecting to floatmodels, -1 when it isn't conntected
		int m_automationModel = -1;
	};
	// deletes the point's automation model
	// if modelLocation == point location
	void deleteAutomationModel(int modelLocation, bool callDataChanged);
	// swapping values, "shouldShiftBetween" moves the values (between) once left or right to keep the order
	// handle m_isFixedEndPoints when using this
	void swap(size_t pointLocationA, size_t pointLocationB, bool shouldShiftBetween);

	// returns effected attribute value from base attribValue (input attribute value), does clamp
	// this function applies the point Effects (like add effect) based on attribValue and effectValue
	float processEffect(size_t pointLocation, float attribValue, uint8_t attribLocation, float effectValue);
	float processSingleEffect(size_t pointLocation, size_t effectSlot, float attribValue, float effectValue);
	// returns automated attribute value from base attribValue (input attribute value), does clamp
	float processAutomation(size_t pointLocation, float attribValue, uint8_t attribLocation);

	// line types, m_type is used for this
	// fadeInStartVal: from what relative x value should the line type fade out
	// samplesOut: output for the line type functions
	// xArray: exact sorted x coordinates of samples between 0 and 1
	// startLoc: from where to apply line type
	// endLoc: where to stop applying line type
	// other inputs: mostly between -1 and 1
	void processBezier(std::vector<float>* samplesOut, std::vector<float>* xArray, size_t startLoc, size_t endLoc,
		float yBefore, float yAfter, float targetYBefore, float targetYAfter, float curveStrength);
	void processLineTypeArraySine(std::vector<float>* samplesOut, std::vector<float>* xArray, size_t startLoc, size_t endLoc,
		float sineAmp, float sineFreq, float fadeInStartVal);
	void processLineTypeArraySineB(std::vector<float>* samplesOut, std::vector<float>* xArray, size_t startLoc, size_t endLoc,
		float sineAmp, float sineFreq, float sinePhase, float fadeInStartVal);
	void processLineTypeArrayPeak(std::vector<float>* samplesOut, std::vector<float>* xArray, size_t startLoc, size_t endLoc,
		float peakAmp, float peakX, float peakWidth, float fadeInStartVal);
	void processLineTypeArraySteps(std::vector<float>* samplesOut, std::vector<float>* xArray, size_t startLoc, size_t endLoc,
		std::vector<float>* yArray, float stepCount, float stepCurve, float fadeInStartVal);
	void processLineTypeArrayRandom(std::vector<float>* samplesOut, std::vector<float>* xArray, size_t startLoc, size_t endLoc,
		float randomAmp, float randomCount, float randomSeed, float fadeInStartVal);

	// updating
	// adds the m_dataArray points that are
	// effected by the changed effector array points.
	// ONLY WORKS IN SORTED ARRAYS
	void getUpdatingFromEffector(std::vector<size_t>* effectorUpdatedPoints);
	// if pointLocation >= 0 -> adds the location to m_needsUpdating
	// else it will update the whole m_dataArray and m_bakedSamples
	// changes in the size of m_dataArray (addtition, deletion, ect.)
	// should to cause a full update
	void getUpdatingFromPoint(int pointLocation);
	// adds the points that are changed because their
	// automation is changed
	void getUpdatingFromAutomation();
	// recalculates and sorts m_needsUpdating so
	// every point is in there only once
	void getUpdatingOriginals();

	// real getSamples processing
	void getSamplesInner(size_t targetSizeIn, bool* isChangedOut,
		std::vector<size_t>* updatingValuesOut, std::vector<float>* sampleBufferOut);
	// redraw lines
	void getSamplesUpdateLines(VectorGraphDataArray* effector, std::vector<float>* effectorSamples,
		std::vector<float>* sampleXLocations, size_t pointLocation, float stepSize);
	bool isEffectedPoint(size_t pointLocation);

	// checks m_isFixedEndPoints, does not call dataChanged()
	void formatDataArrayEndPoints();

	// can new data be added or removed
	bool m_isFixedSize;
	// can the positions be changed
	bool m_isFixedX;
	// can the values be changed
	bool m_isFixedY;
	// if true then it makes the last point coordinate 1, 1, the first point coordinate -1, 0
	bool m_isFixedEndPoints;
	// can VectorGraphView select this
	bool m_isSelectable;
	// can the point attributes be edited
	// every attribute outside of x and y
	// automation can be changed
	bool m_isEditableAttrib;
	// can the points be automated
	bool m_isAutomatable;
	// can the points be effected
	bool m_isEffectable;
	// if VectorGraphDataArray is allowed to save this
	bool m_isSaveable;
	// can values be less than 0
	bool m_isNonNegative;

	QColor m_lineColor;
	QColor m_activeColor;
	QColor m_fillColor;
	QColor m_automatedColor;

	VectorGraphModel* m_parent;
	// simple id system for setEffectorArrayLocation
	int m_id;

	// which VectorGraphDataArray can effect this one, -1 if not effected
	int m_effectorLocation;
	// is this VectorGraphDataArray effects others?
	bool m_isAnEffector;

	// ordered array of VectorGraphPoints
	std::vector<VectorGraphPoint> m_dataArray;
	

	// baking

	/*
	* getSamples() will return m_bakedSamples if lines are unchanged
	* else it will recalculate the changed line's values, update m_bakedSamples
	* getSamples() needs to know where did lines change so it updates
	* m_needsUpdating by running getUpdatingFromEffector()
	* if m_isDataChanged is true, then getSamples recalculates all the lines/samples
	* getSamples() clears m_needsUpdating after it has run
	* updating a line means recalculating m_bakedSamples in getSamples()
	* based on the changed points (stored in m_needsUpdating)
	* changes in a point will causes its line to update (line started by the point)
	* changes in position needs to cause the line before to update too
	* addition or deletion needs to cause all the lines to update
	*/

	// if we want to update all (the full line in getSamples())
	bool m_isDataChanged;
	// array containing output final float values for optimalization
	std::vector<float> m_bakedSamples;
	// used for updating m_bakedSamples fast
	std::vector<float> m_updatingBakedSamples;
	// unsorted array of locations in m_dataArray
	// that need to be updated
	// sorted in getUpdatingOriginals() because some functions need this to be sorted
	std::vector<size_t> m_needsUpdating;

	// this stores all the FloatModels, unsorted, should only contain currently used FloatModels
	// used for automation
	std::vector<FloatModel*> m_automationModelArray;

	// used in lineType calculations to store
	// large amount of floats without reallocation
	std::vector<float> m_universalSampleBuffer;

	// used for saving
	friend class lmms::VectorGraphModel;
};

} // namespace lmms

#endif // LMMS_GUI_VECTORGRAPHMODEL_H
