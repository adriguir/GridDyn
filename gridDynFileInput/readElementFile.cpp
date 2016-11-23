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

#include "readElement.h"

#include "gridDyn.h"
#include "readerHelper.h"
#include "readerElement.h"
#include "elementReaderTemplates.hpp"

#include <boost/filesystem.hpp>
#include <sstream>

using namespace readerConfig;

void loadElementInformation (gridCoreObject *obj, std::shared_ptr<readerElement> &element, const std::string &objectName, readerInfo *ri, const IgnoreListType &ignoreList)
{
  objSetAttributes (obj, element, objectName, ri, ignoreList);
  readImports (element, ri, obj, false);
  // check for child objects
  loadSubObjects (element, ri, obj);

  //get all element fields
  paramLoopElement (obj, element, objectName, ri, ignoreList);
  readImports (element, ri, obj, true);
}

static const std::string importString ("import");
void readImports (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *parentObject, bool finalFlag)
{
  if (element->hasElement (importString) == false)
    {
      return;
    }

  //run any source files
  auto bflags = ri->flags;
  element->bookmark ();
  element->moveToFirstChild (importString);
  while (element->isValid ())
    {
      bool finalMode = false;
      std::string fstring = getElementField (element, "final", defMatchType);

      if ((fstring == "true")||(fstring == "1"))
        {
          finalMode = true;
        }

      if (finalFlag != finalMode)
        {
          element->moveToNextSibling (importString);
          continue;
        }

      std::string flags = getElementField (element, "flags", defMatchType);
      if (!flags.empty ())
        {
          ri->flags = addflags (ri->flags, flags);
        }
      std::string sourceFile = getElementField (element, "file", defMatchType);
      if (sourceFile.empty ())
        {
          //if we don't find a field named file, just use the text in the source element
          sourceFile = element->getText ();
        }

      //check through the files to find the right location
      ri->checkFileParam (sourceFile,true);

      boost::filesystem::path sourcePath (sourceFile);
      std::string prefix = getElementField (element,"prefix",match_type::capital_case_match);
      //get the prefix if any
      if (prefix.empty ())
        {
          prefix = ri->prefix;
        }
      else if (!(ri->prefix.empty ()))
        {
          prefix = ri->prefix + '_' + prefix;
        }

      //check for type override
      std::string ext = convertToLowerCase (getElementField (element, "filetype", defMatchType));

      std::swap (prefix, ri->prefix);
      if (ext.empty ())
        {
          loadFile (parentObject, sourceFile, ri);
        }
      else
        {
          loadFile (parentObject, sourceFile, ri, ext);
        }
      std::swap (prefix, ri->prefix);

      ri->flags = bflags;
      element->moveToNextSibling (importString);                    // next import file
    }
  element->restore ();
}

static const std::string unitString1("units");
static const std::string unitString2("unit");

gridUnits::units_t readUnits(const std::shared_ptr<readerElement> &element, const std::string &field)
{
	std::string uname = element->getAttributeText(unitString1);
	//actually specifying a "unit" attribute takes precedence
	if (uname.empty())
	{
		uname = element->getAttributeText(unitString2);
	}
	if (!uname.empty())
	{
		auto retUnits = gridUnits::getUnits(uname);
		if (retUnits == gridUnits::defUnit)
		{
			WARNPRINT(READER_WARN_ALL, "unknown unit " << uname);
		}
		return retUnits;
	}
	if (field.back() == ')')
	{
		auto p = field.find_last_of('(');
		
		if (p != std::string::npos)
		{
			uname = field.substr(p + 1, field.length() - 2 - p);
			auto retUnits= gridUnits::getUnits(uname);
			if (retUnits == gridUnits::defUnit)
			{
				WARNPRINT(READER_WARN_ALL, "unknown unit " << uname);
			}
			return retUnits;
		}
	}
	return gridUnits::defUnit;
}

static const std::string valueString("value");

