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

#include <project/RubyContinuousVariableRecord.hpp>
#include <project/RubyContinuousVariableRecord_Impl.hpp>
#include <project/ProjectDatabase.hpp>
#include <project/ProblemRecord.hpp>
#include <project/FunctionRecord.hpp>
#include <project/RubyPerturbationRecord.hpp>
#include <project/OSArgumentRecord.hpp>

#include <analysis/RubyContinuousVariable.hpp>
#include <analysis/RubyContinuousVariable_Impl.hpp>

#include <utilities/core/Assert.hpp>

using namespace openstudio::ruleset;

namespace openstudio {
namespace project {

namespace detail {

  RubyContinuousVariableRecord_Impl::RubyContinuousVariableRecord_Impl(
      const analysis::RubyContinuousVariable& rubyContinuousVariable,
      ProblemRecord& problemRecord,
      int variableVectorIndex)
    : ContinuousVariableRecord_Impl(rubyContinuousVariable,
                                    ContinuousVariableRecordType::RubyContinuousVariableRecord,
                                    problemRecord,
                                    variableVectorIndex)
  {}

  RubyContinuousVariableRecord_Impl::RubyContinuousVariableRecord_Impl(
      const analysis::RubyContinuousVariable& rubyContinuousVariable,
      FunctionRecord& functionRecord,
      int variableVectorIndex,
      boost::optional<double> functionCoefficient)
    : ContinuousVariableRecord_Impl(rubyContinuousVariable,
                                    ContinuousVariableRecordType::RubyContinuousVariableRecord,
                                    functionRecord,
                                    variableVectorIndex,
                                    functionCoefficient)
  {}

  RubyContinuousVariableRecord_Impl::RubyContinuousVariableRecord_Impl(const QSqlQuery& query, ProjectDatabase& database)
    : ContinuousVariableRecord_Impl(query, database)
  {
    BOOST_ASSERT(query.isValid());
    BOOST_ASSERT(query.isActive());
    BOOST_ASSERT(query.isSelect());

    QVariant value;

    value = query.value(RubyContinuousVariableRecord::ColumnsType::rubyPerturbationRecordId);
    BOOST_ASSERT(value.isValid() && !value.isNull());
    m_rubyPerturbationRecordId = value.toInt();
  }

  std::vector<ObjectRecord> RubyContinuousVariableRecord_Impl::children() const {
    ObjectRecordVector result = InputVariableRecord_Impl::children();
    result.push_back(osArgumentRecord());
    return result;
  }

  std::vector<ObjectRecord> RubyContinuousVariableRecord_Impl::resources() const {
    ObjectRecordVector result;
    result.push_back(rubyPerturbationRecord());
    return result;
  }

  void RubyContinuousVariableRecord_Impl::saveRow(const boost::shared_ptr<QSqlDatabase> &database)
  {
    QSqlQuery query(*database);
    this->makeUpdateByIdQuery<RubyContinuousVariableRecord>(query);
    this->bindValues(query);
    assertExec(query);
  }

  OSArgumentRecord RubyContinuousVariableRecord_Impl::osArgumentRecord() const {
    OptionalOSArgumentRecord result;

    ProjectDatabase database = projectDatabase();
    QSqlQuery query(*(database.qSqlDatabase()));
    query.prepare(toQString("SELECT * FROM " + OSArgumentRecord::databaseTableName() +
        " WHERE rubyContinuousVariableRecordId=:rubyContinuousVariableRecordId" ));
    query.bindValue(":rubyContinuousVariableRecordId",id());
    assertExec(query);
    if (query.first()) {
      result = OSArgumentRecord::factoryFromQuery(query, database);
    }
    BOOST_ASSERT(result);

    return *result;
  }

  RubyPerturbationRecord RubyContinuousVariableRecord_Impl::rubyPerturbationRecord() const {
    OptionalRubyPerturbationRecord result;
    ProjectDatabase database = projectDatabase();
    result = RubyPerturbationRecord::getRubyPerturbationRecord(m_rubyPerturbationRecordId,database);
    BOOST_ASSERT(result);
    return *result;
  }

  analysis::Variable RubyContinuousVariableRecord_Impl::variable() const {
    return rubyContinuousVariable().cast<analysis::Variable>();
  }

  analysis::ContinuousVariable RubyContinuousVariableRecord_Impl::continuousVariable() const {
    return rubyContinuousVariable().cast<analysis::ContinuousVariable>();
  }

  analysis::InputVariable RubyContinuousVariableRecord_Impl::inputVariable() const {
    return rubyContinuousVariable().cast<analysis::InputVariable>();
  }

  analysis::RubyContinuousVariable RubyContinuousVariableRecord_Impl::rubyContinuousVariable() const {
    ruleset::OSArgument argument = osArgumentRecord().osArgument();
    analysis::RubyPerturbation perturbation = rubyPerturbationRecord().rubyPerturbation();
    return analysis::RubyContinuousVariable(handle(),
                                            uuidLast(),
                                            name(),
                                            displayName(),
                                            description(),
                                            uncertaintyDescription(),
                                            minimum(),
                                            maximum(),
                                            increment(),
                                            nSteps(),
                                            argument,
                                            perturbation);
  }

