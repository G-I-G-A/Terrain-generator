#ifndef MODEL_H
#define MODEL_H

#include "Signal.h"

#include <iostream>

class IModel
{
public:
	virtual ~IModel() = default;

	Signal<std::string> signal;
};

class Model : public IModel
{
public:
	Model()
	{
		std::cout << "Model()" << std::endl;
	}

	void databaseUpdate()
	{
		signal.notify("Database updated");
	}
};

#endif // MODEL_H

