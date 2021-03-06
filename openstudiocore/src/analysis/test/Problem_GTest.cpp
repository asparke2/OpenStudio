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

#include <gtest/gtest.h>
#include <analysis/test/AnalysisFixture.hpp>

#include <analysis/Problem.hpp>
#include <analysis/Variable.hpp>
#include <analysis/Function.hpp>
#include <analysis/DiscreteVariable.hpp>
#include <analysis/DiscreteVariable_Impl.hpp>
#include <analysis/DiscretePerturbation.hpp>
#include <analysis/NullPerturbation.hpp>
#include <analysis/RubyPerturbation.hpp>
#include <analysis/RubyPerturbation_Impl.hpp>
#include <analysis/WorkflowStep.hpp>

#include <runmanager/lib/Workflow.hpp>
#include <runmanager/lib/WorkItem.hpp>

#include <ruleset/OSArgument.hpp>

#include <utilities/bcl/BCLMeasure.hpp>
#include <utilities/core/Containers.hpp>

#include <OpenStudio.hxx>
#include <resources.hxx>

using namespace openstudio;
using namespace openstudio::ruleset;
using namespace openstudio::analysis;

TEST_F(AnalysisFixture, Problem_Constructors) {
  VariableVector variables;
  DiscretePerturbationVector perturbations;
  runmanager::Workflow workflow;

  // almost-default constructor
  Problem problem("Problem",variables,workflow);
  EXPECT_EQ(0,problem.numVariables());
  OptionalInt combinatorialSize = problem.combinatorialSize(true);
  ASSERT_TRUE(combinatorialSize);
  EXPECT_EQ(0,*combinatorialSize);

  // variables with consistent file types
  variables.clear();
  perturbations.clear();
  perturbations.push_back(NullPerturbation());
  openstudio::path rubyScriptPath = toPath(rubyLibDir()) /
                                    toPath("openstudio/runmanager/rubyscripts/PerturbObject.rb");
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::OSM,
                                           FileReferenceType::OSM));
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::OSM,
                                           FileReferenceType::OSM));
  variables.push_back(DiscreteVariable("Variable 1",perturbations));
  perturbations.clear();
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::OSM,
                                           FileReferenceType::IDF));
  variables.push_back(DiscreteVariable("Variable 2",perturbations));
  perturbations.clear();
  perturbations.push_back(NullPerturbation());
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::IDF,
                                           FileReferenceType::IDF));
  variables.push_back(DiscreteVariable("Variable 3",perturbations));
  problem = Problem("Problem",variables,workflow);
  EXPECT_EQ(3,problem.numVariables());
  EXPECT_EQ(6,problem.combinatorialSize(true).get());

  // variables with inconistent file types
  variables.clear();
  perturbations.clear();
  perturbations.push_back(NullPerturbation());
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::OSM,
                                           FileReferenceType::OSM));
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::OSM,
                                           FileReferenceType::OSM));
  variables.push_back(DiscreteVariable("Variable 1",perturbations));
  perturbations.clear();
  perturbations.push_back(NullPerturbation());
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::IDF,
                                           FileReferenceType::IDF));
  variables.push_back(DiscreteVariable("Variable 2",perturbations));
  EXPECT_THROW(Problem("Problem",variables,workflow),std::exception);

  // variables and non-null workflow with consistent file types
  variables.clear();
  perturbations.clear();
  perturbations.push_back(NullPerturbation());
  perturbations.push_back(RubyPerturbation(rubyScriptPath,
                                           FileReferenceType::IDF,
                                           FileReferenceType::IDF));
  variables.push_back(DiscreteVariable("Variable 1",perturbations));
  workflow = runmanager::Workflow();
  workflow.addJob(openstudio::runmanager::JobType::EnergyPlus);
  problem = Problem("Problem",variables,workflow);
  problem.setDisplayName("Display Name");
  problem.setDescription("long winded description");
  EXPECT_EQ(1,problem.numVariables());
  EXPECT_EQ(2,problem.combinatorialSize(true).get());

  // deserialization
  Problem problemCopy(problem.uuid(),
                      problem.versionUUID(),
                      problem.name(),
                      problem.displayName(),
                      problem.description(),
                      problem.workflow(),
                      problem.responses());
  EXPECT_FALSE(problem == problemCopy); // different impls
  EXPECT_TRUE(problem.uuid() == problemCopy.uuid());
  EXPECT_TRUE(problem.versionUUID() == problemCopy.versionUUID());
  EXPECT_EQ(problem.name(),problemCopy.name());
  EXPECT_EQ(problem.displayName(),problemCopy.displayName());
  EXPECT_EQ(problem.description(),problemCopy.description());
  EXPECT_TRUE(problem.workflow() == problemCopy.workflow());

  // variables and non-null workflow with inconsistent file types
  workflow = runmanager::Workflow();
  workflow.addJob(openstudio::runmanager::JobType::ModelToIdf);
  workflow.addJob(openstudio::runmanager::JobType::EnergyPlus);
  EXPECT_THROW(Problem("Problem",variables,workflow),std::exception);
}

