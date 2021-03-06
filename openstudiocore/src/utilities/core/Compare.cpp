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

#include <utilities/core/Compare.hpp>
#include <utilities/core/Logger.hpp>
#include <utilities/idf/WorkspaceObject.hpp>
#include <utilities/bcl/BCLComponent.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

namespace openstudio {

bool IstringPairCompare::operator()(const std::pair<std::string, std::string>& x,
                                    const std::pair<std::string, std::string>& y) const
{
  bool result(false);

  if (boost::iequals(x.first, y.first)) {
    result = boost::lexicographical_compare(x.second, y.second, boost::is_iless());
  }
  else {
    result = boost::lexicographical_compare(x.first, y.first, boost::is_iless());
  };

  return result;
}

VersionString::VersionString(const std::string& version)
  : m_str(version)
{
  boost::regex versionRegex("(\\d+)\\.(\\d+)(?:\\.(\\d+))?(?:\\.(\\d+))?");
  boost::smatch m;
  if (boost::regex_match(version,m,versionRegex)) {
    m_major = boost::lexical_cast<int>(std::string(m[1].first,m[1].second));
    m_minor = boost::lexical_cast<int>(std::string(m[2].first,m[2].second));
    int n = m.size();
    if (3 < n) {
      std::string temp(m[3].first,m[3].second);
      if (!temp.empty()) {
        m_patch = boost::lexical_cast<int>(temp);
      }
    }
    if (4 < n) {
      std::string temp(m[4].first,m[4].second);
      if (!temp.empty()) {
        m_build = boost::lexical_cast<int>(temp);
      }
    }
  }
  else {
    LOG_FREE_AND_THROW("openstudio.utilities.VersionString",
                       "Could not parse '" << version << "' as a version string.");
  }
}

std::string VersionString::str() const {
  return m_str;
}

int VersionString::major() const {
  return m_major;
}

int VersionString::minor() const {
  return m_minor;
}

boost::optional<int> VersionString::patch() const {
  return m_patch;
}

boost::optional<int> VersionString::build() const {
  return m_build;
}

bool VersionString::operator<(const VersionString& other) const {

  int meCurrent = major();
  int otherCurrent = other.major();

  if (meCurrent < otherCurrent) {
    return true;
  }
  else if (otherCurrent < meCurrent) {
    return false;
  }

  BOOST_ASSERT(meCurrent == otherCurrent);

  meCurrent = minor();
  otherCurrent = other.minor();

  if (meCurrent < otherCurrent) {
    return true;
  }
  else if (otherCurrent < meCurrent) {
    return false;
  }

  BOOST_ASSERT(meCurrent == otherCurrent);

  if (patch() && other.patch()) {
    meCurrent = patch().get();
    otherCurrent = other.patch().get();

    if (meCurrent < otherCurrent) {
      return true;
    }
    else if (otherCurrent < meCurrent) {
      return false;
    }

    BOOST_ASSERT(meCurrent == otherCurrent);

    if (build() && other.build()) {

      meCurrent = build().get();
      otherCurrent = other.build().get();

      if (meCurrent < otherCurrent) {
        return true;
      }
      else if (otherCurrent < meCurrent) {
        return false;
      }

      BOOST_ASSERT(meCurrent == otherCurrent);
    }
  }

  return false;
}

bool VersionString::operator>(const VersionString& other) const {
  return other < *this;
}

bool VersionString::operator==(const VersionString& other) const {
  return (!(*this < other)) && (!(other < *this));
}

bool VersionString::operator!=(const VersionString& other) const {
  return !(*this == other);
}

bool VersionString::operator<=(const VersionString& other) const {
  return (!(other < *this));
}

bool VersionString::operator>=(const VersionString& other) const {
  return (!(*this < other));
}

bool VersionString::fidelityEqual(const VersionString& other) const {
  if (patch()) {
    if (!other.patch()) {
      return false;
    }
    if (build()) {
      if (!other.build()) {
        return false;
      }
    }
    else if (other.build()) {
      return false;
    }
  }
  else if (other.patch()) {
    return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os,const VersionString& version) {
  os << version.str();
  return os;
}

bool WorkspaceObjectNameLess::operator()(const WorkspaceObject& a, const WorkspaceObject& b) const
{
  std::string aname;
  boost::optional<std::string> oaname = a.name();
  if (oaname){
    aname = *oaname;
  }
  std::string bname;
  boost::optional<std::string> obname = b.name();
  if (obname){
    bname = *obname;
  }
  return istringLess(aname, bname);
}

bool WorkspaceObjectNameGreater::operator()(const WorkspaceObject& a, const WorkspaceObject& b) const
{
  std::string aname;
  boost::optional<std::string> oaname = a.name();
  if (oaname){
    aname = *oaname;
  }
  std::string bname;
  boost::optional<std::string> obname = b.name();
  if (obname){
    bname = *obname;
  }
  return istringLess(bname, aname);
}

bool BCLComponentNameLess::operator()(const BCLComponent& a, const BCLComponent& b) const
{
  std::string aname;
  boost::optional<std::string> oaname = a.name();
  if (oaname){
    aname = *oaname;
  }
  std::string bname;
  boost::optional<std::string> obname = b.name();
  if (obname){
    bname = *obname;
  }
  return istringLess(aname, bname);
}

bool BCLComponentNameGreater::operator()(const BCLComponent& a, const BCLComponent& b) const
{
  std::string aname;
  boost::optional<std::string> oaname = a.name();
  if (oaname){
    aname = *oaname;
  }
  std::string bname;
  boost::optional<std::string> obname = b.name();
  if (obname){
    bname = *obname;
  }
  return istringLess(bname, aname);
}


} // openstudio
