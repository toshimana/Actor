#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <functional>
#include <boost/signals2/signal.hpp>

#include <Actor.hpp>

#include <iostream>

using namespace ::testing;

// ƒeƒXƒg—pActor
namespace TestActorMessage
{
	struct GetThreadID
	{
	};

	struct ExecFunc
	{
		ExecFunc( std::function<void(void)> _func ) : func( _func ){}
		std::function<void( void )> func;
	};

	typedef boost::variant<GetThreadID, ExecFunc> Message;
};

class TestActor : public ActorBase<TestActorMessage::Message>
{
public:
	boost::signals2::signal<void( boost::thread::id )> changeThreadID;

	TestActor( void ) : ActorBase(){}

	void connectChangeThreadID( std::function<void( boost::thread::id )> func ) 
	{
		changeThreadID.connect( func );
	}

private:
	void processMessage( std::shared_ptr<TestActorMessage::Message> msg )
	{
		MessageVisitor mv( this );
		boost::apply_visitor( mv, *msg );
	}

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( TestActor* const obj ) : base( obj ){}

		void operator()( const TestActorMessage::GetThreadID& msg ) const {
			base->changeThreadID( boost::this_thread::get_id() );
		}
		void operator()( const TestActorMessage::ExecFunc& msg ) const {
			msg.func();
		}

	private:
		TestActor* const base;
	};
};

	
class ActorTest : public ::testing::Test
{
};

TEST_F( ActorTest, noSpawnTest )
{
	auto p = std::make_shared<TestActor>();

	boost::thread::id threadID;
	p->connectChangeThreadID( [p,&threadID]( const boost::thread::id& i ){
		auto msg = new TestActorMessage::Message( TestActorMessage::ExecFunc( [i,&threadID]( void ){
			threadID = i;
		} ) );
		p->entry( msg );
	} );

	auto msg = new TestActorMessage::Message( TestActorMessage::GetThreadID() );
	p->entry( msg );

	bool ret;
	ret = p->receive(); // GetThreaID‚ÌŽÀŽ{
	ASSERT_TRUE( ret );

	ret = p->receive(); // SetThreadID‚ÌŽÀŽ{
	ASSERT_TRUE( ret );

	ASSERT_THAT( threadID, Eq( boost::this_thread::get_id() ) );
}


namespace AnotherActorMessage
{
	class AnotherActor;

	struct GetThreadID
	{
	};

	typedef boost::variant<GetThreadID> Message;

	class MessageVisitor;
};

class AnotherActor : public Actor<AnotherActorMessage::Message>
{
public:
	boost::signals2::signal<void( boost::thread::id )> changeThreadID;

	AnotherActor( void )
		: Actor()
	{}

	~AnotherActor( void )
	{
	}

	void connectChangeThreadID( std::function<void( boost::thread::id )> func ) 
	{
		changeThreadID.connect( func );
	}

	boost::thread::id getThreadID( void ) const
	{
		return th.get_id();
	}
private:
	void processMessage( std::shared_ptr<AnotherActorMessage::Message> msg )
	{
		MessageVisitor mv( this );
		boost::apply_visitor( mv, *msg );
	}

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( ::AnotherActor* const obj ) : base( obj ){}

		void operator()( const AnotherActorMessage::GetThreadID& msg ) const {
			base->changeThreadID( boost::this_thread::get_id() );
		}

	private:
		::AnotherActor* const base;
	};
};


TEST_F( ActorTest, spawnTest )
{
	auto aActor = std::make_shared<AnotherActor>();

	ASSERT_THAT( aActor->getThreadID(), Ne( boost::this_thread::get_id() ) );
	auto tActor = std::make_shared<TestActor>();

	boost::thread::id threadID;
	aActor->connectChangeThreadID( [tActor,&threadID]( const boost::thread::id& i ) {
		tActor->entry( new TestActorMessage::Message( TestActorMessage::ExecFunc( [i, &threadID]( void ) {
			threadID = i;
		} ) ) );
	} );
	aActor->entry( new AnotherActorMessage::Message( AnotherActorMessage::GetThreadID() ) );

	while ( !tActor->receive() ){}
	ASSERT_THAT( threadID, aActor->getThreadID() );
}