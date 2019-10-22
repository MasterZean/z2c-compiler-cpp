#ifndef __BUILDER_HPP__
#define __BUILDER_HPP__

#include <Core/Core.h>

#include "BuildMethod.h"

namespace Z2 {
	
using namespace Upp;

class Builder {
public:
	Builder(const BuildMethod& abm): bm(abm) {
	}
	
	void ZCompilerPath(const String& azPath) {
		zPath = azPath;
	}
	
	void TargetRoot(const String& aroot) {
		target = aroot;
	}
	
	void Arch(const String& aarch) {
		arch = aarch;
	}
	
	void Optimize(const String& ao) {
		optimize = ao;
	}
	
	void CPP(bool aCpp) {
		cpp = aCpp;
	}
		
	bool Build(const String& path, const String& origPath);
	
private:
	BuildMethod bm;
	String zPath;
	String target;
	String cppPath;
	String linkPath;
	String arch;
	String env;
	String optimize;
	bool cpp = true;
	
	bool BuildMSC(const String& path, const String& origPath);
	bool BuildGCC(const String& path, const String& origPath);
	
	void DoPathsMSC();
	void DoEnvMSC();
	void DoEnvGCC();
	
	bool CompileMSC(const String& src, const String& out);
	bool CompileGCC(const String& src, const String& out);
};

}

#endif