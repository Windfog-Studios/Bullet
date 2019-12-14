#ifndef _MODULE_UI_H_
#define _MODULE_UI_H_
#include "Module.h"
#include "Timer.h"

class ModuleUI : public Module
{
public:
	ModuleUI() {};
	virtual ~ModuleUI() {};

private:
	bool Start() { return true;};
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() { return true; };

public:
	Timer timer;
	
private:

};

#endif // !_MODULE_UI_H_

