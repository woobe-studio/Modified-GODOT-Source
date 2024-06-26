/**************************************************************************/
/*  visual_server_callbacks.h                                             */
/**************************************************************************/


#ifndef VISUAL_SERVER_CALLBACKS_H
#define VISUAL_SERVER_CALLBACKS_H

#include "core/local_vector.h"
#include "core/object_id.h"
#include "core/os/mutex.h"

class VisualServerCallbacks {
public:
	enum CallbackType {
		CALLBACK_NOTIFICATION_ENTER_GAMEPLAY,
		CALLBACK_NOTIFICATION_EXIT_GAMEPLAY,
		CALLBACK_SIGNAL_ENTER_GAMEPLAY,
		CALLBACK_SIGNAL_EXIT_GAMEPLAY,
	};

	struct Message {
		CallbackType type;
		ObjectID object_id;
	};

	void lock();
	void unlock();
	void flush();

	const Message &get_message(int p_index) const { return messages[p_index]; }
	void clear() { messages.clear(); }

private:
	LocalVector<Message, int32_t> messages;
	Mutex mutex;
};

#endif // VISUAL_SERVER_CALLBACKS_H
