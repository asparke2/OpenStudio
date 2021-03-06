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

#ifndef PROJECT_MODELOBJECTFILTERNUMERICATTRIBUTERECORD_IMPL_HPP
#define PROJECT_MODELOBJECTFILTERNUMERICATTRIBUTERECORD_IMPL_HPP

#include "ProjectAPI.hpp"
#include <project/ModelObjectFilterAttributeRecord_Impl.hpp>

#include <ruleset/RulesetEnums.hpp>

#include <utilities/core/Logger.hpp>
#include <utilities/data/Attribute.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional.hpp>

class QSqlDatabase;

namespace openstudio {
namespace ruleset {
  class ModelObjectFilterNumericAttribute;
}
namespace project {

namespace detail {

  class PROJECT_API ModelObjectFilterNumericAttributeRecord_Impl : public ModelObjectFilterAttributeRecord_Impl
  {

    Q_OBJECT;

    public:

      /// constructors
      ModelObjectFilterNumericAttributeRecord_Impl(const openstudio::ruleset::ModelObjectFilterNumericAttribute& modelObjectFilterNumericAttribute, const ProjectDatabase& projectDatabase);

      /// constructor from query, throws if bad query
      ModelObjectFilterNumericAttributeRecord_Impl(const QSqlQuery& query, const ProjectDatabase& projectDatabase);

      virtual ~ModelObjectFilterNumericAttributeRecord_Impl() {}

      /// get the filter from the record
      openstudio::ruleset::ModelObjectFilterNumericAttribute modelObjectFilterNumericAttribute() const;

      /// get the corresponding filter clause
      virtual openstudio::ruleset::FilterClause filterClause() const;

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

      REGISTER_LOGGER("openstudio.project.ModelObjectFilterNumericAttributeRecord");

      openstudio::AttributeValueType m_attributeValueType;
      QVariant m_attributeValue;
      openstudio::ruleset::RulesetNumericalPredicate m_predicate;

      openstudio::AttributeValueType m_lastAttributeValueType;
      QVariant m_lastAttributeValue;
      openstudio::ruleset::RulesetNumericalPredicate m_lastPredicate;

  };

} // detail
} // project
} // openstudio

#endif // PROJECT_MODELOBJECTFILTERNUMERICATTRIBUTERECORD_IMPL_HPP
