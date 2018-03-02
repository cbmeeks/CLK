//
//  StaticAnalyser.hpp
//  Clock Signal
//
//  Created by Thomas Harte on 15/09/2016.
//  Copyright © 2016 Thomas Harte. All rights reserved.
//

#ifndef StaticAnalyser_Atari_StaticAnalyser_hpp
#define StaticAnalyser_Atari_StaticAnalyser_hpp

#include "../StaticAnalyser.hpp"

namespace Analyser {
namespace Static {
namespace Atari {

void AddTargets(const Media &media, std::vector<std::unique_ptr<Target>> &destination);

}
}
}

#endif /* StaticAnalyser_hpp */