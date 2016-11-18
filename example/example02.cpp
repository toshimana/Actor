#include <iostream>
#include <string>

#include <Actor.hpp>

struct CountActor;
using SpCountActor = std::shared_ptr<CountActor>;
using WpCountActor = std::weak_ptr<CountActor>;

struct CountActor : public Actor<std::pair<int,WpCountActor> >, std::enable_shared_from_this<CountActor>
{
	void process_message(const Message& msg)
	{
		std::cout << std::this_thread::get_id() << " : " << msg.first << std::endl;

		SpCountActor sp = msg.second.lock();
		if (sp) {
			sp->entry(std::make_pair(1 + msg.first, shared_from_this()));
		}
	}
};

int main()
{
	SpCountActor actor1 = std::make_shared<CountActor>();
	SpCountActor actor2 = std::make_shared<CountActor>();

	actor1->entry( std::make_pair(0, actor2));
	
	std::this_thread::sleep_for(std::chrono::seconds(1));

	actor1->halt();
	actor2->halt();

	return 0;
}