#include <iostream>
#include <sstream>
#include <atlbase.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <Kinect.h>

#define PEOPLE 6

// 書籍での解説のためにマクロにしています。実際には展開した形で使うことを検討してください。
#define ERROR_CHECK(ret) \
if ((ret) != S_OK) { \
	std::stringstream ss; \
	ss << "failed " #ret " " << std::hex << ret << std::endl; \
	throw std::runtime_error(ss.str().c_str()); \
}

class ShapeActor{
private:

public:
	int shape[PEOPLE];
	int actor[PEOPLE];

	void set_shape_actor_own(IBody* bodies[PEOPLE]);
	void set_shape_actor_exchange(IBody* bodies[PEOPLE]);
};