gridParameter * getElementParam (const std::shared_ptr<readerElement> &element, gridParameter *param)
{
  gridParameter *ret;

  if (param == nullptr)
    {
      ret = new gridParameter ();  //TODO:  PT this may not be the best thing to do
    }
  else
    {
      ret = param;
      ret->paramUnits = gridUnits::defUnit;
      ret->valid = false;
    }
  std::string fname = convertToLowerCase (element->getName ());

  if (fname == "param")
    {
      std::string pname = element->getAttributeText ("name");
      if (pname.empty ())
        {
          pname = element->getAttributeText ("field");
        }
      if (pname.empty ())
        {
          //no name or field attribute so just read the string and see if we can process it
          ret->fromString (element->getText ());
          return ret;
        }
	  ret->paramUnits = readUnits(element,pname);
      if (pname.back () == ')')
        {
          auto p = pname.find_last_of ('(');
          if (p != std::string::npos)
            {
			  pname.erase(p);
            }
        }
      ret->field = convertToLowerCase (pname);
      if (element->hasAttribute (valueString))
        {
          ret->value = element->getAttributeValue (valueString);
          if (ret->value == readerNullVal)
            {
              ret->stringType = true;
              ret->strVal = element->getAttributeText (valueString);
            }
          else
            {
              ret->stringType = false;
            }

        }
      else
        {
          ret->value = element->getValue ();
          if (ret->value == readerNullVal)
            {
              ret->stringType = true;
              ret->strVal = element->getText ();
            }
          else
            {
              ret->stringType = false;
            }

        }
	  
	  

    }
  // all other properties
  else
    {
	  ret->paramUnits = readUnits(element,fname);
      if (fname.back () == ')')
        {
		  auto p = fname.find_last_of('(');
		  if (p != std::string::npos)
		  {
			  fname.erase(p);
		  }
        }
      ret->field = fname;
      ret->value = element->getValue ();
      if (ret->value == readerNullVal)
        {
          ret->stringType = true;
          ret->strVal = element->getText ();
        }
      else
        {
          ret->stringType = false;
        }

	  

    }
  ret->valid = true;
  return ret;
}

static const IgnoreListType keywords {
  "type", "ref", "number","index", "retype","name","define","library","import","area","bus","link","load","exciter",
  "source","governor","controlblock", "pss", "simulation","generator","array","relay", "parent", "genmodel", "line", "solver",
  "agc", "reserve", "reservedispatch", "dispatch","econ","configuration", "custom", "purpose"
};

void objSetAttributes (gridCoreObject *obj, std::shared_ptr<readerElement> &aP, const std::string &typeName, readerInfo *ri, const IgnoreListType &ignoreList)
{

  auto att = aP->getFirstAttribute ();
  while (att.isValid ())
    {
      gridUnits::units_t  unitType = gridUnits::defUnit;
      std::string fname = convertToLowerCase (att.getName ());

      if (fname.back () == ')')
        {
          auto p = fname.find_last_of ('(');
          if (p != std::string::npos)
            {
              std::string ustring = fname.substr (p + 1, fname.length () - 2 - p);
              unitType = gridUnits::getUnits (ustring);
              fname = fname.substr (0, p - 1);
            }
        }
      auto ifind = keywords.find (fname);
      if (ifind != keywords.end ())
        {
          att = aP->getNextAttribute ();
          continue;
        }
      ifind = ignoreList.find (fname);
      if (ifind != ignoreList.end ())
        {
          att = aP->getNextAttribute ();
          continue;
        }

      else if (fname.find ("file") != std::string::npos)
        {
          std::string strVal = att.getText ();
          ri->checkFileParam (strVal);
		  gridParameter po(fname, strVal);
		  objectParameterSet(typeName, obj, po);
        }
      else if ((fname.find ("workdir") != std::string::npos) || (fname.find ("directory") != std::string::npos))
        {
          std::string strVal = att.getText ();
          ri->checkDirectoryParam (strVal);
		  gridParameter po(fname, strVal);
		  objectParameterSet(typeName, obj, po);
        }
      else if ((fname == "flag") || (fname == "flags"))     //read the flags parameter
        {

          auto v = splitlineTrim (att.getText ());

          for (auto &temp : v)
            {
              makeLowerCase (temp);                         //make the flags lower case
			  try
			  {
				  obj->setFlag(temp, true);
			  }
			  catch (const unrecognizedParameter &)
			  {
				  WARNPRINT(READER_WARN_ALL, "flag " << temp << " not found");
			  }
             
            }
        }
      else
        {
          double val = att.getValue ();
          if (val != readerNullVal)
            {
			  gridParameter po(fname, val);
			  po.paramUnits = unitType;
			  objectParameterSet(typeName, obj, po);
              
            }
          else
            {
              gridParameter po (fname, att.getText ());
              paramStringProcess (&po, ri);
              objectParameterSet (typeName, obj, po);
            }

        }
      att = aP->getNextAttribute ();
    }

}


