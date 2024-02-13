#pragma once

namespace bv
{

class ReportOutputsNode;
class TargetsNode;
class RuleSelectorNode;
class ProjectSettingsNode;

struct [[nodiscard]] ProjectTreeNodes final
{
    const ReportOutputsNode& reportOutputsNode;
    const TargetsNode& targetsNode;
    const RuleSelectorNode& ruleSelectorNode;
    const ProjectSettingsNode& projectSettingsNode;
};

} //namespace bv
