#pragma once
#include <string>
#include <map>

void StartTools();
bool CheckSHA1();
int SendDBUpdate(std::string params_str);
std::string BuildParamsString(std::map<std::string, std::string> params);
void StopRunning();