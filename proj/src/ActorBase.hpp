#pragma once

#include <boost/thread.hpp>
#include <boost/variant.hpp>
#include <boost/lockfree/queue.hpp>

template <typename SubType, typename Message, typename MessageVisitor>
class ActorBase
{
public:
	ActorBase( void )
		: messageQueue( 128 )
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

		// Žg—pŒã‚É‰ð•ú‚Å‚«‚é‚æ‚¤‚É‚·‚é
		std::shared_ptr<Message> msg( pMsg );

		MessageVisitor mv( static_cast<SubType*>( this ) );
		boost::apply_visitor( mv, *msg );
		   
		return true;
	}

private:
	boost::lockfree::queue<Message*> messageQueue;
};