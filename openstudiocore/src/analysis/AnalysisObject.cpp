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

#include <analysis/AnalysisObject.hpp>
#include <analysis/AnalysisObject_Impl.hpp>

namespace openstudio {
namespace analysis {

namespace detail {

  AnalysisObject_Impl::AnalysisObject_Impl(const std::string& name)
    : m_uuid(createUUID()),
      m_versionUUID(createUUID()),
      m_name(name),
      m_displayName(name),
      m_dirty(true)
  {}

  AnalysisObject_Impl::AnalysisObject_Impl(const UUID& uuid,
                                           const UUID& versionUUID,
                                           const std::string& name,
                                           const std::string& displayName,
                                           const std::string& description)
    : m_uuid(uuid),
      m_versionUUID(versionUUID),
      m_name(name),
      m_displayName(displayName),
      m_description(description),
      m_dirty(false)
  {}

  AnalysisObject_Impl::AnalysisObject_Impl(const AnalysisObject_Impl &other)
    : m_uuid(createUUID()),
      m_versionUUID(createUUID()),
      m_name(other.name()),
      m_displayName(other.displayName()),
      m_description(other.description()),
      m_dirty(true)
  {}

  std::string AnalysisObject_Impl::name() const {
    return m_name;
  }

  std::string AnalysisObject_Impl::displayName() const {
    return m_displayName;
  }

  std::string AnalysisObject_Impl::description() const {
    return m_description;
  }

  openstudio::UUID AnalysisObject_Impl::uuid() const {
    return m_uuid;
  }

  openstudio::UUID AnalysisObject_Impl::versionUUID() const {
    return m_versionUUID;
  }

  bool AnalysisObject_Impl::isDirty() const {
    return m_dirty;
  }

  bool AnalysisObject_Impl::uuidEqual(const AnalysisObject& other) const {
    return (uuid() == other.uuid());
  }

  bool AnalysisObject_Impl::uuidAndVersionEqual(const AnalysisObject& other) const {
    return ((uuid() == other.uuid()) && (versionUUID() == other.versionUUID()));
  }

  void AnalysisObject_Impl::setName(const std::string& newName) {
    m_name = newName;
    onChange(AnalysisObject_Impl::Benign);
  }

  void AnalysisObject_Impl::setDisplayName(const std::string& newDisplayName) {
    m_displayName = newDisplayName;
    onChange(AnalysisObject_Impl::Benign);
  }

  void AnalysisObject_Impl::setDescription(const std::string& newDescription) {
    m_description = newDescription;
    onChange(AnalysisObject_Impl::Benign);
  }

  bool AnalysisObject_Impl::clearDirtyFlag() {
    m_dirty = false;
    emit clean();
    return true;
  }

  bool AnalysisObject_Impl::connect(const char* signal,
                                    const QObject* receiver,
                                    const char* slot,
                                    Qt::ConnectionType type) const
  {
    return QObject::connect(this, signal, receiver, slot, type);
  }

  bool AnalysisObject_Impl::disconnect(const char* signal,
                                       const QObject* receiver,
                                       const char* slot) const
  {
    return QObject::disconnect(this,signal,receiver,slot);
  }

  void AnalysisObject_Impl::onChildChanged(ChangeType changeType) {
    onChange(changeType);
  }

  void AnalysisObject_Impl::onParentClean() {
    clearDirtyFlag();
  }

  void AnalysisObject_Impl::onChange(ChangeType changeType) {
    m_versionUUID = createUUID();
    m_dirty = true;
    emit changed(changeType);
  }

  void AnalysisObject_Impl::connectChild(AnalysisObject& child, bool setParent) const {
    if (setParent) {
      AnalysisObject copyOfThis = getPublicObject<AnalysisObject>();
      child.setParent(copyOfThis);
    }

    bool connected = connect(SIGNAL(clean()),
                             child.getImpl<detail::AnalysisObject_Impl>().get(),
                             SLOT(onParentClean()));
    BOOST_ASSERT(connected);
    connected = child.connect(SIGNAL(changed(ChangeType)),
                              this,
                              SLOT(onChildChanged(ChangeType)));
    BOOST_ASSERT(connected);
  }