TEST_F(AnalysisFixture, Problem_FileTypesAreCompatible_VariableNotInProblem) {
  // fileTypesAreCompatible only works with variables that are in the problem
  DiscreteVariable notInProblem("Not in Problem",DiscretePerturbationVector());

  Problem problem("My Problem, Not Yours",VariableVector(),runmanager::Workflow());
  EXPECT_FALSE(problem.fileTypesAreCompatible(notInProblem,
                                              FileReferenceType(FileReferenceType::IDF),
                                              FileReferenceType(FileReferenceType::IDF)));
  EXPECT_FALSE(problem.fileTypesAreCompatible(notInProblem,
                                              FileReferenceType(FileReferenceType::OSM),
                                              FileReferenceType(FileReferenceType::OSM)));
  EXPECT_FALSE(problem.fileTypesAreCompatible(notInProblem,
                                              FileReferenceType(FileReferenceType::OSM),
                                              boost::none));

  problem.push(DiscreteVariable("Variable 1",DiscretePerturbationVector(1u,NullPerturbation())));
  problem.push(DiscreteVariable("Variable 2",DiscretePerturbationVector(1u,NullPerturbation())));
  problem.push(DiscreteVariable("Variable 3",DiscretePerturbationVector(1u,NullPerturbation())));
  problem.push(runmanager::WorkItem(runmanager::JobType::ModelToIdf));
  problem.push(runmanager::WorkItem(runmanager::JobType::EnergyPlus));
  EXPECT_EQ(3u,problem.variables().size());

  EXPECT_FALSE(problem.fileTypesAreCompatible(notInProblem,
                                              FileReferenceType(FileReferenceType::IDF),
                                              FileReferenceType(FileReferenceType::IDF)));
  EXPECT_FALSE(problem.fileTypesAreCompatible(notInProblem,
                                              FileReferenceType(FileReferenceType::OSM),
                                              FileReferenceType(FileReferenceType::OSM)));
  EXPECT_FALSE(problem.fileTypesAreCompatible(notInProblem,
                                              FileReferenceType(FileReferenceType::OSM),
                                              boost::none));
}

