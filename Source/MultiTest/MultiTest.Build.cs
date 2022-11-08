// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using System.IO;
using UnrealBuildTool;

public class MultiTest : ModuleRules
{
    public MultiTest(ReadOnlyTargetRules Target) : base(Target)
	{

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","Sockets"});

		PrivateDependencyModuleNames.AddRange(new string[] { });


        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemIncludePaths.Add(ModuleDirectory + "\\..\\ThirdParty\\ProtoBuf");
           
            PublicAdditionalLibraries.Add(ModuleDirectory + "\\..\\ThirdParty\\ProtoBuf\\lib\\Win64\\libprotobuf.lib");

        }

        bEnableUndefinedIdentifierWarnings = false;

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

    }
}
