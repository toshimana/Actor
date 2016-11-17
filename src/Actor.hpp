#pragma once

#include "ActorBase.hpp"

#include <thread>
#include <chrono>

template <typename Message_>
class Actor : public ActorBase <Message_>
{
public:
	Actor( void )
		: ActorBase()
		, th( &Actor<Message_>::exec, this )
	{
	}

	virtual ~Actor( void )
	{
		halt();
		th.join();
	}

protected:
	std::thread th;

private:
	void exec( void )
	{
		while ( !halt_flag ) {
			receive();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
};