TEST_F(AnalysisFixture, Problem_FileTypesAreCompatible_NewDiscreteVariable) {
  // ok to insert null-only discrete variable anywhere in the chain
  // (between null-model, model-model, idf-idf, idf-workflow)
  VariableVector variables;
  // 0
  variables.push_back(
        DiscreteVariable("Null Variable 1",
                         DiscretePerturbationVector(1u,NullPerturbation())));
  // 1
  variables.push_back(
        DiscreteVariable("Model Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("modelUserScript.rb"),
                                                                        FileReferenceType::OSM,
                                                                        FileReferenceType::OSM,
                                                                        true))));
  // 2
  variables.push_back(
        DiscreteVariable("Translation Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("translationUserScript.rb"),
                                                                        FileReferenceType::OSM,
                                                                        FileReferenceType::IDF,
                                                                        true))));
  // 3
  variables.push_back(
        DiscreteVariable("Workspace Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("workspaceUserScript.rb"),
                                                                        FileReferenceType::IDF,
                                                                        FileReferenceType::IDF,
                                                                        true))));
  runmanager::Workflow workflow;
  workflow.addJob(runmanager::JobType::EnergyPlus);
  Problem problem("Problem 1",variables,workflow);
  DiscreteVariable newVar("New Discrete Variable",
                          DiscretePerturbationVector(1u,NullPerturbation()));
  EXPECT_TRUE(problem.insert(4,newVar.clone().cast<InputVariable>()));
  ASSERT_EQ(5,problem.numVariables());
  EXPECT_EQ("New Discrete Variable",problem.variables()[4].name());
  EXPECT_TRUE(problem.insert(3,newVar.clone().cast<InputVariable>()));
  EXPECT_EQ(6,problem.numVariables());
  ASSERT_GE(problem.numVariables(),5);
  EXPECT_EQ("Translation Variable 1",problem.variables()[2].name());
  EXPECT_EQ("New Discrete Variable",problem.variables()[3].name());
  EXPECT_EQ("Workspace Variable 1",problem.variables()[4].name());
  EXPECT_TRUE(problem.insert(2,newVar.clone().cast<InputVariable>()));
  EXPECT_TRUE(problem.insert(1,newVar.clone().cast<InputVariable>()));
  EXPECT_TRUE(problem.insert(0,newVar.clone().cast<InputVariable>()));
  EXPECT_EQ(9,problem.numVariables());
  variables = castVector<Variable>(problem.variables());
  for (int i = 0, n = problem.numVariables(); i < n; ++i) {
    if (i % 2 == 0) {
      EXPECT_EQ("New Discrete Variable",variables[i].name());
    }
    else {
      EXPECT_NE("New Discrete Variable",variables[i].name());
    }
  }

  // (between null-idf, idf-idf, idf-workflow)
  variables.clear();
  // 0
  variables.push_back(
        DiscreteVariable("Null Variable 1",
                         DiscretePerturbationVector(1u,NullPerturbation())));
  // 1
  variables.push_back(
        DiscreteVariable("Workspace Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("workspaceUserScript.rb"),
                                                                        FileReferenceType::IDF,
                                                                        FileReferenceType::IDF,
                                                                        true))));
  // 2
  variables.push_back(
        DiscreteVariable("Workspace Variable 2",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("workspaceUserScript.rb"),
                                                                        FileReferenceType::IDF,
                                                                        FileReferenceType::IDF,
                                                                        true))));
  problem = Problem("Problem 2",variables,workflow);
  EXPECT_TRUE(problem.insert(3,newVar.clone().cast<InputVariable>()));
  EXPECT_TRUE(problem.insert(2,newVar.clone().cast<InputVariable>()));
  EXPECT_TRUE(problem.insert(1,newVar.clone().cast<InputVariable>()));
  EXPECT_TRUE(problem.insert(0,newVar.clone().cast<InputVariable>()));
  EXPECT_EQ(7,problem.numVariables());
  variables = castVector<Variable>(problem.variables());
  for (int i = 0, n = problem.numVariables(); i < n; ++i) {
    if (i % 2 == 0) {
      EXPECT_EQ("New Discrete Variable",variables[i].name());
    }
    else {
      EXPECT_NE("New Discrete Variable",variables[i].name());
    }
  }

  // expected behavior for then adding first perturbation
  // test with BCLMeasure first. verify with RubyPerturbation.
  openstudio::path dir = resourcesPath() / toPath("/utilities/BCL/Measures/SetWindowToWallRatioByFacade");
  BCLMeasure measure = BCLMeasure::load(dir).get();
  EXPECT_EQ(FileReferenceType(FileReferenceType::OSM),measure.inputFileType());
  EXPECT_EQ(FileReferenceType(FileReferenceType::OSM),measure.outputFileType());
  RubyPerturbation perturbation(measure);

  // have idf-only problem with two null variables at the top
  // try to add perturbations to first variable
  DiscreteVariable firstVariable = problem.variables()[0].cast<DiscreteVariable>();
  ASSERT_TRUE(firstVariable.parent());
  WorkflowStep step0 = problem.workflow()[0];
  EXPECT_TRUE(firstVariable.parent().get() == step0);
  EXPECT_EQ(1u,firstVariable.numPerturbations(false));
  EXPECT_FALSE(problem.fileTypesAreCompatible(firstVariable,
                                              measure.inputFileType(),
                                              measure.outputFileType()));
  ASSERT_TRUE(firstVariable.parent());
  EXPECT_TRUE(firstVariable.parent().get() == problem.workflow()[0]);
  EXPECT_TRUE(problem.workflow()[0] == step0);
  EXPECT_FALSE(firstVariable.push(perturbation));
  EXPECT_EQ(1u,firstVariable.numPerturbations(false));

  // make second variable a translation variable
  DiscreteVariable secondVariable = problem.variables()[1].cast<DiscreteVariable>();
  EXPECT_FALSE(secondVariable.push(RubyPerturbation(toPath("myTranslationScript.rb"),
                                                    FileReferenceType::OSM,
                                                    FileReferenceType::IDF)));
  secondVariable.erase(secondVariable.perturbations(false)[0]);
  EXPECT_TRUE(secondVariable.push(RubyPerturbation(toPath("myTranslationScript.rb"),
                                                   FileReferenceType::OSM,
                                                   FileReferenceType::IDF)));

  // now should be able to add OSM measure
  EXPECT_TRUE(problem.fileTypesAreCompatible(firstVariable,
                                             measure.inputFileType(),
                                             measure.outputFileType()));
  EXPECT_TRUE(firstVariable.push(perturbation));
  EXPECT_EQ(2u,firstVariable.numPerturbations(false));
}

