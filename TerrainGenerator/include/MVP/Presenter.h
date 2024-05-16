#ifndef PRESENTER_H
#define PRESENTER_H

#include "Model.h"
#include "View.h"

#include <type_traits>

template<typename Model, typename View>
class Presenter
{
	static_assert(std::is_base_of_v<IModel, Model>, "Model !! IModel");
	static_assert(std::is_base_of_v<IView, View>, "View !! IView");

public:
	Presenter()
		: m_model(new Model())
		, m_view(new View())
	{
		std::cout << "Presenter()" << std::endl;
	}

	~Presenter()
	{
		if (m_model)
		{
			delete m_model;
			m_model = nullptr;
		}
		if (m_view)
		{
			delete m_view;
			m_view = nullptr;
		}
	}

protected:
	Model* m_model;
	View* m_view;
};

#endif // PRESENTER_H