void paramLoopElement (gridCoreObject *obj, std::shared_ptr<readerElement> &aP, const std::string &typeName, readerInfo *ri, const IgnoreListType &ignoreList)
{

  gridParameter param;

  aP->moveToFirstChild ();
  while (aP->isValid ())
    {
      std::string fname = convertToLowerCase (aP->getName ());
      auto ifind = keywords.find (fname);
      if (ifind != keywords.end ())
        {
          aP->moveToNextSibling ();
          continue;
        }
      ifind = ri->getIgnoreList ().find (fname);
      if (ifind != ri->getIgnoreList ().end ())
        {
          aP->moveToNextSibling ();
          continue;
        }
      ifind = ignoreList.find (fname);
      if (ifind != ignoreList.end ())
        {
          aP->moveToNextSibling ();
          continue;
        }
	  //TODO:: PT move the event and recorder loading to a separate function
      if ((fname == "recorder")||(fname=="collector"))
        {
          loadCollectorElement (aP, obj, ri);
        }
      // event
      else if ((fname == "event")||(fname=="player"))
        {
          loadEventElement (aP, obj, ri);
        }
      else           //get all the parameter fields
        {
          getElementParam (aP, &param);
          if (param.valid)
            {
              if (param.stringType == true)
                {
                  if (param.field.find ("file") != std::string::npos)
                    {
                      ri->checkFileParam (param.strVal);
                      objectParameterSet (typeName, obj, param);
                    }
                  else if ((param.field.find ("workdir") != std::string::npos) || (param.field.find ("directory") != std::string::npos))
                    {
                      ri->checkDirectoryParam (param.strVal);
                      objectParameterSet (typeName, obj, param);
                    }
                  else if ((fname == "flag") || (fname == "flags"))                 //read the flags parameter
                    {
                      paramStringProcess (&param, ri);
                      auto v = splitlineTrim (param.strVal);
                      for (auto &temp : v)
                        {
                          makeLowerCase (temp);                                     //make the flags lower case
						  try
						  {
							  obj->setFlag(temp, true);
						  }
                          catch (const unrecognizedParameter &)
                            {
                              WARNPRINT (READER_WARN_ALL, "flag " << temp << " not found");
                            }
                        }
                    }
                  else
                    {

                      paramStringProcess (&param, ri);
                      objectParameterSet (typeName, obj, param);
                    }
                }
              else
                {
                  objectParameterSet (typeName, obj, param);
                }
            }
        }
      aP->moveToNextSibling ();
    }
  aP->moveToParent ();
}


void readConfigurationFields (std::shared_ptr<readerElement> &sim, readerInfo *)
{
  if (sim->hasElement ("configuration"))
    {
      sim->bookmark ();
      sim->moveToFirstChild ("configuration");
      auto cfgAtt = sim->getFirstAttribute ();
      while (cfgAtt.isValid ())
        {
          if ((cfgAtt.getName () == "matching") || (cfgAtt.getName () == "match_type"))
            {
              readerConfig::setDefaultMatchType (cfgAtt.getText ());
            }
          else if (cfgAtt.getName () == "printlevel")
            {
              readerConfig::setPrintMode (cfgAtt.getText ());
            }
          cfgAtt = sim->getNextAttribute ();
        }

      sim->moveToFirstChild ();
      while (sim->isValid ())
        {
          std::string fname = sim->getName ();
          if ((fname == "matching") || (fname == "match_type"))
            {
              readerConfig::setDefaultMatchType (sim->getText ());
            }
          else if (fname == "printlevel")
            {
              readerConfig::setPrintMode (cfgAtt.getText ());
            }
          sim->moveToNextSibling ();
        }

      sim->restore ();
    }


}