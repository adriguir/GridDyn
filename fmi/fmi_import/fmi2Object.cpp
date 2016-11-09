/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil;  eval: (c-set-offset 'innamespace 0); -*- */
/*
* LLNS Copyright Start
* Copyright (c) 2016, Lawrence Livermore National Security
* This work was performed under the auspices of the U.S. Department
* of Energy by Lawrence Livermore National Laboratory in part under
* Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
* Produced at the Lawrence Livermore National Laboratory.
* All rights reserved.
* For details, see the LICENSE file.
* LLNS Copyright End
*/

#include "fmiObjects.h"

fmi2Object::fmi2Object(fmi2Component cmp, std::shared_ptr<const fmiInfo> keyInfo, std::shared_ptr<const fmiCommonFunctions> comFunc):info(keyInfo),comp(cmp),commonFunctions(comFunc)
{
	
}

fmi2Object::~fmi2Object()
{
	commonFunctions->fmi2FreeInstance(comp);
}
void fmi2Object::setupExperiment(fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)
{
	auto ret=commonFunctions->fmi2SetupExperiment(comp,toleranceDefined,tolerance,startTime,stopTimeDefined,stopTime);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

fmuMode fmi2Object::getCurrentMode() const
{
	return currentMode;
}

void fmi2Object::setMode(fmuMode mode)
{
	if (mode == fmuMode::error)
	{
		currentMode = fmuMode::error;
		throw(fmiErrorException());
	}
	fmi2Status ret = fmi2Error;
	if (mode==fmuMode::terminated)
	{
		currentMode = fmuMode::terminated;
		ret=commonFunctions->fmi2Terminate(comp);
	}

	
	
	switch(currentMode)
	{
	case fmuMode::instantiatedMode:
		if (mode==fmuMode::initializationMode)
		{
			ret=commonFunctions->fmi2EnterInitializationMode(comp);
			
		}
		break;
	case fmuMode::initializationMode:
		if (mode == fmuMode::eventMode)
		{
			ret = commonFunctions->fmi2ExitInitializationMode(comp);

		}
		break;
	default:
		break;
	}

	if (ret == fmi2OK)
	{
		currentMode = mode;
	}
	else if (currentMode == mode)
	{
		ret = fmi2OK;
	}
	handleNonOKReturnValues(ret);
}

void fmi2Object::reset()
{
	currentMode = fmuMode::instantiatedMode;
	auto ret = commonFunctions->fmi2Reset(comp);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

template<>
std::string fmi2Object::get<std::string>(const std::string &param) const
{
	auto ref = info->getVariableInfo(param);
	if (ref.type != fmi_type_t::string)
	{
		handleNonOKReturnValues(fmi2Status::fmi2Discard);
		return ""; //if we get here just return an empty string otherwise we threw an exception
	}
	fmi2String res;
	auto retval = commonFunctions->fmi2GetString(comp, &(ref.valueRef), 1, &res);
	if (retval != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(retval);
	}
	return std::string(res); //this should copy the actual the string
}

void fmi2Object::get(const fmiVariableSet &vrset, fmi2Real value[]) const
{
	auto ret = commonFunctions->fmi2GetReal(comp, vrset.getValueRef(), vrset.getVRcount(),value);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}
void fmi2Object::get(const fmiVariableSet &vrset, fmi2Integer value[]) const
{
	auto ret = commonFunctions->fmi2GetInteger(comp, vrset.getValueRef(), vrset.getVRcount(), value);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

void fmi2Object::get(const fmiVariableSet &vrset, fmi2String value[]) const
{
	auto ret = commonFunctions->fmi2GetString(comp, vrset.getValueRef(), vrset.getVRcount(), value);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

void fmi2Object::set(const fmiVariableSet &vrset, fmi2Integer value[])
{
	auto ret = commonFunctions->fmi2GetInteger(comp, vrset.getValueRef(), vrset.getVRcount(), value);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}


void fmi2Object::set(const fmiVariableSet &vrset, fmi2Real value[])
{
	
	auto retval = commonFunctions->fmi2SetReal(comp, vrset.getValueRef(), vrset.getVRcount(), value);
	if (retval != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(retval);
	}
}

void fmi2Object::set(const std::string &param, const char *val)
{
	auto ref = info->getVariableInfo(param);
	if (ref.type != fmi_type_t::string)
	{
		handleNonOKReturnValues(fmi2Status::fmi2Discard);
		return;
	}
	auto ret = commonFunctions->fmi2SetString(comp, &(ref.valueRef), 1, &val);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

void fmi2Object::set(const std::string &param, const std::string &val)
{
	auto ref = info->getVariableInfo(param);
	if (ref.type != fmi_type_t::string)
	{
		handleNonOKReturnValues(fmi2Status::fmi2Discard);
		return;
	}
	fmi2String val2 = val.c_str();
	auto ret = commonFunctions->fmi2SetString(comp, &(ref.valueRef), 1, &val2);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

void fmi2Object::getFMUState(fmi2FMUstate *FMUstate)
{
	auto ret = commonFunctions->fmi2GetFMUstate(comp, FMUstate);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}
void fmi2Object::setFMUState(fmi2FMUstate FMUstate)
{
	auto ret = commonFunctions->fmi2SetFMUstate(comp, FMUstate);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

size_t fmi2Object::serializedStateSize(fmi2FMUstate FMUstate)
{
	size_t sz;
	auto ret = commonFunctions->fmi2SerializedFMUstateSize(comp, FMUstate, &sz);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
	return sz;
}
void fmi2Object::serializeState(fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size)
{
	auto ret = commonFunctions->fmi2SerializeFMUstate(comp, FMUstate, serializedState, size);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}
void fmi2Object::deSerializeState(const fmi2Byte serializedState[], size_t size, fmi2FMUstate *FMUstate)
{
	auto ret = commonFunctions->fmi2DeSerializeFMUstate(comp, serializedState, size, FMUstate);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}
void fmi2Object::getDirectionalDerivative(const fmi2ValueReference vUnknown_ref[], size_t nUnknown, const fmi2ValueReference vKnown_ref[], size_t nKnown, const fmi2Real dvKnown[], fmi2Real dvUnknown[])
{
	auto ret=commonFunctions->fmi2GetDirectionalDerivative(comp, vUnknown_ref, nUnknown, vKnown_ref, nKnown, dvKnown, dvUnknown);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

fmi2Real fmi2Object::getPartialDerivative(int index_x, int index_y, double dx)
{
	double dy;
	commonFunctions->fmi2GetDirectionalDerivative(comp, &(info->getVariableInfo(index_x).valueRef), 1, &(info->getVariableInfo(index_y).valueRef), 1, &dx, &dy);
	return dy;
}

void fmi2Object::setOutputVariables(const std::vector<std::string> outNames)
{
	activeOutputs.clear();
	activeOutputIndices.clear();
	for (auto &outName : outNames)
	{
		auto vI = info->getVariableInfo(outName);
		if ((vI.index >= 0)&&(vI.type==fmi_type_t::real)&&(vI.causality==fmi_causality_type_t::output))
		{
			activeOutputs.push(vI.valueRef);
			activeOutputIndices.push_back(vI.index);
		}
		//TODO:: what to do if this condition is not valid?  
	}
}

void fmi2Object::setOutputVariables(const std::vector<int> outIndices)
{
	activeOutputIndices.clear();
	activeOutputs.clear();
	for (auto &outIndex : outIndices)
	{
		auto vI = info->getVariableInfo(outIndex);
		if ((vI.index >= 0) && (vI.type == fmi_type_t::real) && (vI.causality == fmi_causality_type_t::output))
		{
			activeOutputs.push(vI.valueRef);
			activeOutputIndices.push_back(vI.index);
		}
	}
}

void fmi2Object::setInputVariables(const std::vector<std::string> inNames)
{
	activeInputs.clear();
	activeInputIndices.clear();
	for (auto &inName : inNames)
	{
		auto vI = info->getVariableInfo(inName);
		if ((vI.index >= 0) && (vI.type == fmi_type_t::real) && (vI.causality == fmi_causality_type_t::input))
		{
			activeInputs.push(vI.valueRef);
			activeInputIndices.push_back(vI.index);
		}
	}
}

void fmi2Object::setInputVariables(const std::vector<int> inIndices)
{
	activeInputs.clear();
	activeInputIndices.clear();
	for (auto &inIndex : inIndices)
	{
		auto vI = info->getVariableInfo(inIndex);
		if ((vI.index >= 0) && (vI.type == fmi_type_t::real) && (vI.causality == fmi_causality_type_t::input))
		{
			activeInputs.push(vI.valueRef);
			activeInputIndices.push_back(vI.index);
		}
	}
}


void fmi2Object::setInputs(const fmi2Real inputs[])
{
	auto ret = commonFunctions->fmi2SetReal(comp, activeInputs.getValueRef(), activeInputs.getVRcount(),inputs);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}


void fmi2Object::getCurrentInputs(fmi2Real inputs[])
{
	auto ret= commonFunctions->fmi2GetReal(comp, activeInputs.getValueRef(), activeInputs.getVRcount(), inputs);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

void fmi2Object::getOutputs(fmi2Real outputs[]) const
{
	auto ret=commonFunctions->fmi2GetReal(comp, activeOutputs.getValueRef(), activeInputs.getVRcount(), outputs);
	if (ret != fmi2Status::fmi2OK)
	{
		handleNonOKReturnValues(ret);
	}
}

fmi2Real fmi2Object::getOutput(size_t outNum) const
{
	fmi2Real out=-1e48;
	if (outNum < activeOutputs.getVRcount())
	{
		commonFunctions->fmi2GetReal(comp, activeOutputs.getValueRef() + outNum, 1, &out);
	}
	return out;
}


fmiVariableSet fmi2Object::getVariableSet(const std::string &variable) const
{
	return info->getVariableSet(variable);
}

fmiVariableSet fmi2Object::getVariableSet(int index) const
{
	return info->getVariableSet(index);
}

std::vector<std::string> fmi2Object::getOutputNames() const
{
	std::vector<std::string> oVec;
	oVec.reserve(activeOutputs.getVRcount());
	for (auto &os : activeOutputIndices)
	{
		oVec.push_back(info->getVariableInfo(os).name);
	}
	return oVec;
}

std::vector<std::string> fmi2Object::getInputNames() const
{
	std::vector<std::string> oVec;
	oVec.reserve(activeInputs.getVRcount());
	for (auto &os : activeInputIndices)
	{
		oVec.push_back(info->getVariableInfo(os).name);
	}
	return oVec;
}


bool fmi2Object::isParameter(const std::string &param, fmi_type_t type)
{
	auto vi = info->getVariableInfo(param);
	if (vi.index >= 0)
	{
		if ((vi.causality == fmi_causality_type_t::parameter) || (vi.causality == fmi_causality_type_t::input))
		{
			if (vi.type == type)
			{
				return true;
			}
			if (type == fmi_type_t::numeric)
			{
				if (vi.type != fmi_type_t::string)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool fmi2Object::isVariable(const std::string &var, fmi_type_t type)
{
	auto vi = info->getVariableInfo(var);
	if (vi.index >= 0)
	{	
		if (vi.type == type)
		{
			return true;
		}
		if (type == fmi_type_t::numeric)
		{
			if (vi.type != fmi_type_t::string)
			{
				return true;
			}
		}
	}

	return false;
}

void fmi2Object::handleNonOKReturnValues(fmi2Status retval) const
{
	switch (retval)
	{
	case fmi2Status::fmi2OK:
		return;
	case fmi2Status::fmi2Pending:

	case fmi2Status::fmi2Discard:
		if (exceptionOnDiscard)
		{
			throw(fmiDiscardException());
		}
		break;
	case fmi2Status::fmi2Warning:
		throw(fmiWarningException());
	case fmi2Status::fmi2Error:
		throw(fmiErrorException());
	case fmi2Status::fmi2Fatal:
		throw(fmiFatalException());
	default:
		throw(fmiException());
	}
}