TEST_F(AnalysisFixture, Problem_FileTypesAreCompatible_ExistingDiscreteVariable) {
  // expected behavior when discrete variable already contains perturbations
  // test with BCLMeasure first. verify with RubyPerturbation.

  // create problem
  VariableVector variables;
  variables.push_back(
        DiscreteVariable("Model Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("modelUserScript.rb"),
                                                                        FileReferenceType::OSM,
                                                                        FileReferenceType::OSM,
                                                                        true))));
  variables.back().cast<DiscreteVariable>().insert(0,NullPerturbation());
  // 2
  variables.push_back(
        DiscreteVariable("Translation Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("translationUserScript.rb"),
                                                                        FileReferenceType::OSM,
                                                                        FileReferenceType::IDF,
                                                                        true))));
  // 3
  variables.push_back(
        DiscreteVariable("Workspace Variable 1",
                         DiscretePerturbationVector(1u,RubyPerturbation(toPath("workspaceUserScript.rb"),
                                                                        FileReferenceType::IDF,
                                                                        FileReferenceType::IDF,
                                                                        true))));
  variables.back().cast<DiscreteVariable>().insert(0,NullPerturbation());
  runmanager::Workflow workflow;
  workflow.addJob(runmanager::JobType::EnergyPlus);
  Problem problem("Problem",variables,workflow);

  // create perturbation to be pushed onto a variable
  openstudio::path dir = resourcesPath() / toPath("/utilities/BCL/Measures/SetWindowToWallRatioByFacade");
  BCLMeasure measure = BCLMeasure::load(dir).get();
  EXPECT_EQ(FileReferenceType(FileReferenceType::OSM),measure.inputFileType());
  EXPECT_EQ(FileReferenceType(FileReferenceType::OSM),measure.outputFileType());
  RubyPerturbation perturbation(measure);

  ASSERT_EQ(3,problem.numVariables());

  // can be added to variable 0 (OSM)
  WorkflowStep step = problem.workflow()[0];
  InputVariable var = step.inputVariable();
  EXPECT_EQ(2u,var.cast<DiscreteVariable>().numPerturbations(false));
  EXPECT_TRUE(problem.fileTypesAreCompatible(step,
                                             measure.inputFileType(),
                                             measure.outputFileType()));
  EXPECT_TRUE(var.cast<DiscreteVariable>().push(perturbation));
  EXPECT_EQ(3u,var.cast<DiscreteVariable>().numPerturbations(false));

  // cannot be added to variable 1 (translation)
  step = problem.workflow()[1];
  var = step.inputVariable();
  EXPECT_EQ(1u,var.cast<DiscreteVariable>().numPerturbations(false));
  EXPECT_FALSE(problem.fileTypesAreCompatible(step,
                                              measure.inputFileType(),
                                              measure.outputFileType()));
  EXPECT_FALSE(var.cast<DiscreteVariable>().push(perturbation));
  EXPECT_EQ(1u,var.cast<DiscreteVariable>().numPerturbations(false));

  // cannot be added to variable 2 (IDF)
  step = problem.variables()[2];
  var = step.inputVariable();
  EXPECT_EQ(2u,var.cast<DiscreteVariable>().numPerturbations(false));
  EXPECT_FALSE(problem.fileTypesAreCompatible(step,
                                              measure.inputFileType(),
                                              measure.outputFileType()));
  EXPECT_FALSE(var.cast<DiscreteVariable>().push(perturbation));
  EXPECT_EQ(2u,var.cast<DiscreteVariable>().numPerturbations(false));
}

