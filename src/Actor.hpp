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
		th.interrupt();
		th.join();
	}

protected:
	boost::thread th;

private:
	void exec( void )
	{
		while ( true ) {
			receive();
			boost::this_thread::sleep( boost::posix_time::milliseconds( 1 ) );
		}
	}
};
