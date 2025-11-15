<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="14.0" InitialTargets="ValidateMsbuilderHasRequiredComponents">

  <PropertyGroup>
    <EngineSourceDirectory>..\..\..\..\Engine\Source</EngineSourceDirectory>
    <EngineDirectory>..\..\..\..\</EngineDirectory>
    <bGenerateProjectFiles>true</bGenerateProjectFiles>
  </PropertyGroup>

  <Import Project="$(EngineDirectory)\Build\BatchFiles\Build.vc.props"/>

</Project>
