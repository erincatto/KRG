#pragma once
#include "Applications/Reflector/Database/ReflectionDatabase.h"
#include <sstream>

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem::Reflection;

//-------------------------------------------------------------------------

namespace KRG::CPP::EnumGenerator
{
    void Generate( std::stringstream& codeFile, String const& exportMacro, ReflectedType const& type );
}