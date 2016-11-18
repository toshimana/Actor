#pragma once

#include <mutex>
#include <queue>
#include <atomic>

// ActorBase : nonthreading actor
template <typename Message_>
class ActorBase
{
public:
	using Message = Message_;

	ActorBase(void)
		: halt_flag(false)
		, message_queue()
	{}

	virtual ~ActorBase(void)
	{}

	template <typename T>
	void entry(const T& msg)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (halt_flag) return;
		message_queue.push(msg);
	}

	void receive(void)
	{
		Message msg;
		{
			std::lock_guard<std::mutex> lock(mtx);

			if (halt_flag) return;
			if (message_queue.empty()) return;

			msg = message_queue.front();
			message_queue.pop();
		}

		process_message(msg);
	}

protected:
	std::atomic_bool halt_flag;

	void to_halt(void)
	{
		std::lock_guard<std::mutex> lock(mtx);
		halt_flag = true;
	}

private:
	std::mutex mtx;
	std::queue<Message> message_queue;

	virtual void process_message(const Message& msg) = 0;
};


#include <thread>
#include <chrono>

// Actor : running ActorBase on unique thread
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
		if (!halt_flag) halt();
	}

	void halt() { 
		to_halt(); 
		th.join();
	}

protected:
	std::thread th;

private:
	void exec( void )
	{
		while ( !halt_flag ) {
			receive();
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}
};