  void RubyContinuousVariableRecord_Impl::revertToLastRecordIds() {
    m_rubyPerturbationRecordId = m_lastRubyPerturbationRecordId;
  }

  void RubyContinuousVariableRecord_Impl::setRubyPerturbationRecordId(int id) {
    m_rubyPerturbationRecordId = id;
    this->onChange(false);
  }

  void RubyContinuousVariableRecord_Impl::bindValues(QSqlQuery& query) const {
    ContinuousVariableRecord_Impl::bindValues(query);

    query.bindValue(RubyContinuousVariableRecord::ColumnsType::rubyPerturbationRecordId,m_rubyPerturbationRecordId);
  }

  void RubyContinuousVariableRecord_Impl::setLastValues(const QSqlQuery& query, ProjectDatabase& projectDatabase) {
    BOOST_ASSERT(query.isValid());
    BOOST_ASSERT(query.isActive());
    BOOST_ASSERT(query.isSelect());

    ContinuousVariableRecord_Impl::setLastValues(query,projectDatabase);

    QVariant value;

    value = query.value(RubyContinuousVariableRecord::ColumnsType::rubyPerturbationRecordId);
    BOOST_ASSERT(value.isValid() && !value.isNull());
    m_lastRubyPerturbationRecordId = value.toInt();
  }

  bool RubyContinuousVariableRecord_Impl::compareValues(const QSqlQuery& query) const {
    BOOST_ASSERT(query.isValid());
    BOOST_ASSERT(query.isActive());
    BOOST_ASSERT(query.isSelect());

    bool result = ContinuousVariableRecord_Impl::compareValues(query);

    QVariant value;

    value = query.value(RubyContinuousVariableRecord::ColumnsType::rubyPerturbationRecordId);
    BOOST_ASSERT(value.isValid() && !value.isNull());
    result = result && (m_rubyPerturbationRecordId == value.toInt());

    return result;
  }

  void RubyContinuousVariableRecord_Impl::saveLastValues() {
    ContinuousVariableRecord_Impl::saveLastValues();

    m_lastRubyPerturbationRecordId = m_rubyPerturbationRecordId;
  }

