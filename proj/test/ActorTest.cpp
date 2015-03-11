#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <functional>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/variant.hpp>
#include <boost/lockfree/queue.hpp>
#include <Actor.h>


#include <iostream>

using namespace ::testing;

// テスト用Actor
class TestActor : public Actor
{
public:
	struct GetThreadID
	{
	};

	struct SetThreadID
	{
		SetThreadID( void ){}
		SetThreadID( const boost::thread::id& _id ) : id( _id ){}
		boost::thread::id id;
	};

	typedef boost::variant<GetThreadID, SetThreadID> Message;

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( TestActor* const obj ) : base( obj ){}

		void operator()( const GetThreadID& msg ) const {
			std::cout << __FUNCTION__ << "GetThreadID" << std::endl;
			base->changeThreadID( boost::this_thread::get_id() );
		}
		void operator()( const SetThreadID& msg ) const {
			std::cout << __FUNCTION__ << "SetThreadID" << std::endl;
			base->threadID = msg.id;
		}

	private:
		TestActor* const base;
	};
	
	TestActor( void ) :messageQueue( 128 ){}


	void connectChangeThreadID( std::function<void( boost::thread::id )> func ) 
	{
		changeThreadID.connect( func );
	}

	void entry( Message* msg )
	{
		messageQueue.push( msg );
	}

	bool receive( void )
	{
		Message* pMsg;
		if ( !messageQueue.pop( pMsg ) ) return false;

		// 使用後に解放できるようにする
		std::shared_ptr<Message> msg( pMsg );

		MessageVisitor mv( this );
		boost::apply_visitor( mv, *msg );
		   
		return true;
	}

	boost::optional<boost::thread::id> threadID;

private:
	boost::signals2::signal<void( boost::thread::id )> changeThreadID;
	boost::lockfree::queue<Message*> messageQueue;
};

class ActorTest : public ::testing::Test
{
};

TEST_F( ActorTest, noSpawnTest )
{
	auto p = std::make_shared<TestActor>();

	p->connectChangeThreadID( [p]( const boost::thread::id& i ){
		auto msg = new TestActor::Message( TestActor::SetThreadID( i ) );
		p->entry( msg );
	} );

	auto msg = new TestActor::Message( TestActor::GetThreadID() );
	p->entry( msg );

	bool ret;
	ret = p->receive(); // GetThreaIDの実施
	ASSERT_TRUE( ret );

	ret = p->receive(); // SetThreadIDの実施
	ASSERT_TRUE( ret );

	ASSERT_TRUE( p->threadID );
	ASSERT_THAT( *(p->threadID), Eq( boost::this_thread::get_id() ) );
}