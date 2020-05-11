/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2018, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

#ifndef GRIDSECONDARY_H_
#define GRIDSECONDARY_H_
#include "gridComponent.h"

namespace griddyn {
class gridBus;

/** @brief base class for objects that can attach to a bus like Load and Generator
 *  gridSecondary class defines the interface for secondary objects which are nominally objects that
 *hang directly off a bus
 **/
class gridSecondary: public gridComponent {
  protected:
    gridBus* bus = nullptr;  //!< reference to the bus connecting the object;
  public:
    /** @brief default constructor*/
    explicit gridSecondary(const std::string& objName = "");

    virtual coreObject* clone(coreObject* obj = nullptr) const override;

    virtual void updateObjectLinkages(coreObject* newRoot) override;

    virtual void pFlowInitializeA(coreTime time0, std::uint32_t flags) override final;

    virtual void pFlowInitializeB() override final;

    virtual void dynInitializeA(coreTime time0, std::uint32_t flags) override final;

    virtual void dynInitializeB(const IOdata& inputs,
                                const IOdata& desiredOutput,
                                IOdata& fieldSet) override final;

  protected:
    virtual void pFlowObjectInitializeA(coreTime time0, std::uint32_t flags) override;

  public:
    virtual void set(const std::string& param, const std::string& val) override;

    virtual void
        set(const std::string& param, double val, units::unit unitType = units::defunit) override;

    /**
    *@brief get the real output power
    @details  these are the main data output functions for secondary object.  The intention is to
    supply the load/gen to the bus they may have other outputs but first and second are the real and
    reactive power negative numbers indicate power generation
    @param[in] inputs the inputs for the secondary object
    * @param[in] sD the current state data for the simulation
    * @param[in] sMode the mode the solver is in
    @return the real output power consumed or generated by the device
    **/
    virtual double
        getRealPower(const IOdata& inputs, const stateData& sD, const solverMode& sMode) const;

    /**
    *@brief get the reactive output power
    @param[in] inputs the inputs for the secondary object
    * @param[in] sD the current state data for the simulation
    * @param[in] sMode the mode the solver is in
    @return the reactive output power consumed or generated by the device
    **/
    virtual double
        getReactivePower(const IOdata& inputs, const stateData& sD, const solverMode& sMode) const;

    /**
    *@brief get the real output power from local data
    @return a the real power output
    **/
    virtual double getRealPower() const;

    /**
    *@brief get the reactive output power from local data
    @return a the reactive power output
    **/
    virtual double getReactivePower() const;

    /**
    *@brief get a vector of all outputs
    @param[in] inputs the inputs for the secondary object
    * @param[in] sD the current state data for the simulation
    * @param[in] sMode the mode the solver is in
    @return a vector containing  all the outputs
    **/
    virtual IOdata getOutputs(const IOdata& inputs,
                              const stateData& sD,
                              const solverMode& sMode) const override;

    const gridBus* getBus() const { return bus; };

    /**
    *@brief get the time derivative of a single state
    @param[in] inputs the inputs for the secondary object
    * @param[in] sD the current state data for the simulation
    * @param[in] sMode the mode the solver is in
    @param[in] num the number of the state being requested
    @return the value of the time derivative of a state being requested
    **/
    virtual double getDoutdt(const IOdata& inputs,
                             const stateData& sD,
                             const solverMode& sMode,
                             index_t num = 0) const override;

    /**
    *@brief get a single state
    @param[in] inputs the inputs for the secondary object
    * @param[in] sD the current state data for the simulation
    * @param[in] sMode the mode the solver is in
    @param[in] num the number of the state being requested
    @return the value of the state being requested
    **/
    virtual double getOutput(const IOdata& inputs,
                             const stateData& sD,
                             const solverMode& sMode,
                             index_t outputNum = 0) const override;

    /**
    *@brief get a single state based on local data
    @param[in] the number of the state being requested
    @return the value of the state being requested
    **/
    virtual double getOutput(index_t outputNum = 0) const override;

    /**
    *@brief predict what the outputs will be at a specific time in the future
    @param[in] ptime the time for which to make a prediction
    @param[in] inputs the inputs for the secondary object
    * @param[in] sD the current state data for the simulation
    * @param[in] sMode the mode the solver is in
    @return a vector containing  all the predicted outputs
    **/
    virtual IOdata predictOutputs(coreTime predictionTime,
                                  const IOdata& inputs,
                                  const stateData& sD,
                                  const solverMode& sMode) const;

    /**
    *@brief get the available upwards generating capacity of a system
    @param[in] time the time period within which to do the adjustment
    @return the available up capacity of the gridSecondary unit
    **/
    virtual double getAdjustableCapacityUp(coreTime time = maxTime) const;

    /**
    *@brief get the available downwards generating capacity of a system
    @param[in] the time period within which to do the adjustment
    @return the available up capacity of the gridSecondary unit
    **/
    virtual double getAdjustableCapacityDown(coreTime time = maxTime) const;

    virtual const std::vector<stringVec>& inputNames() const override;
    virtual const std::vector<stringVec>& outputNames() const override;

    virtual units::unit inputUnits(index_t inputNum) const override;

    virtual units::unit outputUnits(index_t outputNum) const override;
};

}  // namespace griddyn
#endif
