#include "luatest.h"
#include "lua_tinker.h"

void test1(lua_State* L);
void test2(lua_State* L);
void test3(lua_State* L);
void test4(lua_State* L);
void test5(lua_State* L);
void test6(lua_State* L);

int main()
{
	lua_State* L = lua_open();

	luaopen_base(L);

	luaopen_string(L);

	//test1(L);
	//test2(L);
	test3(L);
	//test4(L);
	//test5(L);

	lua_close(L);

	getchar();
	return 0;
}

//test1
int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

void test1(lua_State* L)
{
	lua_tinker::def(L, "cpp_func", cpp_func);

	lua_tinker::dofile(L, "sample1.lua");

	int result = lua_tinker::call<int>(L, "lua_func", 3, 4);

	printf("lua_func(3,4) = %d\n", result);
}

//test2

static int cpp_int = 100;

void test2(lua_State* L)
{
	lua_tinker::set(L, "cpp_int", cpp_int);

	lua_tinker::dofile(L, "sample2.lua");

	int lua_int = lua_tinker::get<int>(L, "lua_int");

	printf("lua_int = %d\n", lua_int);
}

//test3

struct A
{
	A(int v) : value(v) {}
	int value;
};

struct base
{
	base() {}

	const char* is_base(){ return "this is base"; }
};

class test : public base
{
public:
	test(int val) : _test(val) {}
	~test() {}

	const char* is_test(){ return "this is test"; }

	void ret_void() {}
	int ret_int()				{ return _test;			}
	int ret_mul(int m) const	{ return _test * m;		}
	A get()						{ return A(_test);		}
	void set(A a)				{ _test = a.value;		}

	int _test;
};

test g_test(11);

void test3(lua_State* L)
{
	lua_tinker::class_add<base>(L, "base");

	lua_tinker::class_def<base>(L, "is_base", &base::is_base);

	lua_tinker::class_add<test>(L, "test");
	//inheritance
	lua_tinker::class_inh<test, base>(L);
	//constructor
	lua_tinker::class_con<test>(L, lua_tinker::constructor<test,int>);

	lua_tinker::class_def<test>(L, "is_test", &test::is_test);
	lua_tinker::class_def<test>(L, "ret_void", &test::ret_void);
	lua_tinker::class_def<test>(L, "ret_int", &test::ret_int);
	lua_tinker::class_def<test>(L, "ret_mul", &test::ret_mul);
	lua_tinker::class_def<test>(L, "get", &test::get);
	lua_tinker::class_def<test>(L, "set", &test::set);
	lua_tinker::class_mem<test>(L, "_test", &test::_test);

	lua_tinker::set(L, "g_test", &g_test);

	lua_tinker::dofile(L, "sample3.lua");
}

//test4

void test4(lua_State* L)
{
	lua_tinker::table haha(L, "haha");

	haha.set("value", 1);

	haha.set("inside", lua_tinker::table(L));

	lua_tinker::table inside = haha.get<lua_tinker::table>("inside");

	inside.set("value", 2);

	lua_tinker::dofile(L, "sample4.lua");

	const char* test = haha.get<const char*>("test");
	printf("haha.test = %s\n", test);

	lua_tinker::table temp(L);

	temp.set("name", "local table !!");

	lua_tinker::call<void>(L, "print_table", temp);

	lua_tinker::table ret = lua_tinker::call<lua_tinker::table>(L, "return_table", "give me a table !!");
	printf("ret.name =\t%s\n", ret.get<const char*>("name"));
}

//test5

void show_error(const char* error)
{
	printf("_ALERT -> %s\n", error);
}

void test5(lua_State* L)
{
	printf("%s\n","-------------------------- current stack");
	lua_tinker::enum_stack(L);

	lua_pushnumber(L, 1);

	printf("%s\n","-------------------------- stack after push '1'");
	lua_tinker::enum_stack(L);

	lua_tinker::dofile(L, "sample5.lua");

	printf("%s\n","-------------------------- calling test_error()");
	lua_tinker::call<void>(L, "test_error");

	printf("%s\n","-------------------------- calling test_error_3()");
	lua_tinker::call<void>(L, "test_error_3");

	lua_tinker::def(L, "_ALERT", show_error);

	lua_tinker::call<void>(L, "_ALERT", "test !!!");

	printf("%s\n","-------------------------- calling test_error()");
	lua_tinker::call<void>(L, "test_error");
}