/**********************************************************************
 *  Copyright (c) 2008-2012, Alliance for Sustainable Energy.
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

#include <ruleset/OSArgument.hpp>

#include <utilities/idf/WorkspaceObject.hpp>

#include <utilities/idd/IddObject.hpp>

#include <utilities/units/QuantityFactory.hpp>

#include <utilities/core/Assert.hpp>
#include <utilities/core/Containers.hpp>
#include <utilities/core/Compare.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <sstream>

namespace openstudio {
namespace ruleset {

OSArgument::OSArgument(const UUID& uuid,
                       const UUID& versionUUID,
                       const std::string& name,
                       const std::string& displayName,
                       const OSArgumentType& type,
                       bool required,
                       const boost::optional<std::string>& value,
                       const boost::optional<std::string>& defaultValue,
                       const OSDomainType& domainType,
                       std::vector<std::string>& domain,
                       const std::vector<std::string>& choices,
                       const std::vector<std::string>& choiceDisplayNames,
                       bool isRead,
                       const std::string& extension)
  : m_uuid(uuid),
    m_name(name),
    m_displayName(displayName),
    m_type(type),
    m_required(required),
    m_domainType(domainType),
    m_choices(choices),
    m_choiceDisplayNames(choiceDisplayNames),
    m_isRead(isRead),
    m_extension(extension)
{
  bool result;
  if (value) {
    result = setValue(value.get());
    BOOST_ASSERT(result);
  }
  if (defaultValue) {
    result = setDefaultValue(defaultValue.get());
    BOOST_ASSERT(result);
  }
  if (!domain.empty()) {
    result = setDomain(domain);
    BOOST_ASSERT(result);
  }
  m_versionUUID = versionUUID;
}

OSArgument OSArgument::clone() const {
  OSArgument result(*this);
  result.m_uuid = createUUID();
  result.m_versionUUID = createUUID();
  return result;
}

OSArgument OSArgument::makeBoolArgument(const std::string& name,bool required)
{
  OSArgument result(name, OSArgumentType::Boolean, required);
  result.setDomainType(OSDomainType::Enumeration);
  result.m_choices.push_back("true");
  result.m_choices.push_back("false");
  return result;
}

OSArgument OSArgument::makeDoubleArgument(const std::string& name,bool required)
{
  OSArgument result(name, OSArgumentType::Double, required);
  result.setDomainType(OSDomainType::Interval);
  return result;
}

OSArgument OSArgument::makeQuantityArgument(const std::string& name,bool required)
{
  OSArgument result(name, OSArgumentType::Quantity, required);
  result.setDomainType(OSDomainType::Interval);
  return result;
}

OSArgument OSArgument::makeIntegerArgument(const std::string& name,bool required)
{
  OSArgument result(name, OSArgumentType::Integer, required);
  result.setDomainType(OSDomainType::Interval);
  return result;
}

OSArgument OSArgument::makeStringArgument(const std::string& name,bool required)
{
  OSArgument result(name, OSArgumentType::String, required);
  result.setDomainType(OSDomainType::Enumeration);
  return result;
}

OSArgument OSArgument::makeChoiceArgument(const std::string& name,
                                          const std::vector<std::string>& choices,
                                          bool required)
{
  return makeChoiceArgument(name,choices,choices,required);
}

OSArgument OSArgument::makeChoiceArgument(const std::string& name,
                                          const std::vector<std::string>& choices,
                                          const std::vector<std::string>& displayNames,
                                          bool required)
{
  OSArgument result(name, OSArgumentType::Choice, required);
  result.setDomainType(OSDomainType::Enumeration);
  result.m_choices = choices;
  result.m_choiceDisplayNames = displayNames;
  return result;
}

OSArgument OSArgument::makePathArgument(const std::string& name,
                                        bool isRead,
                                        const std::string& extension,
                                        bool required)
{
  OSArgument result(name, OSArgumentType::Path, required);
  result.setDomainType(OSDomainType::Enumeration);
  result.m_isRead = isRead;
  result.m_extension = extension;
  return result;
}

openstudio::UUID OSArgument::uuid() const {
  return m_uuid;
}

openstudio::UUID OSArgument::versionUUID() const {
  return m_versionUUID;
}

std::string OSArgument::name() const {
  return m_name;
}

std::string OSArgument::displayName() const {
  return m_displayName;
}

OSArgumentType OSArgument::type() const {
  return m_type;
}

bool OSArgument::required() const {
  return m_required;
}

bool OSArgument::hasValue() const {
  return (!m_value.isNull());
}

bool OSArgument::valueAsBool() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  if ("true" == m_value.toString()){
    return true;
  }
  return false;
}

double OSArgument::valueAsDouble() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return m_value.toDouble();
}

Quantity OSArgument::valueAsQuantity() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return m_value.value<openstudio::Quantity>();
}

int OSArgument::valueAsInteger() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return m_value.toInt();
}

std::string OSArgument::valueAsString() const
{
  if (!hasValue()) {
    LOG_AND_THROW("Argument " << name() << " has no value.");
  }
  return printQVariant(m_value);
}

openstudio::path OSArgument::valueAsPath() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return toPath(m_value.toString());
}

QVariant OSArgument::valueAsQVariant() const {
  return m_value;
}

bool OSArgument::hasDefaultValue() const {
  return (!m_defaultValue.isNull());
}

bool OSArgument::defaultValueAsBool() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  if ("true" == m_defaultValue.toString()){
    return true;
  }
  return false;
}

double OSArgument::defaultValueAsDouble() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return m_defaultValue.toDouble();
}

Quantity OSArgument::defaultValueAsQuantity() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return m_defaultValue.value<openstudio::Quantity>();
}

int OSArgument::defaultValueAsInteger() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return m_defaultValue.toInt();
}

std::string OSArgument::defaultValueAsString() const
{
  if (!hasDefaultValue()) {
    LOG_AND_THROW("Argument " << name() << " does not have a default value.");
  }
  return printQVariant(m_defaultValue);
}

openstudio::path OSArgument::defaultValueAsPath() const
{
  // TODO: Throw if (!hasValue()) or wrong type.
  return toPath(m_defaultValue.toString());
}

QVariant OSArgument::defaultValueAsQVariant() const {
  return m_defaultValue;
}

bool OSArgument::hasDomain() const {
  return !m_domain.empty();
}

OSDomainType OSArgument::domainType() const {
  return m_domainType;
}

std::vector<bool> OSArgument::domainAsBool() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  BoolVector result;
  BOOST_FOREACH(const QVariant& value,m_domain) {
    if ("true" == value.toString()) {
      result.push_back(true);
    }
    result.push_back(false);
  }
  return result;
}

std::vector<double> OSArgument::domainAsDouble() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  DoubleVector result;
  BOOST_FOREACH(const QVariant& value,m_domain) {
    result.push_back(value.toDouble());
  }
  return result;
}

std::vector<Quantity> OSArgument::domainAsQuantity() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  QuantityVector result;
  BOOST_FOREACH(const QVariant& value,m_domain) {
    result.push_back(value.value<openstudio::Quantity>());
  }
  return result;
}

std::vector<int> OSArgument::domainAsInteger() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  IntVector result;
  BOOST_FOREACH(const QVariant& value,m_domain) {
    result.push_back(value.toInt());
  }
  return result;
}

std::vector<std::string> OSArgument::domainAsString() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  StringVector result;
  BOOST_FOREACH(const QVariant& value,m_domain) {
    result.push_back(printQVariant(value));
  }
  return result;
}

std::vector<openstudio::path> OSArgument::domainAsPath() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  std::vector<openstudio::path> result;
  BOOST_FOREACH(const QVariant& value,m_domain) {
    result.push_back(toPath(value.toString()));
  }
  return result;
}

std::vector<QVariant> OSArgument::domainAsQVariant() const {
  if (!hasDomain()) {
    LOG_AND_THROW("No domain set for OSArgument '" << name() << "'.");
  }
  return m_domain;
}

std::vector<std::string> OSArgument::choiceValues() const {
  return m_choices;
}

std::vector<std::string> OSArgument::choiceValueDisplayNames() const {
  return m_choiceDisplayNames;
}

std::string OSArgument::valueDisplayName() const {
  std::string valueString = valueAsString();
  int index = int(std::find(m_choices.begin(),m_choices.end(),valueString) -
                  m_choices.begin());
  if (index < int(m_choiceDisplayNames.size())) {
    return m_choiceDisplayNames[index];
  }
  return valueString;
}

std::string OSArgument::defaultValueDisplayName() const {
  std::string defaultValueString = defaultValueAsString();
  int index = int(std::find(m_choices.begin(),m_choices.end(),defaultValueString) -
                  m_choices.begin());
  if (index < int(m_choiceDisplayNames.size())) {
    return m_choiceDisplayNames[index];
  }
  return defaultValueString;
}


bool OSArgument::isRead() const {
  return m_isRead;
}

std::string OSArgument::extension() const {
  return m_extension;
}

void OSArgument::setDisplayName(const std::string& displayName) {
  m_displayName = displayName;
  onChange();
}

bool OSArgument::setValue(bool value) {
  bool result = false;
  if (m_type == OSArgumentType::Boolean) {
    if (value) {
      m_value.setValue(QString("true"));
    }
    else {
      m_value.setValue(QString("false"));
    }
    BOOST_ASSERT(hasValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setValue(double value) {
  bool result = false;
  if (m_type == OSArgumentType::Double) {
    m_value.setValue(value);
    BOOST_ASSERT(hasValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setValue(const Quantity& value) {
  bool result = false;
  if (m_type == OSArgumentType::Quantity) {
    m_value = QVariant::fromValue<openstudio::Quantity>(value);
    BOOST_ASSERT(hasValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setValue(int value) {
  bool result = false;
  if (m_type == OSArgumentType::Integer) {
    m_value.setValue(value);
    BOOST_ASSERT(hasValue());
    onChange();
    result = true;
  }
  else if (m_type == OSArgumentType::Double) {
    result = setValue(double(value));
  }
  return result;
}

bool OSArgument::setValue(const std::string& value) {
  bool result = setStringInternal(m_value, value);
  if (result) {
    BOOST_ASSERT(hasValue());
    onChange();
  }
  return result;
}

bool OSArgument::setValue(const char* value) {
  return setValue(std::string(value));
}

bool OSArgument::setValue(const openstudio::path& value) {
  bool result = false;
  if (m_type == OSArgumentType::Path) {
    m_value.setValue(toQString(value));
    BOOST_ASSERT(hasValue());
    onChange();
    result = true;
  }
  return result;
}

void OSArgument::clearValue() {
  m_value = QVariant();
  BOOST_ASSERT(!hasValue());
  onChange();
}

bool OSArgument::setDefaultValue(bool defaultValue) {
  bool result = false;
  if (m_type == OSArgumentType::Boolean) {
    if (defaultValue) {
      m_defaultValue.setValue(QString("true"));
    }
    else {
      m_defaultValue.setValue(QString("false"));
    }
    BOOST_ASSERT(hasDefaultValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setDefaultValue(double defaultValue) {
  bool result = false;
  if (m_type == OSArgumentType::Double){
    m_defaultValue.setValue(defaultValue);
    BOOST_ASSERT(hasDefaultValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setDefaultValue(const Quantity& value) {
  bool result = false;
  if (m_type == OSArgumentType::Quantity) {
    m_defaultValue = QVariant::fromValue<openstudio::Quantity>(value);
    BOOST_ASSERT(hasDefaultValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setDefaultValue(int defaultValue)
{
  bool result = false;
  if (m_type == OSArgumentType::Integer){
    m_defaultValue.setValue(defaultValue);
    BOOST_ASSERT(hasDefaultValue());
    onChange();
    result = true;
  }
  else if (m_type == OSArgumentType::Double) {
    result = setDefaultValue(double(defaultValue));
  }
  return result;
}

bool OSArgument::setDefaultValue(const std::string& defaultValue) {
  bool result = setStringInternal(m_defaultValue, defaultValue);
  if (result) {
    BOOST_ASSERT(hasDefaultValue());
    onChange();
  }
  return result;
}

bool OSArgument::setDefaultValue(const char* defaultValue) {
  return setDefaultValue(std::string(defaultValue));
}

bool OSArgument::setDefaultValue(const openstudio::path& defaultValue) {
  bool result = false;
  if (m_type == OSArgumentType::Path){
    m_defaultValue.setValue(toQString(defaultValue));
    BOOST_ASSERT(hasDefaultValue());
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setDomainType(const OSDomainType& domainType) {
  if (domainType != this->domainType()) {
    // check whether domainType makes sense for type()
    switch (domainType.value()) {
      case OSDomainType::Interval :
        switch (m_type.value()) {
          case OSArgumentType::Boolean :
          case OSArgumentType::String :
          case OSArgumentType::Choice :
          case OSArgumentType::Path :
            LOG(Info,"OSDomainType " << domainType.valueName() << " does not make sense for "
                << "arguments of type " << m_type.valueName() << ", such as " << name() << ".");
            return false;
          default:
           break;
        }
        break;
      case OSDomainType::Enumeration :
        break;
      default :
        BOOST_ASSERT(false);
    }

    // if so, change and clear current domain
    m_domainType = domainType;
    m_domain.clear();
    onChange();
  }
  return true;
}

bool OSArgument::setDomain(const std::vector<bool>& domain) {
  bool result(false);
  if (m_type == OSArgumentType::Boolean) {
    BOOST_ASSERT(m_domainType == OSDomainType::Enumeration);
    // could check for uniqueness, but pass on that for now
    m_domain.clear();
    BOOST_FOREACH(bool value,domain) {
      if (value) {
        m_domain.push_back(QVariant(QString("true")));
      }
      else {
        m_domain.push_back(QVariant(QString("false")));
      }
    }
    onChange();
    result = true;
  }
  return result;
}

bool OSArgument::setDomain(const std::vector<double>& domain) {
  bool result(false);
  if (m_type == OSArgumentType::Double) {
    if ((m_domainType != OSDomainType::Interval) || (domain.size() == 2u)) {
      // could check for uniqueness, min < max, but pass on that for now
      m_domain.clear();
      BOOST_FOREACH(double value, domain) {
        m_domain.push_back(QVariant(value));
      }
      onChange();
      result = true;
    }
  }
  return result;
}

bool OSArgument::setDomain(const std::vector<Quantity>& domain) {
  bool result(false);
  if (m_type == OSArgumentType::Quantity) {
    if ((m_domainType != OSDomainType::Interval) || (domain.size() == 2u)) {
      // could check for uniqueness, min < max, but pass on that for now
      m_domain.clear();
      BOOST_FOREACH(const Quantity& value, domain) {
        m_domain.push_back(QVariant::fromValue<openstudio::Quantity>(value));
      }
      onChange();
      result = true;
    }
  }
  return result;
}

bool OSArgument::setDomain(const std::vector<int>& domain) {
  bool result(false);
  if (m_type == OSArgumentType::Integer) {
    if ((m_domainType != OSDomainType::Interval) || (domain.size() == 2u)) {
      // could check for uniqueness, min < max, but pass on that for now
      m_domain.clear();
      BOOST_FOREACH(int value, domain) {
        m_domain.push_back(QVariant(value));
      }
      onChange();
      result = true;
    }
  }
  return result;
}

bool OSArgument::setDomain(const std::vector<std::string>& domain) {
  bool result(false);
  if ((m_domainType != OSDomainType::Interval) || (domain.size() == 2u)) {
    std::vector<QVariant> originalDomain = m_domain;
    m_domain.clear();
    BOOST_FOREACH(const std::string& value, domain) {
      QVariant newValue;
      result = setStringInternal(newValue,value);
      if (!result) {
        m_domain = originalDomain;
        break;
      }
      m_domain.push_back(newValue);
    }
    if (result) {
      onChange();
    }
  }
  return result;
}

bool OSArgument::setDomain(const std::vector<openstudio::path>& domain) {
  bool result(false);
  if (m_type == OSArgumentType::Path) {
    BOOST_ASSERT(m_domainType == OSDomainType::Enumeration);
    // could check for uniqueness, but pass on that for now
    m_domain.clear();
    BOOST_FOREACH(const openstudio::path& value, domain) {
      m_domain.push_back(QVariant(toQString(value)));
    }
    onChange();
    result = true;
  }
  return result;
}

void OSArgument::clearDomain() {
  m_domain.clear();
}

std::string OSArgument::print() const {
  std::stringstream ss;

  // name
  ss << name();
  if (!displayName().empty()) {
    ss << " (" << displayName() << ")";
  }
  ss << std::endl;

  // type and required
  ss << type().valueName() << ", ";
  if (required()) {
    ss << "Required";
  }
  else {
    ss << "Optional";
  }
  ss << std::endl;

  // value
  ss << "Value: ";
  if (hasValue()) {
    ss << printValue(false) << " ";
  }
  if (hasDefaultValue()) {
    ss << "(" << printDefaultValue() << ")";
  }
  ss << std::endl;

  if (m_type.value() == OSArgumentType::Choice) {
    ss << "Choices:" << std::endl;
    int dnn = m_choiceDisplayNames.size();
    for (int i = 0, n = m_choices.size(); i < n; ++i) {
      ss << "  " << m_choices[i];
      if ((i < dnn) && (!m_choiceDisplayNames[i].empty())) {
        ss << " (" << m_choiceDisplayNames[i] << ")";
      }
      ss << std::endl;
    }
  }

  if (hasDomain()) {
    ss << m_domainType.valueName() << " Domain: ";
    if (m_domainType == OSDomainType::Interval) {
      BOOST_ASSERT(m_domain.size() == 2u);
      ss << "[" << printQVariant(m_domain[0]) << ", " << printQVariant(m_domain[1]) << "]" << std::endl;
    }
    else {
      ss << std::endl;
      BOOST_FOREACH(const QVariant& value, m_domain) {
        ss << "  " << printQVariant(value) << std::endl;
      }
    }
  }

  return ss.str();

}

std::string OSArgument::printValue(bool printDefault) const {
  std::string result;

  QVariant toPrint;
  if (hasValue()) {
    toPrint = m_value;
  }
  else if (printDefault && hasDefaultValue()) {
    toPrint = m_defaultValue;
  }

  if (!toPrint.isNull()) {
    result = printQVariant(toPrint);
  }

  return result;
}

std::string OSArgument::printDefaultValue() const {
  std::string result;

  if (hasDefaultValue()) {
    result = printQVariant(m_defaultValue);
  }

  return result;
}

OSArgument::OSArgument()
  : m_uuid(createUUID()), m_versionUUID(createUUID())
{}

OSArgument::OSArgument(const std::string& name,
                       const OSArgumentType& type,
                       bool required)
  : m_uuid(createUUID()),
    m_versionUUID(createUUID()),
    m_name(name),
    m_displayName(name),
    m_type(type),
    m_required(required),
    m_isRead(false)
{}

bool OSArgument::setStringInternal(QVariant& variant, const std::string& value) {
  bool result = false;
  if (m_type == OSArgumentType::Boolean) {
    if (value == "true") {
      variant.setValue(QString("true"));
      result = true;
    }
    else if (value == "false") {
      variant.setValue(QString("false"));
      result = true;
    }
  }
  else if (m_type == OSArgumentType::Double) {
    bool test;
    double temp = toQString(value).toDouble(&test);
    if (test){
      variant.setValue(temp);
      result = true;
    }
  }
  else if (m_type == OSArgumentType::Quantity) {
    OptionalQuantity oq = createQuantity(value);
    if (oq) {
      variant = QVariant::fromValue<openstudio::Quantity>(*oq);
      result = true;
    }
  }
  else if (m_type == OSArgumentType::Integer) {
    bool test;
    int temp = toQString(value).toInt(&test);
    if (test){
      variant.setValue(temp);
      result = true;
    }
  }
  else if (m_type == OSArgumentType::String) {
    variant.setValue(toQString(value));
    result = true;
  }
  else if (m_type == OSArgumentType::Choice) {
    if (std::find(m_choices.begin(), m_choices.end(), value) != m_choices.end()){
      variant.setValue(toQString(value));
      result = true;
    }
    else {
      // can also set using display name
      StringVector::const_iterator it = std::find(m_choiceDisplayNames.begin(), m_choiceDisplayNames.end(), value);
      if (it != m_choiceDisplayNames.end()) {
        int index = int(it - m_choiceDisplayNames.begin());
        if (index < int(m_choices.size())) {
          variant.setValue(toQString(m_choices[index]));
          result = true;
        }
      }
    }
  }
  else if (m_type == OSArgumentType::Path) {
    QString temp = toQString(toPath(value));
    if (!temp.isEmpty()){
      variant.setValue(temp);
      result = true;
    }
  }
  return result;
}

std::string OSArgument::printQVariant(const QVariant& toPrint) const {
  BOOST_ASSERT(!toPrint.isNull());
  std::string result;

  std::stringstream ss;
  switch (m_type.value()) {
    case OSArgumentType::Double:
      result = toString(toPrint.toDouble());
      break;
    case OSArgumentType::Quantity:
      ss << toPrint.value<openstudio::Quantity>();
      result = ss.str();
      break;
    case OSArgumentType::Integer:
      result = boost::lexical_cast<std::string>(toPrint.toInt());
      break;
    case OSArgumentType::Boolean:
    case OSArgumentType::String:
    case OSArgumentType::Choice:
    case OSArgumentType::Path:
      result = toPrint.toString().toStdString();
      break;
    default:
      BOOST_ASSERT(false);
  }

  return result;
}

void OSArgument::onChange() {
  m_versionUUID = createUUID();
}

std::ostream& operator<<(std::ostream& os, const OSArgument& arg) {
  os << arg.print();
  return os;
}

OSArgument makeChoiceArgumentOfWorkspaceObjects(const std::string& name,
                                                const IddObjectType& iddObjectType,
                                                const Workspace& workspace,
                                                bool required)
{
  std::vector< std::pair<std::string, std::string> > intermediate;

  std::vector<WorkspaceObject> objects = workspace.getObjectsByType(iddObjectType);
  BOOST_FOREACH(const WorkspaceObject& object, objects){
    std::string objectName;
    if (object.name()) {
      objectName = object.name().get();
    }
    else {
      objectName = object.iddObject().type().valueName();
    }
    intermediate.push_back(std::pair<std::string,std::string>(toString(object.handle()),
                                                              objectName));
  }

  std::sort(intermediate.begin(),
            intermediate.end(),
            SecondOfPairLess< std::pair<std::string,std::string> >());

  int n = intermediate.size();
  StringVector choices(n), displayNames(n);
  for (int i = 0; i < n; ++i) {
    choices[i] = intermediate[i].first;
    displayNames[i] = intermediate[i].second;
  }

  return OSArgument::makeChoiceArgument(name,
                                        choices,
                                        displayNames,
                                        required);
}

OSArgument makeChoiceArgumentOfWorkspaceObjects(const std::string& name,
                                                const std::string& referenceName,
                                                const Workspace& workspace,
                                                bool required)
{
  std::vector< std::pair<std::string, std::string> > intermediate;

  std::vector<WorkspaceObject> objects = workspace.getObjectsByReference(referenceName);
  BOOST_FOREACH(const WorkspaceObject& object, objects){
    intermediate.push_back(std::pair<std::string,std::string>(toString(object.handle()),
                                                              object.name().get()));
  }

  std::sort(intermediate.begin(),
            intermediate.end(),
            SecondOfPairLess< std::pair<std::string,std::string> >());

  int n = intermediate.size();
  StringVector choices(n), displayNames(n);
  for (int i = 0; i < n; ++i) {
    choices[i] = intermediate[i].first;
    displayNames[i] = intermediate[i].second;
  }

  return OSArgument::makeChoiceArgument(name,
                                        choices,
                                        displayNames,
                                        required);
}

std::map<std::string,OSArgument> convertOSArgumentVectorToMap(const std::vector<OSArgument>& arguments)
{
  std::map<std::string, OSArgument> argMap;
  BOOST_FOREACH(const OSArgument& arg,arguments) {
    argMap[arg.name()] = arg.clone();
  }
  return argMap;
}

} // ruleset
} // openstudio
