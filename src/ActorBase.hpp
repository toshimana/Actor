#pragma once

#include <mutex>
#include <queue>
#include <atomic>


template <typename Message_>
class ActorBase
{
public:
	using Message = Message_;

	ActorBase(void)
		: halt_flag(false)
		, message_queue()
	{}

	virtual ~ActorBase( void )
	{}

	template <typename T>
	void entry( const T& msg )
	{
		if (halt_flag) return;

		std::lock_guard<std::mutex> lock(mtx);
		message_queue.push(msg);
	}

	void receive( void )
	{
		if (halt_flag) return;

		Message msg;
		{
			std::lock_guard<std::mutex> lock(mtx);
			if (message_queue.empty()) return;

			msg = message_queue.front();
			message_queue.pop();
		}

		process_message( msg );
	}

protected:
	std::atomic_bool halt_flag;

	void halt(void)
	{
		halt_flag = true;
	}

private:
	std::mutex mtx;
	std::queue<Message> message_queue;

	virtual void process_message( const Message& msg ) = 0;
};