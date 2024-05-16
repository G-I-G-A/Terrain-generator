#ifndef VIEW_H
#define VIEW_H

#include <iostream>

class IView
{
public:
	virtual ~IView() = default;
};

class View : public IView
{
public:
	View()
	{
		std::cout << "View()" << std::endl;
	}

	void display(const std::string& message) const
	{
		std::cout << "View::display() : " << message << std::endl;
	}
};

#endif // VIEW_H
