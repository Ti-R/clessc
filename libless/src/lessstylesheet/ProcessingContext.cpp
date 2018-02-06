#include "less/lessstylesheet/ProcessingContext.h"
#include "less/lessstylesheet/LessRuleset.h"
#include "less/lessstylesheet/LessStylesheet.h"
#include "less/lessstylesheet/MixinCall.h"

ProcessingContext::ProcessingContext() {
  stack = NULL;
  contextStylesheet = NULL;
}

void ProcessingContext::setLessStylesheet(const LessStylesheet &stylesheet) {
  contextStylesheet = &stylesheet;
}
const LessStylesheet *ProcessingContext::getLessStylesheet() const {
  return contextStylesheet;
}

const TokenList *ProcessingContext::getVariable(const std::string &key) const {
  const TokenList* t;
  
  if (stack != NULL) {
    if ((t = stack->getVariable(key, *this)) != NULL)
      return t;
    return NULL;
  } else
    return getLessStylesheet()->getVariable(key, *this);
}

const TokenList *ProcessingContext::getFunctionVariable
(const std::string &key,
 const Function* function) const {
  
  std::map<const Function*, VariableMap>::const_iterator it;

  if ((it = variables.find(function)) != variables.end())
    return (*it).second.getVariable(key);
  else
    return NULL;
}

const TokenList *ProcessingContext::getBaseVariable
(const std::string &key) const {
  return base_variables.getVariable(key);
}

void ProcessingContext::pushMixinCall(const Function &function,
                                      bool savepoint) {
  stack = new MixinCall(stack, function, savepoint);
}

void ProcessingContext::popMixinCall() {
  if (stack != NULL) {
    stack = stack->parent;
  }
}

VariableMap *ProcessingContext::getStackArguments() const {
  if (stack != NULL)
    return &stack->arguments;
  else
    return NULL;
}
VariableMap *ProcessingContext::getStackArguments(const Function *function) const {
  MixinCall* tmp = stack;
  while (tmp != NULL) {
    if (tmp->function == function)
        return &tmp->arguments;
    tmp = tmp->parent;
  }
  return NULL;
}

bool ProcessingContext::isStackEmpty() const {
  return stack == NULL;
}

bool ProcessingContext::isSavePoint() const {
  return (stack != NULL && stack->savepoint);
}

const Function* ProcessingContext::getSavePoint() const {
  MixinCall *tmp = stack;
  while (tmp != NULL && !tmp->savepoint) 
    tmp = tmp->parent;
  return (tmp == NULL) ? NULL : tmp->function;
}

void ProcessingContext::getFunctions(std::list<const Function *> &functionList,
                                     const Mixin &mixin) const {
  if (stack != NULL)
    stack->getFunctions(functionList, mixin, *this);
  else if (contextStylesheet != NULL)
    contextStylesheet->getFunctions(functionList, mixin, *this);
}

bool ProcessingContext::isInStack(const Function &function) const {
  if (stack != NULL)
    return stack->isInStack(function);
  else
    return NULL;
}

void ProcessingContext::addExtension(Extension &extension) {
  extensions.push_back(extension);
}
std::list<Extension> &ProcessingContext::getExtensions() {
  return extensions;
}

void ProcessingContext::addClosure(const LessRuleset &ruleset) {
  if (stack == NULL)
    return;
  
  const Function* fnc = getSavePoint();
  Closure *c = new Closure(ruleset, *stack);
  
  if (fnc != NULL)
    closures[fnc].push_back(c);
  else
    base_closures.push_back(c);
}

void ProcessingContext::addVariables(const VariableMap &variables) {
  const Function* fnc = getSavePoint();
  if (fnc != NULL)
    this->variables[fnc].overwrite(variables);
  else
    base_variables.overwrite(variables);
}

const std::list<Closure *> *ProcessingContext::getClosures(const Function *function) const {
  std::map<const Function*, std::list<Closure *>>::const_iterator it;

  if ((it = closures.find(function)) != closures.end())
    return &(*it).second;
  else
    return NULL;
}
const std::list<Closure *> *ProcessingContext::getBaseClosures() const {
  return &base_closures;
}

ValueProcessor *ProcessingContext::getValueProcessor() {
  return &processor;
}

void ProcessingContext::interpolate(TokenList &tokens) {
  processor.interpolate(tokens, *this);
}
void ProcessingContext::interpolate(std::string &str) {
  processor.interpolate(str, *this);
}

void ProcessingContext::processValue(TokenList &value) {
  processor.processValue(value, *this);
}

bool ProcessingContext::validateCondition(const TokenList &value, bool defaultVal) const {
  return processor.validateCondition(value, *this, defaultVal);
}
