//
//  StaticAnalyser.cpp
//  Clock Signal
//
//  Created by Thomas Harte on 14/04/2018.
//  Copyright © 2018 Thomas Harte. All rights reserved.
//

#include "StaticAnalyser.hpp"
#include "Target.hpp"

Analyser::Static::TargetList Analyser::Static::AppleII::GetTargets(const Media &media, const std::string &file_name, TargetPlatform::IntType potential_platforms) {
	auto target = std::unique_ptr<Target>(new Target);
	target->machine = Machine::AppleII;
	target->media = media;

	target->has_disk = !target->media.disks.empty();

	TargetList targets;
	targets.push_back(std::move(target));
	return targets;
}