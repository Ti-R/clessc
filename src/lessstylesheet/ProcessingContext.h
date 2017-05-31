/*
 * Copyright 2012 Bram van der Kroef
 *
 * This file is part of LESS CSS Compiler.
 *
 * LESS CSS Compiler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LESS CSS Compiler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LESS CSS Compiler.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Author: Bram van der Kroef <bram@vanderkroef.net>
 */

#ifndef __ProcessingContext_h__
#define __ProcessingContext_h__

#include <map>
#include <string>
#include <list>

#include "../TokenList.h"
#include "../value/ValueScope.h"
#include "../value/ValueProcessor.h"
#include "../lessstylesheet/Extension.h"
#include "Function.h"
#include "Closure.h"

class LessRuleset;
class Function;
class Closure;
class Mixin;

class ProcessingContext {
private:
  const ValueScope* scopes;
  std::list<const Function*> functionStack;
  ValueProcessor processor;
  std::list<Extension> extensions;
  std::list<std::pair<std::list<Closure*>*, const ValueScope*>> closureStack;
  
public:
  ProcessingContext();
  
  const TokenList* getVariable(const std::string &key);
  void pushScope(const std::map<std::string, TokenList> &scope);
  void popScope();
  
  void pushFunction(const Function &function);
  void popFunction();
  bool isInStack(const Function &function);

  void pushClosureScope(std::list<Closure*>
                        &scope);
  void popClosureScope();
  void addClosure(const LessRuleset &ruleset);
  void getClosures(std::list<const Function*> &closureList,
                   const Mixin &mixin);
  void addExtension(Extension& extension);
  std::list<Extension>& getExtensions();

  ValueProcessor* getValueProcessor();

  void interpolate(TokenList &tokens);
  void interpolate(std::string &str);
  void processValue(TokenList& value);
  bool validateCondition(TokenList &value);
};

#endif
