#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <queue>


template <typename Message>
class ActorBase
{
	using Lock = std::lock_guard<std::mutex>;

public:
	ActorBase( void )
		: messageQueue()
	{}

	virtual ~ActorBase( void )
	{}

	template <typename T>
	void entry( const T& msg )
	{
		Lock lock(mtx);
		messageQueue.push(msg);
	}

	void receive( void )
	{
		Message msg;
		{
			Lock lock(mtx);
			if (messageQueue.empty()) return;

			msg = messageQueue.front();
			messageQueue.pop();
		}

		processMessage( msg );
	}

private:
	std::mutex mtx;
	std::queue<Message> messageQueue;

	virtual void processMessage( const Message& msg ) = 0;
};