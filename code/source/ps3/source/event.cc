
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include"cmx_event.h"

namespace cmx {
	namespace event {

		std::deque<Event > event_que;

		bool Event::procEvent() {
			if(translate == true)
			return translate_joy();
			return false;
		}
	}
}
