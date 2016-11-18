#include <iostream>
#include <string>

#include <Actor.hpp>

struct EchoActor : public Actor<std::string>
{
	void process_message(const Message& msg)
	{
		std::cout << "received message: " << msg << std::endl;
	}
};

int main()
{
	EchoActor actor;
	actor.entry("Test");
	actor.entry("Hello");
	actor.entry("World");
	
	std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}