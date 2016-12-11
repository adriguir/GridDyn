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

#ifndef FNCS_LOAD_HEADER_
#define FNCS_LOAD_HEADER_

#include "loadModels/otherLoads.h"

class fncsLoad:public gridRampLoad
{
public:
	enum fncs_load_flags
	{
		use_ramp = object_flag8,
		predictive_ramp = object_flag9,
		initial_query = object_flag10,
	};
protected:
	std::string voltageKey;			//!< the key to send voltage
	std::string loadKey; 	//!< time series containing the load information
	gridUnits::units_t inputUnits = gridUnits::defUnit;
	double scaleFactor = 1.0;			//!< scaling factor on the load
private:
	double prevP = 0;
	double prevQ = 0;
public:
	explicit fncsLoad(const std::string &objName = "fncsLoad_$");

	~fncsLoad()
	{
	}
	coreObject * clone(coreObject *obj = nullptr) const override;
	virtual void pFlowObjectInitializeA(gridDyn_time time0, unsigned long flags) override;
	virtual void pFlowObjectInitializeB() override;

	virtual void updateA(gridDyn_time time) override;
	virtual gridDyn_time updateB() override;
	virtual void timestep(gridDyn_time ttime, const IOdata &args, const solverMode &sMode) override;
	virtual void setFlag(const std::string &param, bool val = true) override;
	virtual void set(const std::string &param, const std::string &val) override;
	virtual void set(const std::string &param, double val, gridUnits::units_t unitType = gridUnits::defUnit) override;
	
};
#endif