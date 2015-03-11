#pragma once

#include "ActorBase.hpp"

#include <iostream>

template <typename SubType, typename Message, typename MessageVisitor>
class Actor : public ActorBase < SubType, Message, MessageVisitor >
{
public:
	Actor( void )
		: ActorBase()
		, th( &Actor<SubType,Message,MessageVisitor>::exec, this )
	{
		std::cout << th.get_id() << std::endl;
	}

	virtual ~Actor( void )
	{
		std::cout << "Actor Destructor" << std::endl;
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
