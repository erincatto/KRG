#pragma once

#include "Applications/Reflector/Database/ReflectionDatabase.h"
#include <sstream>

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;
using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG::CPP::TypeGenerator
{
    void Generate( ReflectionDatabase const& database, std::stringstream& codeFile, String const& exportMacro, ReflectedType const& type, TVector<ReflectedType> const& parentDescs );
}