  void AnalysisObject_Impl::disconnectChild(AnalysisObject& child) const {
    child.clearParent();

    bool disconnected = disconnect(SIGNAL(clean()),
                                   child.getImpl<detail::AnalysisObject_Impl>().get(),
                                   SLOT(onParentClean()));
    BOOST_ASSERT(disconnected);
    disconnected = child.disconnect(SIGNAL(changed(ChangeType)),
                                    this,
                                    SLOT(onChildChanged(ChangeType)));
    BOOST_ASSERT(disconnected);
  }

  void AnalysisObject_Impl::setDirtyFlag() {
    m_dirty = true;
  }

  boost::optional<AnalysisObject> AnalysisObject_Impl::parent() const {
    return m_parent;
  }

  void AnalysisObject_Impl::setParent(const AnalysisObject& parent) const {
    m_parent = parent;
  }

  void AnalysisObject_Impl::clearParent() const {
    m_parent.reset();
  }

} // detail

AnalysisObject AnalysisObject::clone() const {
  return m_impl->clone();
}

std::string AnalysisObject::name() const {
  return getImpl<detail::AnalysisObject_Impl>()->name();
}

std::string AnalysisObject::displayName() const {
  return getImpl<detail::AnalysisObject_Impl>()->displayName();
}

std::string AnalysisObject::description() const {
  return getImpl<detail::AnalysisObject_Impl>()->description();
}

openstudio::UUID AnalysisObject::uuid() const {
  return getImpl<detail::AnalysisObject_Impl>()->uuid();
}

openstudio::UUID AnalysisObject::versionUUID() const {
  return getImpl<detail::AnalysisObject_Impl>()->versionUUID();
}

bool AnalysisObject::isDirty() const {
  return getImpl<detail::AnalysisObject_Impl>()->isDirty();
}

bool AnalysisObject::operator==(const AnalysisObject& other) const {
  return m_impl == other.m_impl;
}

bool AnalysisObject::operator!=(const AnalysisObject& other) const {
  return !(operator==(other));
}

bool AnalysisObject::uuidEqual(const AnalysisObject& other) const {
  return getImpl<detail::AnalysisObject_Impl>()->uuidEqual(other);
}

bool AnalysisObject::uuidAndVersionEqual(const AnalysisObject& other) const {
  return getImpl<detail::AnalysisObject_Impl>()->uuidAndVersionEqual(other);
}

void AnalysisObject::setName(const std::string& newName) {
  getImpl<detail::AnalysisObject_Impl>()->setName(newName);
}

void AnalysisObject::setDisplayName(const std::string& newDisplayName) {
  getImpl<detail::AnalysisObject_Impl>()->setDisplayName(newDisplayName);
}

void AnalysisObject::setDescription(const std::string& newDescription) {
  getImpl<detail::AnalysisObject_Impl>()->setDescription(newDescription);
}

bool AnalysisObject::clearDirtyFlag() {
  return getImpl<detail::AnalysisObject_Impl>()->clearDirtyFlag();
}

/** Connect signal from this AnalysisObject to slot on qObject. */
bool AnalysisObject::connect(const char* signal,
                             const QObject* qObject,
                             const char* slot,
                             Qt::ConnectionType type) const
{
  return getImpl<detail::AnalysisObject_Impl>()->connect(signal,qObject,slot,type);
}

bool AnalysisObject::disconnect(const char* signal,
                                const QObject* receiver,
                                const char* slot) const
{
  return getImpl<detail::AnalysisObject_Impl>()->disconnect(signal,receiver,slot);
}

/// @cond
AnalysisObject::AnalysisObject(boost::shared_ptr<detail::AnalysisObject_Impl> impl)
  : m_impl(impl)
{}

boost::optional<AnalysisObject> AnalysisObject::parent() const {
  return getImpl<detail::AnalysisObject_Impl>()->parent();
}

void AnalysisObject::setParent(const AnalysisObject& parent) const {
  getImpl<detail::AnalysisObject_Impl>()->setParent(parent);
}

void AnalysisObject::clearParent() const {
  getImpl<detail::AnalysisObject_Impl>()->clearParent();
}

void AnalysisObject::onChange() {
  // Generally used by a parent that will make their own call to onChange with
  // the proper ChangeType.
  getImpl<detail::AnalysisObject_Impl>()->onChange(detail::AnalysisObject_Impl::Benign);
}

/// @endcond

} // analysis
} // openstudio

