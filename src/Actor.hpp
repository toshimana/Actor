#pragma once

#include "ActorBase.hpp"

template <typename Message>
class Actor : public ActorBase <Message>
{
public:
	Actor( void )
		: ActorBase()
		, th( &Actor<Message>::exec, this )
	{
	}

	virtual ~Actor( void )
	{
		th.join();
	}

protected:
	std::thread th;

private:
	void exec( void )
	{
		while ( true ) {
			receive();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
};
