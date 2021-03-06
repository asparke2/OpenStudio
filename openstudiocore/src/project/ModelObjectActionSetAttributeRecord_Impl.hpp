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

#ifndef PROJECT_MODELOBJECTACTIONSETATTRIBUTERECORD_IMPL_HPP
#define PROJECT_MODELOBJECTACTIONSETATTRIBUTERECORD_IMPL_HPP

#include "ProjectAPI.hpp"
#include <project/ModelObjectActionClauseRecord_Impl.hpp>

#include <utilities/core/Logger.hpp>
#include <utilities/data/Attribute.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional.hpp>

class QSqlDatabase;

namespace openstudio {
namespace ruleset {
  class ModelObjectActionSetAttribute;
}
namespace project {

namespace detail {

  class PROJECT_API ModelObjectActionSetAttributeRecord_Impl : public ModelObjectActionClauseRecord_Impl
  {

    Q_OBJECT;

    public:

      /// constructors
      ModelObjectActionSetAttributeRecord_Impl(const openstudio::ruleset::ModelObjectActionSetAttribute& modelObjectActionSetAttribute, const ProjectDatabase& projectDatabase);

      /// constructor from query, throws if bad query
      ModelObjectActionSetAttributeRecord_Impl(const QSqlQuery& query, const ProjectDatabase& projectDatabase);

      virtual ~ModelObjectActionSetAttributeRecord_Impl() {}

      /// get the filter from the record
      openstudio::ruleset::ModelObjectActionSetAttribute modelObjectActionSetAttribute() const;

      /// get the corresponding action clause
      virtual openstudio::ruleset::ActionClause actionClause() const;

      /// get parent object
      virtual boost::optional<ObjectRecord> parent() const;

      /// get child objects
      virtual std::vector<ObjectRecord> children() const;

      /// get resource objects
      virtual std::vector<ObjectRecord> resources() const;

      /// get join record objects
      virtual std::vector<JoinRecord> joinRecords() const;

      /// save the row for just this object in the database
      /// requires ProjectDatabase as called in ProjectDatabase ctor/dtor
      virtual void saveRow(const boost::shared_ptr<QSqlDatabase> &database);

    protected:

      /// bind values to a query for saving
      /// override in derived classes
      virtual void bindValues(QSqlQuery& query) const;

      /// set the last state of this object from the query (including id)
      /// override in derived classes
      virtual void setLastValues(const QSqlQuery& query, ProjectDatabase& projectDatabase);

      /// check that values (except id) are same as query
      /// override in derived classes
      virtual bool compareValues(const QSqlQuery& query) const;

      /// save values to last state
      /// override in derived classes
      virtual void saveLastValues();

      /// revert values back to last state
      /// override in derived classes
      virtual void revertToLastValues();

    private:

      REGISTER_LOGGER("openstudio.project.ModelObjectActionSetAttributeRecord");

      std::string m_attributeName;
      openstudio::AttributeValueType m_attributeValueType;            
      QVariant m_attributeValue; 

      std::string m_lastAttributeName;
      openstudio::AttributeValueType m_lastAttributeValueType;            
      QVariant m_lastAttributeValue; 

  };

} // detail
} // project
} // openstudio

#endif // PROJECT_MODELOBJECTACTIONSETATTRIBUTERECORD_IMPL_HPP
