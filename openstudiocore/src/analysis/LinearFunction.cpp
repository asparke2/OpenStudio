/**********************************************************************
*  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
*  All rights reserved.
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#include <analysis/LinearFunction.hpp>
#include <analysis/LinearFunction_Impl.hpp>

#include <analysis/Variable.hpp>

#include <utilities/data/Vector.hpp>
#include <utilities/core/Containers.hpp>
#include <utilities/core/Optional.hpp>
#include <utilities/core/Assert.hpp>

#include <boost/foreach.hpp>

namespace openstudio {
namespace analysis {

namespace detail {

  LinearFunction_Impl::LinearFunction_Impl(const std::string& name,
                                           const std::vector<Variable>& variables,
                                           const std::vector<double>& coefficients)
    : Function_Impl(name,variables), m_coefficients(coefficients)
  {
    if (!m_coefficients.empty() && (m_coefficients.size() != m_variables.size())) {
      LOG_AND_THROW("A linear function must either have no coefficients (assumed to be a vector "
        << "of all ones), or the sizes of the variable and coefficient vectors must be equal.");
    }
  }

  LinearFunction_Impl::LinearFunction_Impl(const UUID& uuid,
                                           const UUID& versionUUID,
                                           const std::string& name,
                                           const std::string& displayName,
                                           const std::string& description,
                                           const std::vector<Variable>& variables,
                                           const std::vector<double>& coefficients)
    : Function_Impl(uuid,versionUUID,name,displayName,description,variables),
      m_coefficients(coefficients)
  {}

  LinearFunction_Impl::LinearFunction_Impl(const LinearFunction_Impl &other)
    : Function_Impl(other),
      m_coefficients(other.coefficients())
  {}

  AnalysisObject LinearFunction_Impl::clone() const {
    boost::shared_ptr<LinearFunction_Impl> impl(new LinearFunction_Impl(*this));
    LinearFunction result(impl);
    VariableVector variables = result.variables();
    BOOST_FOREACH(Variable& variable,variables) {
      variable.setParent(result);
    }
    return result;
  }

  std::vector<double> LinearFunction_Impl::coefficients() const {
    return m_coefficients;
  }

  double LinearFunction_Impl::getValue(const DataPoint& dataPoint) const {
    VariableVector variables = this->variables();
    DoubleVector variableValues, coefficients;
    BOOST_FOREACH(const Variable& variable, variables) {
      variableValues.push_back(variable.getValue(dataPoint));
    }
    OptionalDouble result;
    coefficients = this->coefficients();
    if (coefficients.empty()) {
      result = sum(createVector(variableValues));
    }
    else {
      result = dot(createVector(coefficients),createVector(variableValues));
    }
    BOOST_ASSERT(result);
    return *result;
  }

  bool LinearFunction_Impl::setCoefficients(const std::vector<double>& coefficients) {
    if (coefficients.empty() || (coefficients.size() == m_variables.size())) {
      m_coefficients = coefficients;
      onChange(AnalysisObject_Impl::InvalidatesResults);
      return true;
    }
    return false;
  }

} // detail

LinearFunction::LinearFunction(const std::string& name,
                               const std::vector<Variable>& variables,
                               const std::vector<double>& coefficients)
  : Function(boost::shared_ptr<detail::LinearFunction_Impl>(
                 new detail::LinearFunction_Impl(name,variables,coefficients)))
{
  LinearFunction copyOfThis(getImpl<detail::LinearFunction_Impl>());
  BOOST_FOREACH(const Variable& variable,variables) {
    variable.setParent(copyOfThis);
  }
}

LinearFunction::LinearFunction(const UUID& uuid,
                               const UUID& versionUUID,
                               const std::string& name,
                               const std::string& displayName,
                               const std::string& description,
                               const std::vector<Variable>& variables,
                               const std::vector<double>& coefficients)
  : Function(boost::shared_ptr<detail::LinearFunction_Impl>(
                 new detail::LinearFunction_Impl(uuid,
                                                 versionUUID,
                                                 name,
                                                 displayName,
                                                 description,
                                                 variables,
                                                 coefficients)))
{
  LinearFunction copyOfThis(getImpl<detail::LinearFunction_Impl>());
  BOOST_FOREACH(const Variable& variable,variables) {
    variable.setParent(copyOfThis);
  }
}

std::vector<double> LinearFunction::coefficients() const {
  return getImpl<detail::LinearFunction_Impl>()->coefficients();
}

bool LinearFunction::setCoefficients(const std::vector<double>& coefficients) {
  return getImpl<detail::LinearFunction_Impl>()->setCoefficients(coefficients);
}

/// @cond

LinearFunction::LinearFunction(boost::shared_ptr<detail::LinearFunction_Impl> impl)
  : Function(impl)
{}

/// @endcond

} // analysis
} // openstudio
