#pragma once

#include <boost/thread.hpp>
#include <boost/variant.hpp>
#include <boost/lockfree/queue.hpp>

template <typename Message>
class ActorBase
{
public:
	ActorBase( void )
		: messageQueue( 128 )
	{}

	virtual ~ActorBase( void )
	{}

	void entry( Message* msg )
	{
		while ( !messageQueue.push( msg ) ){
			boost::this_thread::sleep( boost::posix_time::milliseconds( 1 ) );
		}
	}

	bool receive( void )
	{
		Message* pMsg = nullptr;
		if ( !messageQueue.pop( pMsg ) ) return false;

		// �g�p��ɉ���ł���悤�ɂ���
		std::shared_ptr<Message> msg( pMsg );
		processMessage( msg );
				   
		return true;
	}

private:
	boost::lockfree::queue<Message*> messageQueue;

	virtual void processMessage( std::shared_ptr<Message> msg ) = 0;
};