TEST_F(AnalysisFixture, Problem_UpdateMeasure_DiscreteVariables) {
  // open up example measure
  openstudio::path measuresPath = resourcesPath() / toPath("/utilities/BCL/Measures");
  openstudio::path dir = measuresPath / toPath("SetWindowToWallRatioByFacade");
  BCLMeasure measure1 = BCLMeasure::load(dir).get();
  openstudio::path tempDir1 = measuresPath / toPath(toString(createUUID()));
  openstudio::path tempDir2 = measuresPath / toPath(toString(createUUID()));
  {
    // create multiple BCLMeasures
    BCLMeasure measure1_1 = measure1.clone(tempDir1).get();
    measure1_1.setDescription("Window to wall ratio by wwr and offset.");
    measure1_1.save();
    EXPECT_TRUE(measure1_1.uuid() == measure1.uuid());
    EXPECT_FALSE(measure1_1.versionUUID() == measure1.versionUUID());
    BCLMeasure measure2 = measure1.clone(tempDir2).get();
    measure2.changeUID();
    measure2.incrementVersionId();
    measure2.save();
    EXPECT_FALSE(measure2.uuid() == measure1.uuid());
    EXPECT_FALSE(measure2.versionUUID() == measure1.versionUUID());

    // create args for those measures
    OSArgumentVector args1, args1_1, args2;
    args1.push_back(OSArgument::makeDoubleArgument("wwr"));
    args1.push_back(OSArgument::makeDoubleArgument("sillHeight"));
    args1_1.push_back(OSArgument::makeDoubleArgument("wwr"));
    args1_1.push_back(OSArgument::makeDoubleArgument("offset"));
    args1_1.push_back(OSArgument::makeDoubleArgument("vt"));
    args2.push_back(OSArgument::makeIntegerArgument("numPeople"));

    // create a problem that uses multiple BCLMeasures
    Problem problem("Problem",VariableVector(),runmanager::Workflow());
    DiscreteVariable dv("South WWR",DiscretePerturbationVector(1u,NullPerturbation()));
    problem.push(dv);
    RubyPerturbation rp(measure1);
    rp.setArguments(args1);
    dv.push(rp);
    dv.push(rp.clone().cast<RubyPerturbation>());
    ASSERT_EQ(3u,dv.numPerturbations(false));
    rp = dv.perturbations(false)[2].cast<RubyPerturbation>();
    EXPECT_EQ(2u,rp.arguments().size());
    EXPECT_TRUE(rp.hasIncompleteArguments());
    dv = DiscreteVariable("Occupancy",DiscretePerturbationVector(1u,NullPerturbation()));
    problem.push(dv);
    rp = RubyPerturbation(measure2);
    rp.setArguments(args2);
    dv.push(rp);
    OSArgument arg = args2[0].clone();
    arg.setValue(100);
    rp.setArgument(arg);
    EXPECT_EQ(1u,rp.arguments().size());
    EXPECT_FALSE(rp.hasIncompleteArguments());
    dv = DiscreteVariable("North WWR",DiscretePerturbationVector(1u,NullPerturbation()));
    problem.push(dv);
    rp = RubyPerturbation(measure1);
    rp.setArguments(args1);
    arg = args1[0].clone();
    arg.setValue(0.32);
    rp.setArgument(arg);
    arg = args1[1].clone();
    arg.setValue(1.0);
    rp.setArgument(arg);
    EXPECT_EQ(2u,rp.arguments().size());
    EXPECT_FALSE(rp.hasIncompleteArguments());
    dv.push(rp);
    EXPECT_EQ(2u,dv.numPerturbations(false));

    // call update
    problem.clearDirtyFlag();
    problem.updateMeasure(measure1_1,args1_1,false);

    // check state
    VariableVector vars = castVector<Variable>(problem.variables());
    ASSERT_EQ(3u,vars.size());
    EXPECT_TRUE(vars[0].isDirty());
    EXPECT_FALSE(vars[1].isDirty());
    EXPECT_TRUE(vars[2].isDirty());

    dv = vars[0].cast<DiscreteVariable>();
    ASSERT_EQ(3u,dv.numPerturbations(false));
    DiscretePerturbationVector ps = dv.perturbations(false);
    EXPECT_FALSE(ps[0].isDirty());
    EXPECT_TRUE(ps[1].isDirty());
    rp = ps[1].cast<RubyPerturbation>();
    EXPECT_EQ(3u,rp.arguments().size());
    EXPECT_EQ(3u,rp.incompleteArguments().size());
    EXPECT_TRUE(ps[2].isDirty());
    rp = ps[2].cast<RubyPerturbation>();
    EXPECT_EQ(3u,rp.arguments().size());
    EXPECT_EQ(3u,rp.incompleteArguments().size());

    dv = vars[2].cast<DiscreteVariable>();
    ASSERT_EQ(2u,dv.numPerturbations(false));
    ps = dv.perturbations(false);
    EXPECT_FALSE(ps[0].isDirty());
    EXPECT_TRUE(ps[1].isDirty());
    rp = ps[1].cast<RubyPerturbation>();
    EXPECT_EQ(3u,rp.arguments().size());
    EXPECT_EQ(2u,rp.incompleteArguments().size());
  }
  boost::filesystem::remove_all(tempDir1);
  boost::filesystem::remove_all(tempDir2);
}