  void RubyContinuousVariableRecord_Impl::revertToLastValues() {
    ContinuousVariableRecord_Impl::revertToLastValues();

    m_rubyPerturbationRecordId = m_lastRubyPerturbationRecordId;
  }

} // detail

RubyContinuousVariableRecord::RubyContinuousVariableRecord(
    const analysis::RubyContinuousVariable& rubyContinuousVariable,
    ProblemRecord& problemRecord,
    int variableVectorIndex)
  : ContinuousVariableRecord(boost::shared_ptr<detail::RubyContinuousVariableRecord_Impl>(
        new detail::RubyContinuousVariableRecord_Impl(rubyContinuousVariable,
                                                      problemRecord,
                                                      variableVectorIndex)),
        problemRecord.projectDatabase(),
        rubyContinuousVariable)
{
  BOOST_ASSERT(getImpl<detail::RubyContinuousVariableRecord_Impl>());

  constructRelatedRecords(rubyContinuousVariable);
}

RubyContinuousVariableRecord::RubyContinuousVariableRecord(
    const analysis::RubyContinuousVariable& rubyContinuousVariable,
    FunctionRecord& functionRecord,
    int variableVectorIndex,
    boost::optional<double> functionCoefficient)
  : ContinuousVariableRecord(boost::shared_ptr<detail::RubyContinuousVariableRecord_Impl>(
        new detail::RubyContinuousVariableRecord_Impl(rubyContinuousVariable,
                                                      functionRecord,
                                                      variableVectorIndex,
                                                      functionCoefficient)),
        functionRecord.projectDatabase(),
        rubyContinuousVariable)
{
  BOOST_ASSERT(getImpl<detail::RubyContinuousVariableRecord_Impl>());

  constructRelatedRecords(rubyContinuousVariable);
}

RubyContinuousVariableRecord::RubyContinuousVariableRecord(const QSqlQuery& query, ProjectDatabase& database)
  : ContinuousVariableRecord(boost::shared_ptr<detail::RubyContinuousVariableRecord_Impl>(
        new detail::RubyContinuousVariableRecord_Impl(query, database)),
        database,
        analysis::OptionalContinuousVariable())
{
  BOOST_ASSERT(getImpl<detail::RubyContinuousVariableRecord_Impl>());
}

RubyContinuousVariableRecord::RubyContinuousVariableRecord(boost::shared_ptr<detail::RubyContinuousVariableRecord_Impl> impl,
                                                           ProjectDatabase database)
  : ContinuousVariableRecord(impl, database, analysis::OptionalContinuousVariable())
{
  BOOST_ASSERT(getImpl<detail::RubyContinuousVariableRecord_Impl>());
}

boost::optional<RubyContinuousVariableRecord> RubyContinuousVariableRecord::factoryFromQuery(const QSqlQuery& query, ProjectDatabase& database)
{
  OptionalRubyContinuousVariableRecord result;

  try {
    result = RubyContinuousVariableRecord(query,database);
  }
  catch (const std::exception& e) {
    LOG(Error,"Unable to construct RubyContinuousVariableRecord from query, because '"
        << e.what() << "'.");
  }

  return result;
}

std::vector<RubyContinuousVariableRecord> RubyContinuousVariableRecord::getRubyContinuousVariableRecords(ProjectDatabase& database) {
  std::vector<RubyContinuousVariableRecord> result;

  QSqlQuery query(*(database.qSqlDatabase()));
  query.prepare(toQString("SELECT * FROM " + VariableRecord::databaseTableName() +
                          " WHERE variableRecordType=:variableRecordType AND " +
                          "inputVariableRecordType=:inputVariableRecordType AND " +
                          "continuousVariableRecordType=:continuousVariableRecordType"));
  query.bindValue(":variableRecordType", VariableRecordType::InputVariableRecord);
  query.bindValue(":inputVariableRecordType", InputVariableRecordType::ContinuousVariableRecord);
  query.bindValue(":continuousVariableRecordType", ContinuousVariableRecordType::RubyContinuousVariableRecord);
  assertExec(query);
  while (query.next()) {
    result.push_back(RubyContinuousVariableRecord(query, database));
  }

  return result;
}

boost::optional<RubyContinuousVariableRecord> RubyContinuousVariableRecord::getRubyContinuousVariableRecord(int id, ProjectDatabase& database) {
  boost::optional<RubyContinuousVariableRecord> result;

  QSqlQuery query(*(database.qSqlDatabase()));
  query.prepare(toQString("SELECT * FROM " + VariableRecord::databaseTableName() +
                          " WHERE variableRecordType=:variableRecordType AND " +
                          "inputVariableRecordType=:inputVariableRecordType AND " +
                          "continuousVariableRecordType=:continuousVariableRecordType AND " +
                          "id=:id"));
  query.bindValue(":variableRecordType", VariableRecordType::InputVariableRecord);
  query.bindValue(":inputVariableRecordType", InputVariableRecordType::ContinuousVariableRecord);
  query.bindValue(":continuousVariableRecordType", ContinuousVariableRecordType::RubyContinuousVariableRecord);
  query.bindValue(":id",id);
  assertExec(query);
  if (query.first()) {
    result = RubyContinuousVariableRecord(query, database);
  }

  return result;
}

OSArgumentRecord RubyContinuousVariableRecord::osArgumentRecord() const {
  return getImpl<detail::RubyContinuousVariableRecord_Impl>()->osArgumentRecord();
}

RubyPerturbationRecord RubyContinuousVariableRecord::rubyPerturbationRecord() const {
  return getImpl<detail::RubyContinuousVariableRecord_Impl>()->rubyPerturbationRecord();
}

analysis::RubyContinuousVariable RubyContinuousVariableRecord::rubyContinuousVariable() const {
  return getImpl<detail::RubyContinuousVariableRecord_Impl>()->rubyContinuousVariable();
}

/// @cond
RubyContinuousVariableRecord::RubyContinuousVariableRecord(boost::shared_ptr<detail::RubyContinuousVariableRecord_Impl> impl)
  : ContinuousVariableRecord(impl)
{}
/// @endcond

void RubyContinuousVariableRecord::constructRelatedRecords(const analysis::Variable& variable)
{
  analysis::RubyContinuousVariable rubyContinuousVariable = variable.cast<analysis::RubyContinuousVariable>();
  RubyContinuousVariableRecord copyOfThis(getImpl<detail::RubyContinuousVariableRecord_Impl>());
  ProjectDatabase database = projectDatabase();
  bool isNew = database.isNewRecord(copyOfThis);
  if (!isNew) {
    getImpl<detail::RubyContinuousVariableRecord_Impl>()->revertToLastRecordIds();
  }

  // Save resource RubyPerturbation
  if (isNew || rubyContinuousVariable.perturbation().isDirty()) {
    RubyPerturbationRecord rubyPerturbationRecord(rubyContinuousVariable.perturbation(),
                                                  database);
    getImpl<detail::RubyContinuousVariableRecord_Impl>()->setRubyPerturbationRecordId(rubyPerturbationRecord.id());
  }

  // Save child OSArgument
  OSArgument argument = rubyContinuousVariable.argument();
  bool saveArgument = false;
  if (!isNew) {
    {
      // see if old argument record should be removed
      OSArgumentRecord oldArgumentRecord = osArgumentRecord();
      if (oldArgumentRecord.handle() != argument.uuid()) {
        database.removeRecord(oldArgumentRecord);
        saveArgument = true;
      }
      else if (oldArgumentRecord.uuidLast() != argument.versionUUID()) {
        saveArgument = true;
      }
    }
    database.unloadUnusedCleanRecords();
  }
  if (saveArgument || isNew) {
    OSArgumentRecord osArgumentRecord(argument,copyOfThis);
  }
}

} // project
} // openstudio

